
#include "SimCore/PluginFactory.h"

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <dlfcn.h>
#include <algorithm>
#include <string>
#include <vector>

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Exception/Exception.h"

simcore::PluginFactory simcore::PluginFactory::instance_
    __attribute__((init_priority(200)));

namespace simcore {

PluginFactory& PluginFactory::getInstance() { return instance_; }

void PluginFactory::registerGenerator(const std::string& className,
                                      ldmx::PrimaryGeneratorBuilder* builder) {
  GeneratorInfo info;
  info.className_ = className;
  info.builder_ = builder;

  generatorMap_[className] = info;
}

void PluginFactory::createGenerator(const std::string& className,
                                    const std::string& instanceName,
                                    ldmx::Parameters& parameters) {
  auto it{generatorMap_.find(className)};
  if (it == generatorMap_.end()) {
    EXCEPTION_RAISE("CreateGenerator",
                    "Failed to create generator '" + className + "'.");
  }

  auto generator{it->second.builder_(instanceName, parameters)};

  // now that the generator is built --> put it on active list
  generators_.push_back(generator);
}

actionMap PluginFactory::getActions() {
  if (actions_.empty()) {
    actions_[ldmx::TYPE::RUN] = new ldmx::UserRunAction();
    actions_[ldmx::TYPE::EVENT] = new ldmx::UserEventAction();
    actions_[ldmx::TYPE::TRACKING] = new ldmx::UserTrackingAction();
    actions_[ldmx::TYPE::STEPPING] = new ldmx::USteppingAction();
    actions_[ldmx::TYPE::STACKING] = new ldmx::UserStackingAction();
  }

  return actions_;
}

void PluginFactory::registerAction(const std::string& className,
                                   ldmx::UserActionBuilder* builder) {
  auto it{actionInfo_.find(className)};
  if (it != actionInfo_.end()) {
    EXCEPTION_RAISE(
        "ExistingEventProcessorDefinition",
        "The user action " + className + " has already been registered.");
  }

  ActionInfo info;
  info.className_ = className;
  info.builder_ = builder;

  actionInfo_[className] = info;
}

void PluginFactory::createAction(const std::string& className,
                                 const std::string& instanceName,
                                 ldmx::Parameters& parameters) {
  auto it{actionInfo_.find(className)};
  if (it == actionInfo_.end()) {
    EXCEPTION_RAISE("PluginFactory", "Failed to create " + className);
  }

  auto act{it->second.builder_(instanceName, parameters)};

  std::vector<ldmx::TYPE> types = act->getTypes();
  for (auto& type : types) {
    if (type == ldmx::TYPE::RUN)
      std::get<ldmx::UserRunAction*>(actions_[ldmx::TYPE::RUN])->registerAction(act);
    else if (type == ldmx::TYPE::EVENT)
      std::get<ldmx::UserEventAction*>(actions_[ldmx::TYPE::EVENT])->registerAction(act);
    else if (type == ldmx::TYPE::TRACKING)
      std::get<ldmx::UserTrackingAction*>(actions_[ldmx::TYPE::TRACKING])
          ->registerAction(act);
    else if (type == ldmx::TYPE::STEPPING)
      std::get<ldmx::USteppingAction*>(actions_[ldmx::TYPE::STEPPING])->registerAction(act);
    else if (type == ldmx::TYPE::STACKING)
      std::get<ldmx::UserStackingAction*>(actions_[ldmx::TYPE::STACKING])
          ->registerAction(act);
    else
      EXCEPTION_RAISE("PluginFactory", "User action type doesn't exist.");
  }
}

void PluginFactory::registerBiasingOperator(
    const std::string& className, XsecBiasingOperatorBuilder* builder) {
  BiasingOperatorInfo info;
  info.className_ = className;
  info.builder_ = builder;

  biasingInfo_[className] = info;
}

void PluginFactory::createBiasingOperator(const std::string& className,
                                          const std::string& instanceName,
                                          ldmx::Parameters& parameters) {
  auto it{biasingInfo_.find(className)};
  if (it == biasingInfo_.end()) {
    EXCEPTION_RAISE("CreateBiasingOperator",
                    "Failed to create biasing '" + className + "'.");
  }

  auto bop{it->second.builder_(instanceName, parameters)};

  // now that the biasing is built --> put it on active list
  biasing_operators_.push_back(bop);
}

}  // namespace simcore
