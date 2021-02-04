#include "SimCore/DetectorConstruction.h"

#include "Framework/Exception/Exception.h"
#include "SimCore/PluginFactory.h"
#include "SimCore/XsecBiasingOperator.h"

namespace simcore {

DetectorConstruction::DetectorConstruction(
    G4GDMLParser* theParser, framework::config::Parameters& parameters,
    ConditionsInterface& ci)
    : parser_(theParser),
      auxInfoReader_(new AuxInfoReader(theParser, parameters, ci)) {
  parameters_ = parameters;
}

DetectorConstruction::~DetectorConstruction() { delete auxInfoReader_; }

G4VPhysicalVolume* DetectorConstruction::Construct() {
  auxInfoReader_->readGlobalAuxInfo();
  auxInfoReader_->assignAuxInfoToVolumes();
  return parser_->GetWorldVolume();
}

void DetectorConstruction::ConstructSDandField() {
  // Biasing operators were created in RunManager::setupPhysics
  //  which is called before G4RunManager::Initialize
  //  which is where this method ends up being called.

  auto bops{simcore::PluginFactory::getInstance().getBiasingOperators()};
  for (simcore::XsecBiasingOperator* bop : bops) {
    for (G4LogicalVolume* volume : *G4LogicalVolumeStore::GetInstance()) {
      if (bop->getVolumeToBias().compare("ecal") == 0) {
        // TODO this is _horrible_
        //  can we get an 'ecal' and 'hcal' region instead
        //  of just a 'CalorimeterRegion' region?
        G4String volumeName = volume->GetName();
        if ((volumeName.contains("Wthick") || volumeName.contains("Si") ||
             volumeName.contains("W") || volumeName.contains("PCB") ||
             volumeName.contains("CFMix") || volumeName.contains("Al")) &&
            volumeName.contains("volume")) {
          bop->AttachTo(volume);
          std::cout << "[ DetectorConstruction ]: "
                    << "Attaching biasing operator " << bop->GetName()
                    << " to volume " << volume->GetName() << std::endl;
        }  // volume matches pattern for ecal volumes
      } else if (bop->getVolumeToBias().compare("target") == 0) {
        auto region = volume->GetRegion();
        if (region and region->GetName().contains("target")) {
          bop->AttachTo(volume);
          std::cout << "[ DetectorConstruction ]: "
                    << "Attaching biasing operator " << bop->GetName()
                    << " to volume " << volume->GetName() << std::endl;
        }  // volume is in target region
      }    // BOP attached to target or ecal
    }      // loop over volumes
  }        // loop over biasing operators
}
}  // namespace simcore
