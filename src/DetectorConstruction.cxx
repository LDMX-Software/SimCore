#include "SimCore/DetectorConstruction.h"

#include "SimCore/G4eDarkBremsstrahlung.h"

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Exception.h" 

namespace ldmx {

    DetectorConstruction::DetectorConstruction(G4GDMLParser* theParser, Parameters& parameters) :
            parser_(theParser), auxInfoReader_(new AuxInfoReader(theParser, parameters)) {
                parameters_ = parameters; 
    }

    DetectorConstruction::~DetectorConstruction() {
        delete auxInfoReader_;
    }

    G4VPhysicalVolume* DetectorConstruction::Construct() {
        auxInfoReader_->readGlobalAuxInfo();
        auxInfoReader_->assignAuxInfoToVolumes();
        return parser_->GetWorldVolume();
    }

    void DetectorConstruction::ConstructSDandField() {

        auto biasingEnabled{parameters_.getParameter< bool >("biasing_enabled")};
        if (biasingEnabled) {

            auto biasingProcess{parameters_.getParameter< std::string >("biasing_process")}; 
            auto biasingVolume{parameters_.getParameter< std::string >("biasing_volume")};
            auto biasingParticle{parameters_.getParameter< std::string >("biasing_particle")}; 
            auto biasAll{parameters_.getParameter< bool >("biasing_all")}; 
            auto biasIncident{parameters_.getParameter< bool >("biasing_incident")}; 
            auto disableEMBiasing{parameters_.getParameter< bool >("biasing_disableEMBiasing")};
            auto biasThreshold{parameters_.getParameter< double >("biasing_threshold")}; 
            auto biasFactor{parameters_.getParameter< double >("biasing_factor")}; 

            // Instantiate the biasing operator
            // TODO: At some point, this should be more generic i.e. operators should be
            //       similar to plugins.
            XsecBiasingOperator* xsecBiasing{nullptr}; 
            if (biasingProcess.compare("photonNuclear") == 0) { 
                xsecBiasing = new PhotoNuclearXsecBiasingOperator("PhotoNuclearXsecBiasingOperator");
            } else if (biasingProcess.compare("GammaToMuPair") == 0) { 
                xsecBiasing = new GammaToMuPairXsecBiasingOperator("GammaToMuPairXsecBiasingOperator");
            } else if (biasingProcess.compare("electronNuclear") == 0) { 
                xsecBiasing = new ElectroNuclearXsecBiasingOperator("ElectroNuclearXsecBiasingOperator");
            } else if (biasingProcess.compare(G4eDarkBremsstrahlung::PROCESS_NAME) == 0) { 
                xsecBiasing = new DarkBremXsecBiasingOperator("DarkBremXsecBiasingOperator");
            } else {
                EXCEPTION_RAISE("BiasingException", "Invalid process name '" + biasingProcess + "'." ); 
            }

            // Configure the operator
            xsecBiasing->setParticleType(biasingParticle);
            xsecBiasing->setThreshold(biasThreshold); 
            xsecBiasing->setBiasFactor(biasFactor); 

            if (biasAll) xsecBiasing->biasAll(); 
            else if (biasIncident) xsecBiasing->biasIncident();
            
            if (disableEMBiasing) xsecBiasing->disableBiasDownEM(); 


            for (G4LogicalVolume* volume : *G4LogicalVolumeStore::GetInstance()) {
                if (biasingVolume.compare("ecal") == 0) {
                    G4String volumeName = volume->GetName();
                    if ((
                               volumeName.contains("Wthick") 
                            || volumeName.contains("Si")
                            || volumeName.contains("W") 
                            || volumeName.contains("PCB")
                            || volumeName.contains("CFMix")
                        ) && volumeName.contains("volume")
                    ) {
                        xsecBiasing->AttachTo(volume);
                        std::cout << "[ DetectorConstruction ]: " 
                                  << "Attaching biasing operator " << xsecBiasing->GetName() 
                                  << " to volume " << volume->GetName() << std::endl;
                    } //volumeName matches the pattern of volumes within ECal
                } else if (biasingVolume.compare("target") == 0) {
                    auto region=volume->GetRegion();
                    if (region and region->GetName().contains("target")) {
                        xsecBiasing->AttachTo(volume);
                        std::cout << "[ DetectorConstruction ]: " 
                                  << "Attaching biasing operator " << xsecBiasing->GetName() 
                                  << " to volume " << volume->GetName() << std::endl;
                    } //name of region that contains this volume is 'target'
                }//trying to bias target or ecal
            } //loop over logical volumes

        } //biasing is enabled
    } //ConstructSDandField
}
