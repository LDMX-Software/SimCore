#ifndef SIMCORE_PHOTONUCLEAR_MODEL_H
#define SIMCORE_PHOTONUCLEAR_MODEL_H
#include <G4ProcessManager.hh>
#include <string>
#include <utility>

#include "Framework/Configure/Parameters.h"
#include "SimCore/Factory.h"
namespace simcore {
class PhotonuclearModel {
 public:
  PhotonuclearModel(const std::string& name,
                    const framework::config::Parameters& parameters) {
    std::cout << "Creating PN model with name " << name << std::endl;
  }
  virtual ~PhotonuclearModel() = default;
  virtual void ConstructModel(G4ProcessManager* processManager) = 0;
  using Factory =
      ::simcore::Factory<PhotonuclearModel, std::shared_ptr<PhotonuclearModel>,
                         const std::string&,
                         const framework::config::Parameters&>;

  virtual void removeExistingModel(G4ProcessManager* processManager) {
    std::cout << "Starting to remove..." << std::endl;
    const auto processes{processManager->GetProcessList()};
    for (int i{0}; i < processes->size(); ++i) {
      const auto process{(*processes)[i]};
      const auto processName{process->GetProcessName()};
      if (processName == "photonNuclear") {
        std::cout << "Removing process " << processName << std::endl;
        processManager->RemoveProcess(i);
      }
    }
  }

 private:
};
}  // namespace simcore

#define DECLARE_PHOTONUCLEAR_MODEL(CLASS)                                 \
  namespace {                                                             \
  auto v = ::simcore::PhotonuclearModel::Factory::get().declare<CLASS>(); \
  }
#endif /* SIMCORE_PHOTONUCLEAR_MODEL_H */
