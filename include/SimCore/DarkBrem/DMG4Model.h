#ifndef SIMCORE_DARKBREM_DMG4MODEL_H_
#define SIMCORE_DARKBREM_DMG4MODEL_H_

#include "Framework/Configure/Parameters.h"
#include "SimCore/DarkBrem/G4eDarkBremsstrahlung.h"

// DMG4 DarkPhoton Model
#include "DarkMatter.hh"
#include "DarkPhotons.hh"

namespace simcore {
namespace darkbrem {

/**
 * @class DMG4Model
 *
 * A model where we use NA64's DMG4 module to compare our
 * dark brem simulation to theirs.
 */
class DMG4Model : public G4eDarkBremsstrahlungModel {
 public:
  /**
   * Constructor
   * Set the parameters for this model.
   * We pass parameters to the underlying DarkPhoton class
   * from the DMG4 library.
   */
  DMG4Model(framework::config::Parameters& params);

  /**
   * Destructor
   */
  virtual ~DMG4Model() {}

  /**
   * Print the configuration of this model
   */
  virtual void PrintInfo() const;

  /**
   * Record the configuration of this model into the RunHeader
   */
  virtual void RecordConfig(ldmx::RunHeader& h) const;

  /**
   * Calculates the cross section per atom in GEANT4 internal units.
   *
   * @param E0 energy of beam (incoming particle)
   * @param A atomic mass of atom (unused)
   * @param Z atomic number of atom (unused)
   * @return cross section (0. if outside energy cuts)
   */
  virtual G4double ComputeCrossSectionPerAtom(G4double electronKE,
                                              G4double atomicA,
                                              G4double atomicZ);

  /**
   * Simulates the emission of a dark photon + electron.
   *
   * Copied over from the DMG4 source code.
   *  DMG4/src/DMG4/DMProcessDMBrem.cc
   *
   * @param[in,out] particleChange structure holding changes to make to particle
   * track
   * @param[in] track current track being processesed
   * @param[in] step current step of the track
   */
  virtual void GenerateChange(G4ParticleChange& particleChange,
                              const G4Track& track, const G4Step& step);

 private:
  /// model imported from DMG4 library
  std::unique_ptr<DarkMatter> dm_model_;
};

}  // namespace darkbrem
}  // namespace simcore

#endif  // SIMCORE_DARKBREM_DARKBREMVERTEXLIBRARYMODLE_H_
