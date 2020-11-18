
#include "SimCore/Persist/PersistencyManagerFactory.h"

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/Persist/RootPersistencyManager.h"
#include "SimCore/Persist/LcioPersistencyManager.h"

namespace simcore {
namespace persist {

PersistencyManagerFactory *PersistencyManagerFactory::instance_ = nullptr;

PersistencyManagerFactory *PersistencyManagerFactory::getInstance() {
  if (!instance_)
    instance_ = new PersistencyManagerFactory;
  return instance_;
}

PersistencyManagerFactory::PersistencyManagerFactory() {
  registerPersistencyManager("root", &RootPersistencyManager::create);
  registerPersistencyManager("lcio", &LcioPersistencyManager::create);
}

void PersistencyManagerFactory::registerPersistencyManager(
    const std::string &type, createFunc create) {
  persistency_manager_map[type] = create;
}

std::unique_ptr<PersistencyManager>
PersistencyManagerFactory::createPersistencyManager(
    const std::string &type, framework::EventFile &file,
    ldmx::Parameters &parameters, const int &run_number,
    ConditionsInterface &ci) {

  auto it{persistency_manager_map.find(type)};
  if (it != persistency_manager_map.end())
    return it->second(file, parameters, run_number, ci);
  return nullptr;
}

} // namespace persist
} // namespace simcore
