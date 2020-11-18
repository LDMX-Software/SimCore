#ifndef SIMCORE_LCIO_LCIOSIMPARTICLEBUILDER_H_
#define SIMCORE_LCIO_LCIOSIMPARTICLEBUILDER_H_

//#include "UserTrackingAction.h"

#include "EVENT/LCIO.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/MCParticleImpl.h"

#include "G4SystemOfUnits.hh"

#include "G4Event.hh"
#include "SimCore/TrackMap.h"
#include "SimCore/Trajectory.h"
#include <map>

namespace framework {
class Event;
}

namespace simcore {
namespace lcio {

class LcioSimParticleBuilder {

public:
  /// Constructor
  LcioSimParticleBuilder() = default;

  /// Destructor
  ~LcioSimParticleBuilder() = default;

  /**
   * Build the collection of MC particles and store to the output event.
   *
   * @param[in] event The output event.
   */
  void buildSimParticles(framework::Event *event);

  /**
   * Set the current Geant4 event.
   * @param anEvent The Geant4 event.
   */
  void setCurrentEvent(const G4Event *anEvent) {
    g4_event_ = const_cast<G4Event *>(anEvent);
  }

  /// @return A mapping between the track ID and an MCParticle instance
  std::map<int, IMPL::MCParticleImpl *> getParticleMap() { return particle_map_; }

private:
  /// Current Geant4 event.
  G4Event *g4_event_;

  /// Mapping between track ID and the corresponding MCParticle instance
  std::map<int, IMPL::MCParticleImpl *> particle_map_;
};

} // namespace lcio
} // namespace simcore

#endif
