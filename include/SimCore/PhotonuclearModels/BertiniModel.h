#ifndef SIMCORE_BERTINI_MODEL_H
#define SIMCORE_BERTINI_MODEL_H

#include <G4ProcessManager.hh>

#include "SimCore/PhotonuclearModel.h"
namespace simcore {

class BertiniModel : public PhotonuclearModel {
 public:
  BertiniModel(const std::string& name,
               const framework::config::Parameters& parameters)
      : PhotonuclearModel{name, parameters} {}
  virtual ~BertiniModel() = default;
  void ConstructModel(G4ProcessManager*) override {}
};

}  // namespace simcore

#endif /* SIMCORE_BERTINI_MODEL_H */
