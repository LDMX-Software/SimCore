#ifndef SIMCORE_TRIGSD_H
#define SIMCORE_TRIGSD_H

#include "SimCore/SensitiveDetector.h"
#include "SimCore/Event/SimCalorimeterHit.h"

namespace simcore {

/**
 * Class defining a sensitive detector of type trigger scintillator.
 */
class TrigScintSD : public SensitiveDetector {
 public:
  /// name of output hits collection
  static const std::string COLLECTION_NAME;

  /**
   * Class constructor.
   *
   * @param[in] name The name of the sensitive detector.
   * @param[in] ci interface to conditions objects
   * @param[in] p python configuration parameters
   */
  TrigScintSD(const std::string& name,
              simcore::ConditionsInterface& ci,
              const framework::config::Parameters& p);

  /// Destructor
  ~TrigScintSD();

  /**
   * Should the input logical volume be included
   * in this sensitive detector?
   *
   * @note Depends on names in GDML!
   */
  virtual bool isSensDet(G4LogicalVolume* vol) const final override {
    return vol->GetName().contains("trigger_pad");
  }

  /**
   * Process steps to create hits.
   *
   * @param[in] step The step information.
   * @param[in] history The readout history.
   */
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);

  /**
   * Save our hits collection into the event bus and reset it.
   */
  virtual void saveHits(framework::Event& event) final override {
    event.add(COLLECTION_NAME, hits_);
    hits_.clear();
  }

 private:
  /**
   * Get the module ID for the input logical volume
   *
   * Assumes we are already in a volume where
   * isSensDet is true.
   *
   * tag <-> 1
   * up  <-> 2
   * dn  <-> 3
   *
   * @note Depends on names in GDML!
   */
  int getModuleID(G4LogicalVolume* vol) const;

 private:
  /// our collection of hits in this SD
  std::vector<ldmx::SimCalorimeterHit> hits_;

};

}  // namespace simcore

#endif