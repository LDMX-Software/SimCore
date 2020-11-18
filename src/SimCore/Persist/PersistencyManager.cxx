
#include "SimCore/Persist/PersistencyManager.h"

namespace simcore {
namespace persist {

PersistencyManager::PersistencyManager(framework::EventFile &file,
                     ldmx::Parameters &parameters, const int &run_number, 
                     ldmx::ConditionsInterface &ci) 
    : G4PersistencyManager(G4PersistencyCenter::GetPersistencyCenter(),
                           "PersistencyManager"),
      file_(file), parameters_(parameters), run_(run_number) {

  // Let Geant4 know what to use this persistency manager
  G4PersistencyCenter::GetPersistencyCenter()->RegisterPersistencyManager(this);
  G4PersistencyCenter::GetPersistencyCenter()->SetPersistencyManager(
      this, "PersistencyManager");
}
} // namespace persist
} // namespace simcore
