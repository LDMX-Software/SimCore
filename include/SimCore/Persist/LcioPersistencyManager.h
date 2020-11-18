#ifndef SIMCORE_PERSIST_LCIOPERSISTENCYMANAGER_H_
#define SIMCORE_PERSIST_LCIOPERSISTENCYMANAGER_H_

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/LCIO/LcioSimParticleBuilder.h"
#include "SimCore/Persist/PersistencyManager.h"

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Configure/Parameters.h"
#include "Framework/EventFile.h"

// Forward declarations
class G4Event; 
class G4Run;

namespace IMPL { 
class LCEventImpl; 
}

namespace simcore {
namespace persist {

/**
 * @class LcioPersistencyManager
 * @brief Manages persistence of Geant4 objects to an LCIO output file
 * @see http://lcio.desy.de/
 */
class LcioPersistencyManager : public PersistencyManager {

public:
  /**
   * Class constructor.
   *
   * @param eventFile file to put output events into
   * @param parameters configuration parameters from Simulator
   * @param run_number current run identifer from Process
   * @param ci Object that interfaces to the conditions system. 
   */
  LcioPersistencyManager(framework::EventFile &file, Parameters &parameters,
                         const int &run_number, ConditionsInterface &ci);

  /// Destructor
  virtual ~LcioPersistencyManager() = default;

  /**
   * Store a Geant4 event to an LCIO output event.
   *
   * @note Events marked as aborted are skipped and not stored.
   */
  G4bool Store(const G4Event *anEvent);

  /**
   * End of run hook which is used to close the current LCIO writer.
   *
   * @param[in] run The Geant4 run object.  
   */
  G4bool Store(const G4Run *run);

  // Used to initialize a persistency manager.  Not used by this manager. 
  void Initialize() {};

  /**
   *
   */
  static std::unique_ptr<PersistencyManager>
  create(framework::EventFile &file, ldmx::Parameters &parameters,
         const int &run_number, ConditionsInterface &ci) {
    std::unique_ptr<PersistencyManager> manager(
        new LcioPersistencyManager(file, parameters, run_number, ci));
    return manager;
  }

private:
  /// Write hits collections from the Geant4 event to an LCIO event.
  void writeHitsCollections(const G4Event *event,
                            IMPL::LCEventImpl *lcio_event);

  /**
   * Write a TrackerHitsCollection (LCDD) to LCIO.
   */
  IMPL::LCCollectionVec *writeTrackerHitsCollection(G4VHitsCollection *hc);

  /**
   * Write a CalorimeterHitsCollection (LCDD) to LCIO.
   */
  IMPL::LCCollectionVec *writeCalorimeterHitsCollection(G4VHitsCollection *hc);

  /// Builds MCParticle collection for the persistency manager.
  lcio::LcioSimParticleBuilder *builder_{new lcio::LcioSimParticleBuilder()};
};
} // namespace persist
} // namespace simcore

#endif // SIMCORE_PERSIST_LCIOPERSISTENCYMANAGER_H_
