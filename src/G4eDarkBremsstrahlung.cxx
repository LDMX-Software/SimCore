/**
 * @file G4eDarkBremsstrahlung.cxx
 * @brief Class providing the Dark Bremsstrahlung process class.
 * @author Michael Revering, University of Minnesota
 * @author Tom Eichlersmith, University of Minnesota
 */

#include "SimCore/G4eDarkBremsstrahlung.h"
#include "SimCore/G4APrime.h"
#include "SimCore/G4eDarkBremsstrahlungModel.h"

#include "G4DynamicParticle.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ProcessType.hh"

namespace ldmx {

    G4eDarkBremsstrahlung::G4eDarkBremsstrahlung(Parameters& params, const G4String& name):
        G4VEnergyLossProcess(name),
        parameters_(params),
        isInitialised(false) {  
    
        G4int subtype = 63;   
        SetProcessSubType(subtype);
        SetSecondaryParticle(G4APrime::APrime());
        SetIonisation(false);
    }
    
    G4bool G4eDarkBremsstrahlung::IsApplicable(const G4ParticleDefinition& p) {
        return &p == G4Electron::Electron();
    }
    
    void G4eDarkBremsstrahlung::PrintInfo() {
        G4cout << " A' Mass [MeV]                    : " << parameters_.get<double>("APrimeMass") << G4endl;
        G4cout << " Vertex Library                   : " << parameters_.get<std::string>("library_path") << G4endl;
        G4cout << " Interpretation Method            : " << parameters_.get<int>("method") << G4endl;
        G4cout << " Minimum Electron Threshold [MeV] : " << parameters_.get<int>("threshold") << G4endl;
        G4cout << " Epsilon                          : " << parameters_.get<double>("epsilon") << G4endl;
        G4cout << " Only One Per Event               : " << parameters_.get<bool>("only_one_per_event") << G4endl;
    }
    
    void G4eDarkBremsstrahlung::InitialiseEnergyLossProcess(const G4ParticleDefinition*,
                                                            const G4ParticleDefinition*) {
        if(!isInitialised) {
    
            this->SetEmModel(new G4eDarkBremsstrahlungModel(parameters_),0); //adds model to vector stored in process
    
            //TODO: could make this depend on maximum beam energy passed through LHE files?
            G4double energyLimit = 4*GeV;
    
            this->EmModel(0)->SetLowEnergyLimit(MinKinEnergy());
            this->EmModel(0)->SetHighEnergyLimit(energyLimit);
            
            G4VEmFluctuationModel* fm = 0;
            //adds model to ModelManager which handles initialisation procedures and cleaning up pointers
            this->AddEmModel(0, EmModel(0), fm); 
    
            isInitialised = true;
        }
    
        G4double eth = 0*MeV;
        this->EmModel(0)->SetSecondaryThreshold(eth);
        this->EmModel(0)->SetLPMFlag(false);
    }
    
}
