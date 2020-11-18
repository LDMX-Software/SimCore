#ifndef SIMCORE_PERSIST_PERSISTENCYMANAGER_H_
#define SIMCORE_PERSIST_PERSISTENCYMANAGER_H_

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <functional>

/*~~~~~~~~~~~~~~*/
/*    Geant4    */
/*~~~~~~~~~~~~~~*/
#include "G4PersistencyCenter.hh"
#include "G4PersistencyManager.hh"

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/ConditionsInterface.h" 

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Configure/Parameters.h"
#include "Framework/Event.h"
#include "Framework/EventFile.h"

namespace simcore {
namespace persist {

class PersistencyManager : public G4PersistencyManager {

public:
  /**
   * Constructor.
   *
   * @param event_file EventFile object controller IO for current process.
   * @param parameters Parameters used to configure this persistency manager.
   * @param run_number Current run number.
   *
   */
  PersistencyManager(framework::EventFile &file,
                     ldmx::Parameters &parameters, const int &run_number, 
                     ldmx::ConditionsInterface &ci); 

  /// Destructor.
  virtual ~PersistencyManager() = default;

  /// Implementing this makes an "overloaded-virtual" compiler warning go away.
  G4bool Store(const G4VPhysicalVolume *) override { return false; }

  /**
   * Set the current sim event.
   *
   * The current sim event is used by the persistency manager to persist
   * information about the current Geant4 event.
   *
   * @param[in] event Event buffer for the current event.
   */
  void setCurrentEvent(framework::Event *event) { event_ = event; }

  /**
   * Set the number of events began and completed.
   *
   * These two numbers may or may not be equal
   * depending on if the simulation ran with any filters
   * that would abort events early.
   *
   * These numbers are helpful for evaluating filtering
   * performance, so we put them both in the RunHeader.
   *
   * @param[in] began number of events that were started
   * @param[in] completed number of events that were completed without an abort
   * signal
   */
  virtual void setNumEvents(int began, int completed) {
    events_began_ = began;
    events_completed_ = completed;
  }

  /**
   * Return the current persistency manager instance.
   */
  static PersistencyManager *getInstance() {
    return static_cast<PersistencyManager *>(
        G4PersistencyCenter::GetPersistencyCenter()
            ->CurrentPersistencyManager());
  }

protected:
  /// The event container used to manage the collections.
  framework::Event *event_{nullptr};
  
  /// The output file.
  framework::EventFile &file_;
  
  /// Configuration parameters passed to Simulator
  ldmx::Parameters parameters_;

  /// Number of events started on this production run
  int events_began_{-1};

  /// Number of events completed without being aborted (due to filters)
  int events_completed_{-1};

  /// Run Number, given to us by Simulator from Process
  int run_;

}; // PeristencyManager

using createFunc = std::function<std::unique_ptr<PersistencyManager>(
    framework::EventFile &file, ldmx::Parameters &parameters,
    const int &run_number, ConditionsInterface &ci)>;

} // namespace persist
} // namespace simcore

#endif // SIMCORE_PERSIST_PERSISTENCYMANAGER_H_
