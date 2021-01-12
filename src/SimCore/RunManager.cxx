/**
 * @file RunManager.cxx
 * @brief Class providing a Geant4 run manager implementation.
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 */

#include "SimCore/RunManager.h"

//-------------//
//   ldmx-sw   //
//-------------//
#include "SimCore/APrimePhysics.h"
#include "SimCore/DetectorConstruction.h"
#include "SimCore/GammaPhysics.h"
#include "SimCore/ParallelWorld.h"
#include "SimCore/PrimaryGeneratorAction.h"
#include "SimCore/USteppingAction.h"
#include "SimCore/UserEventAction.h"
#include "SimCore/UserRunAction.h"
#include "SimCore/UserStackingAction.h"
#include "SimCore/UserTrackingAction.h"
#include "SimCore/PluginFactory.h"

//------------//
//   Geant4   //
//------------//
#include "FTFP_BERT.hh"
#include "G4GDMLParser.hh"
#include "G4GenericBiasingPhysics.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4ProcessTable.hh"
#include "G4VModularPhysicsList.hh"

namespace simcore {

RunManager::RunManager(framework::config::Parameters& parameters, ConditionsInterface& ci)
    : conditionsIntf_(ci) {
  parameters_ = parameters;

  // Set whether the ROOT primary generator should use the persisted seed.
  auto rootPrimaryGenUseSeed{
      parameters.getParameter<bool>("rootPrimaryGenUseSeed")};

  // Validate the geometry if specified.
  setUseRootSeed(rootPrimaryGenUseSeed);
}

RunManager::~RunManager() {}

void RunManager::setupPhysics() {
  auto pList{physicsListFactory_.GetReferencePhysList("FTFP_BERT")};

  parallelWorldPath_ = parameters_.getParameter<std::string>("scoringPlanes");
  isPWEnabled_ = !parallelWorldPath_.empty();
  if (isPWEnabled_) {
    std::cout
        << "[ RunManager ]: Parallel worlds physics list has been registered."
        << std::endl;
    pList->RegisterPhysics(new G4ParallelWorldPhysics("ldmxParallelWorld"));
  }

  pList->RegisterPhysics(new GammaPhysics);
  pList->RegisterPhysics(new APrimePhysics(parameters_));

  auto biasing_operators{parameters_.getParameter<std::vector<framework::config::Parameters>>(
      "biasing_operators", {})};
  if (!biasing_operators.empty()) {

    std::cout << "[ RunManager ]: Biasing enabled with "
      << biasing_operators.size() << " operator(s)." << std::endl;

    // create all the biasing operators that will be used
    auto& factory{simcore::PluginFactory::getInstance()};
    for (const framework::config::Parameters& bop : biasing_operators) {
      factory.createBiasingOperator(
          bop.getParameter<std::string>("class_name"),
          bop.getParameter<std::string>("instance_name"), bop);
    }

    // Instantiate the constructor used when biasing
    G4GenericBiasingPhysics* biasingPhysics = new G4GenericBiasingPhysics();

    // specify which particles are going to be biased
    //  this will put a biasing interface wrapper around *all* processes
    //  associated with these particles
    for (const XsecBiasingOperator* bop : factory.getBiasingOperators()) {
      std::cout << "[ RunManager ]: Biasing operator '" << bop->GetName()
                << "' set to bias " << bop->getParticleToBias() << std::endl;
      biasingPhysics->Bias(bop->getParticleToBias());
    }

    // Register the physics constructor to the physics list:
    pList->RegisterPhysics(biasingPhysics);
  }

  this->SetUserInitialization(pList);
}

void RunManager::Initialize() {
  setupPhysics();

  // The parallel world needs to be registered before the mass world is
  // constructed i.e. before G4RunManager::Initialize() is called.
  if (isPWEnabled_) {
    std::cout << "[ RunManager ]: Parallel worlds have been enabled."
              << std::endl;

    auto validateGeometry_{parameters_.getParameter<bool>("validate_detector")};
    G4GDMLParser* pwParser = new G4GDMLParser();
    pwParser->Read(parallelWorldPath_, validateGeometry_);
    this->getDetectorConstruction()->RegisterParallelWorld(
        new ParallelWorld(pwParser, "ldmxParallelWorld", conditionsIntf_));
  }

  G4RunManager::Initialize();

  // Instantiate the primary generator action
  auto primaryGeneratorAction{new PrimaryGeneratorAction(parameters_)};
  SetUserAction(primaryGeneratorAction);

  // Get the plugin factory so we can get the actions
  auto& factory{simcore::PluginFactory::getInstance()};

  // Get instances of all G4 actions
  //      also create them in the action manager
  auto actions{factory.getActions()};

  // Create all user actions
  auto userActions{
      parameters_.getParameter<std::vector<framework::config::Parameters> >("actions", {})};
  for (auto& userAction : userActions) {
    factory.createAction(
        userAction.getParameter<std::string>("class_name"),
        userAction.getParameter<std::string>("instance_name"), userAction);
  }

  // Register all actions with the G4 engine
  for (const auto& [key, act] : actions) {
    std::visit([this](auto&& arg) { this->SetUserAction(arg); }, act);
  }
}

void RunManager::TerminateOneEvent() {
  // have geant4 do its own thing
  G4RunManager::TerminateOneEvent();

  // reset dark brem process (if needed)
  G4ProcessTable* ptable = G4ProcessTable::GetProcessTable();
  G4int verbosity = ptable->GetVerboseLevel();
  // silent ptable while searching for process that may/may not exist
  ptable->SetVerboseLevel(0);  
  // TODO allow eDBrem to be biased or unbiased
  G4String pname = "biasWrapper(eDBrem)";  
  bool active = true;
  ptable->SetProcessActivation(pname, active);
  if (this->GetVerboseLevel() > 1) {
    std::cout << "[ RunManager ] : "
              << "Reset the dark brem process (if it was activated)."
              << std::endl;
  }
  ptable->SetVerboseLevel(verbosity);
}

DetectorConstruction* RunManager::getDetectorConstruction() {
  return static_cast<DetectorConstruction*>(this->userDetector);
}

}  // namespace simcore
