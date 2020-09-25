/**
 * @file G4eDarkBremsstrahlung.cxx
 * @brief Class providing the Dark Bremsstrahlung process class.
 * @author Michael Revering, University of Minnesota
 * @author Tom Eichlersmith, University of Minnesota
 */

#include "SimCore/G4eDarkBremsstrahlung.h"
#include "SimCore/G4APrime.h"
#include "SimCore/DarkBremVertexLibraryModel.h"

#include "Event/RunHeader.h"

#include "G4Electron.hh" //for electron definition
#include "G4ProcessTable.hh" //for deactivating dark brem process
#include "G4ProcessType.hh" //for type of process
#include "G4RunManager.hh" //for VerboseLevel
#include "G4EventManager.hh" //for EventID number

namespace ldmx {

    const std::string G4eDarkBremsstrahlung::PROCESS_NAME = "eDarkBrem";

    G4eDarkBremsstrahlung::G4eDarkBremsstrahlung(const Parameters& params)
        : G4VDiscreteProcess(G4eDarkBremsstrahlung::PROCESS_NAME,fUserDefined) {  
        SetProcessSubType(63); //don't know what this number means or does
        aParticleChange.SetSecondaryWeightByProcess(true); //turn on weighting through this process
    
        onlyOnePerEvent_ = params.getParameter<bool>("only_one_per_event");
        ap_mass_         = params.getParameter<double>("ap_mass");

        auto model{params.getParameter<Parameters>("model")};
        auto model_name{model.getParameter<std::string>("name")};
        if ( model_name == "vertex_library" ) {
            theModel_ = std::make_unique<DarkBremVertexLibraryModel>(model);
        } else {
            EXCEPTION_RAISE("DarkBremModel","Model named '"+model_name+"' is not known.");
        }
    }
    
    G4bool G4eDarkBremsstrahlung::IsApplicable(const G4ParticleDefinition& p) {
        return &p == G4Electron::Electron();
    }
    
    void G4eDarkBremsstrahlung::PrintInfo() {
        G4cout << " Only One Per Event               : " << onlyOnePerEvent_ << G4endl;
        G4cout << " A' Mass [MeV]                    : " << ap_mass_ << G4endl;
        theModel_->PrintInfo();
    }

    void G4eDarkBremsstrahlung::RecordConfig(RunHeader& h) const {
        h.setIntParameter( "Only One DB Per Event" , onlyOnePerEvent_ );
        h.setFloatParameter( "A' Mass [MeV]" , ap_mass_ );
        theModel_->RecordConfig(h);
    }
    
    G4VParticleChange* G4eDarkBremsstrahlung::PostStepDoIt(const G4Track& track, const G4Step& step) {

        // Debugging Purposes: Check if track we get is an electron
        if (not IsApplicable(*track.GetParticleDefinition()))
            EXCEPTION_RAISE("DBBadTrack","Dark brem process receieved a track that isn't applicable.");

        /*
         * Geant4 has decided that it is our time to interact,
         * so we are going to change the particle
         */
        //if ( G4RunManager::GetRunManager()->GetVerboseLevel() > 1 ) {
            std::cout << "[ G4eDarkBremsstrahlung ] : "
                << "(" << G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID() << ") "
                << "A dark brem occurred! "
                << std::endl;
        //}

        if ( onlyOnePerEvent_ ) {
            //Deactivate the process after one dark brem. Needs to be reactivated in the end of event action. 
            //If this is in the stepping action instead, more than one brem can occur within each step.
            G4bool state = false;
            G4String pname = "biasWrapper("+PROCESS_NAME+")";
            G4ProcessTable* ptable = G4ProcessTable::GetProcessTable();
            ptable->SetProcessActivation(pname,state);
        }

        aParticleChange.Initialize(track);

        theModel_->GenerateChange( aParticleChange , track , step );

        /* 
         * Parent class has some internal counters that need to be reset,
         * so we call it before returning. It will return our shared
         * protected member variable aParticleChange that we have been modifying
         */
        return G4VDiscreteProcess::PostStepDoIt(track,step);
    }

    G4double G4eDarkBremsstrahlung::GetMeanFreePath(const G4Track& track, G4double, G4ForceCondition*) {

        //won't happen if it isn't applicable
        if (not IsApplicable(*track.GetParticleDefinition())) return DBL_MAX;

        G4Material* materialWeAreIn = track.GetMaterial();
        const G4ElementVector* theElementVector = materialWeAreIn->GetElementVector();
        const G4double* NbOfAtomsPerVolume = materialWeAreIn->GetVecNbOfAtomsPerVolume();
     
        G4double SIGMA = 0;
        for ( size_t i=0 ; i < materialWeAreIn->GetNumberOfElements() ; i++ ) {
            G4double AtomicZ = (*theElementVector)[i]->GetZ();
            G4double AtomicA = (*theElementVector)[i]->GetA()/(g/mole);
            SIGMA += NbOfAtomsPerVolume[i] *
                theModel_->ComputeCrossSectionPerAtom(
                        track.GetDynamicParticle()->GetKineticEnergy(),
                        AtomicA,
                        AtomicZ
                        );
        }

        return SIGMA > DBL_MIN ? 1./SIGMA : DBL_MAX;
    }
}
