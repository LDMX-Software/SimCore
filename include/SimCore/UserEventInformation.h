#ifndef SIMCORE_USEREVENTINFORMATION_H
#define SIMCORE_USEREVENTINFORMATION_H
#include <string>
#include <iostream>
using namespace std;
#include "G4VUserEventInformation.hh"

namespace simcore {

/**
 * Encapsulates user defined information associated with a Geant4 event.
 */
class UserEventInformation : public G4VUserEventInformation {
 public:
  /// Constructor
  UserEventInformation();

  /// Destructor
  ~UserEventInformation();

  /// Print the information associated with the track
  void Print() const final override;

  /// Increment the number of brem candidates in an event.
  void incBremCandidateCount() { bremCandidateCount_ += 1; }

  /// Decrease the number of brem candidates in an event.
  void decBremCandidateCount() { bremCandidateCount_ -= 1; }

  /// Increment the number of target recoil electrons in an event.
  void incRecoilElectronCount() { recoilElectronCount_ += 1; }

  /// Decrease the number of target recoil electrons in an event.
  void decRecoilElectronCount() { recoilElectronCount_ -= 1; }

  /**
   * Set the event weight.
   *
   * @param[in] weight the event weight
   */
  void setWeight(double weight) { weight_ = weight; }

  /**
   * @return The event weight
   */
  double getWeight() { return weight_; }

  /**
   * Increment the event weight by the input weight
   * for an individual step.
   *
   * @param[in] step_weight weight of an individual step
   */
  void incWeight(double step_weight) { weight_ *= step_weight; }

  /**
   * @return The total number of brem candidates that this event
   *      contains.
   */
  int bremCandidateCount() { return bremCandidateCount_; }

  /**
   * @return The total number of target recoil electrons that this event
   *      contains.
   */
  int recoilElectronCount() { return recoilElectronCount_; }  

  /**
   * Add energy to the photonuclear running total
   *
   * @param[in] delta energy to add to the total
   */
  void addPNEnergy(double delta) { total_photonuclear_energy_ += delta; }

  /**
   * Add energy to the electronuclear running total
   *
   * @param[in] delta energy to add to the total
   */
  void addENEnergy(double delta) { total_electronuclear_energy_ += delta; }

  /**
   * Get the total energy that went PN
   * @returns total simulated energy that went PN in this event
   */
  double getPNEnergy() const { return total_photonuclear_energy_; }

  /**
   * Get the total energy that went EN
   * @returns total simulated energy that went EN in this event
   */
  double getENEnergy() const { return total_electronuclear_energy_; }

  /**
   * Tell us if last step was PN
   * @param[in] yes true if it was
   */
  void lastStepWasPN(bool yes) { last_step_pn_ = yes; }

  /**
   * Was the last step a PN interaction?
   * @returns true if it was
   */
  bool wasLastStepPN() const { return last_step_pn_; }

  /**
   * Tell us if last step was EN
   * @param[in] yes true if it was
   */
  void lastStepWasEN(bool yes) { last_step_en_ = yes; }

  /**
   * Was the last step a EN interaction?
   * @returns true if it was
   */
  bool wasLastStepEN() const { return last_step_en_; }

  /**
   * Tell us if the event is fiducial
   * @param[in] yes true if it was
   */
  void setFiducial(bool yes) { is_fiducial_ = yes; }

  /**
   * Was the event is fiducial?
   * @returns true if it was
   */
  bool isFiducial() const { return is_fiducial_; }

  /**
   * Include the name of the ECal Volume that marked this event as fiducial
   * @param[in] name name of the volume to include
   */
  void setFiducialVolume(std::string name) { fiducial_volume_ = name; }

  /**
   * @returns the name of the ECal Volume
   */
  std::string getFiducialVolume() const {return fiducial_volume_; }

 private:

  /// The tag of whether an event is fiducial or non-fiducial
  bool is_fiducial_{false};

  /// The name of the ecal volume that made the event fiducial
  std::string fiducial_volume_{"none"};

  /// Total number of brem candidates in the event
  int bremCandidateCount_{0};

  /// Total number of target recoil electrons in the event
  int recoilElectronCount_{0};

  /**
   * The event weight
   *
   * @note The action WeightByStep relies on the assumption
   * that this weight *starts at 1*, so the value of this
   * member variable should always be 1.
   */
  double weight_{1.};

  /**
   * The total energy that went into the photon-nuclear interaction
   *
   * @note The action TrackNuclearEnergy relies on the assumption
   * that this running total starts at 0, so the value of this
   * member variable should always be 0 at construction.
   */
  double total_photonuclear_energy_{0.};

  /**
   * Was the most recent step a photon-nuclear interaction?
   */
  bool last_step_pn_{false};

  /**
   * The total energy that went into the electron-nuclear interaction
   *
   * @note The action TrackNuclearEnergy relies on the assumption
   * that this running total starts at 0, so the value of this
   * member variable should always be 0 at construction.
   */
  double total_electronuclear_energy_{0.};

  /**
   * Was the most recent step a electron-nuclear interaction?
   */
  bool last_step_en_{false};
};
}  // namespace simcore

#endif  // SIMCORE_USEREVENTINFORMATION_H