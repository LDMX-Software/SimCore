#ifndef SIMCORE_PERSIST_PERSISTENCYMANAGERFACTORY_H_
#define SIMCORE_PERSIST_PERSISTENCYMANAGERFACTORY_H_

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/ConditionsInterface.h"
#include "SimCore/Persist/PersistencyManager.h"

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <map>

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Configure/Parameters.h"
#include "Framework/EventFile.h"

namespace simcore {
namespace persist {

class PersistencyManagerFactory {

public:
  /// Get the static instance of this class
  static PersistencyManagerFactory *getInstance();

  /// Default constructor
  ~PersistencyManagerFactory() = default;

  /**
   * Create an instance of the parser of the given type.
   *
   */
  std::unique_ptr<PersistencyManager>
  createPersistencyManager(const std::string &type, framework::EventFile &file,
                           ldmx::Parameters &parameters, const int &run_number,
                           ConditionsInterface &ci);

private:
  /// Static instance of this class
  static PersistencyManagerFactory *instance_;

  /// Default constructor
  PersistencyManagerFactory();

  /**
   */
  void registerPersistencyManager(const std::string &type, createFunc create);

  /// Mapping between a persistency manager and its create function
  std::map<std::string, createFunc> persistency_manager_map;
};
} // namespace persist
} // namespace simcore

#endif // SIMCORE_PERSIST_PERSISTENCYMANAGER_
