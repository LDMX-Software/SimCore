#ifndef SIMCORE_PERSIST_ROOTPERSISTENCYMANAGER_H_
#define SIMCORE_PERSIST_ROOTPERSISTENCYMANAGER_H

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <string>
#include <vector>

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/EcalHitIO.h"
#include "SimCore/G4CalorimeterHit.h"
#include "SimCore/G4TrackerHit.h"
#include "SimCore/Persist/PersistencyManager.h"
#include "SimCore/Persist/SimParticleBuilder.h"

/*~~~~~~~~~~~~*/
/*   Geant4   */
/*~~~~~~~~~~~~*/
#include "G4PersistencyCenter.hh"
#include "G4PersistencyManager.hh"

// Forward declarations
class G4Run;

namespace framework { 
class Event;
}

// Forward declarations within the ldmx namespace
namespace ldmx {
class RunHeader;
} // namespace ldmx

using namespace ldmx;

namespace simcore {
namespace persist {

/**
 * @class RootPersistencyManager
 *
 * @note
 * Output is written at the end of each event.  An EventFile is used to
 * write from an Event buffer object into an output branch within a tree.
 * The event buffer is cleared after the event is written.  A
 * SimParticleBuilder is used to build a set of SimParticle objects from
 * the Trajectory objects which were created during event processing. An
 * EcalHitIO instance provides translation of G4CalorimeterHit objects in
 * the ECal to an output SimCalorimeterHit collection, transforming the
 * individual steps into cell energy depositions.  The tracker hit
 * collections of G4TrackerHit objects are translated directly into
 * output SimTrackerHit collections.
 */
class RootPersistencyManager : public PersistencyManager {

public:
  /**
   * Class constructor.
   *
   * @param eventFile file to put output events into
   * @param parameters configuration parameters from Simulator
   * @param runNumber current run identifer from Process
   * @param ci Object that interfaces to the conditions system. 
   */
  RootPersistencyManager(framework::EventFile &file, Parameters &parameters,
                         const int &runNumber, ConditionsInterface &ci);

  /// Destructor
  virtual ~RootPersistencyManager() = default; 

  /**
   * Builds the output ROOT event.
   *
   * @param anEvent The Geant4 event data.
   */
  G4bool Store(const G4Event *anEvent);

  /**
   * This gets called automatically at the end of the run and is used to write
   * out the run header and close the writer.
   *
   * @param aRun The Geant4 run data (not used right now)
   *
   * @return True if event is stored (function is hard-coded to return true).
   */
  G4bool Store(const G4Run *aRun);


  /**
   * This is called "manually" in UserRunAction to open the ROOT writer for the
   * run.
   */
  void Initialize();

public:
  /**
   * Build an output event from the current Geant4 event.
   *
   * @param anEvent The Geant4 event.
   * @param outputEvent The output event.
   */
  void buildEvent(const G4Event *anEvent);

  /**
   * Write header info into the output event from Geant4.
   *
   * @param anEvent The Geant4 event.
   */
  void writeHeader(const G4Event *anEvent);

  /**
   * Write hits collections from Geant4 into a ROOT event.
   *
   * @param anEvent The Geant4 event.
   * @param outputEvent The output event.
   */
  void writeHitsCollections(const G4Event *anEvent, framework::Event *outputEvent);

  /**
   * Write a collection of tracker hits to an output collection.
   *
   * @param hc The collection of G4TrackerHits.
   * @param outputColl The output collection of SimTrackerHits.
   */
  void writeTrackerHitsCollection(G4TrackerHitsCollection *hc,
                                  std::vector<SimTrackerHit> &outputColl);

  /**
   * Write a collection of tracker hits to an output collection.
   *
   * @param hc The collection of G4CalorimeterHits.
   * @param outputColl The output collection of SimCalorimeterHits.
   */
  void
  writeCalorimeterHitsCollection(G4CalorimeterHitsCollection *hc,
                                 std::vector<SimCalorimeterHit> &outputColl);

  static std::unique_ptr<PersistencyManager>
  create(framework::EventFile &file, ldmx::Parameters &parameters,
         const int &run_number, ConditionsInterface &ci) {
    std::unique_ptr<PersistencyManager> manager(
        new RootPersistencyManager(file, parameters, run_number, ci));
    return manager;
  }

private:

  /// Handles ECal hit readout and IO.
  EcalHitIO ecalHitIO_;

  /// Helper for building output SimParticle collection.
  SimParticleBuilder simParticleBuilder_;
};

} // namespace persist
} // namespace simcore

#endif
