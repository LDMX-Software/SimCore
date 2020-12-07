#ifndef SIMCORE_PLUGINFACTORY_H
#define SIMCORE_PLUGINFACTORY_H

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Configure/Parameters.h"
#include "SimCore/PrimaryGenerator.h"
#include "SimCore/USteppingAction.h"
#include "SimCore/UserEventAction.h"
#include "SimCore/UserRunAction.h"
#include "SimCore/UserStackingAction.h"
#include "SimCore/UserTrackingAction.h"
#include "SimCore/UserAction.h"

namespace simcore {

/**
 * @typedef action
 * Holding references to all the differenty types of actions
 * we could have.
 */
typedef std::variant<UserRunAction*, UserEventAction*, UserTrackingAction*,
                     USteppingAction*, UserStackingAction*>
    action;

/**
 * A map of the different types of actions to 
 * their reference.
 */
typedef std::map<TYPE, action> actionMap;

/**
 * @class PluginFactory
 * @brief Class that manages the generators used to fire particles.
 */
class PluginFactory {
 public:
  /// @return the global PluginFactory instance
  static PluginFactory& getInstance();

  /**
   * Get the collection of all enabled generators
   */
  std::vector<PrimaryGenerator*> getGenerators() const { return generators_; };

  /**
   * Attach a new generator to the list of generators
   */
  void registerGenerator(const std::string& className,
                         PrimaryGeneratorBuilder* builder);

  /**
   * Create a new generate and attach it to the list of generators
   */
  void createGenerator(const std::string& className,
                    const std::string& instanceName, ldmx::Parameters& parameters);

  /**
   * Get all of the actions.
   */
  actionMap getActions();

  /**
   *
   */
  void registerAction(const std::string& className, UserActionBuilder* builder);

  /**
   *
   */
  void createAction(const std::string& className,
                    const std::string& instanceName, ldmx::Parameters& parameters);

  /**
   */
  std::vector<XsecBiasingOperator*> getBiasingOperators() const { return biasing_operators_; }

  /**
   *
   */
  void registerBiasingOperator(const std::string& className, XsecBiasingOperatorBuilder* builder);

  /**
   *
   */
  void createBiasingOperator(const std::string& className,
                    const std::string& instanceName, ldmx::Parameters& parameters);

 private:
  /// PluginFactory instance
  static PluginFactory instance_;

  /// Constructor - private to prevent initialization
  PluginFactory() {}

  /**
   * @struct GeneratorInfo
   * @brief Holds necessary information to create a generator
   */
  struct GeneratorInfo {
    /// Name of the Class
    std::string className_;

    /// Class builder
    PrimaryGeneratorBuilder* builder_;
  };

  /// A map of all register generators
  std::map<std::string, GeneratorInfo> generatorMap_;

  /// Cointainer for all generators to be used by the simulation
  std::vector<PrimaryGenerator*> generators_;

  /**
   * @struct ActionInfo
   * @brief Encapsulates the information required to create a UserAction
   */
  struct ActionInfo {
    /// Name of the class
    std::string className_;

    /// Class builder
    UserActionBuilder* builder_;
  };

  /// A map of all registered user actions to their corresponding info.
  std::map<std::string, ActionInfo> actionInfo_;

  /// Container for all Geant4 actions
  actionMap actions_;

  /**
   * @struct BiasingOperatorInfo
   * @brief Encapsulates the information required to create a XsecBiasingOperator
   */
  struct BiasingOperatorInfo {
    /// Name of the class
    std::string className_;

    /// Class builder
    XsecBiasingOperatorBuilder* builder_;
  };

  /// A map of all registered user actions to their corresponding info.
  std::map<std::string, BiasingOperatorInfo> biasingInfo_;

  /// Container for all biasing operators
  std::vector<XsecBiasingOperator*> biasing_operators_;

};  // PluginFactory

}  // namespace simcore

#endif  // SIMCORE_PLUGINFACTORY_H
