
#ifndef SIMCORE_DETECTORCONSTRUCTION_H_
#define SIMCORE_DETECTORCONSTRUCTION_H_

/*~~~~~~~~~~~~*/
/*   Geant4   */
/*~~~~~~~~~~~~*/
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VUserDetectorConstruction.hh"

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Configure/Parameters.h"

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/DarkBremXsecBiasingOperator.h"
#include "SimCore/ElectroNuclearXsecBiasingOperator.h"
#include "SimCore/GammaToMuPairXsecBiasingOperator.h"
#include "SimCore/PhotoNuclearXsecBiasingOperator.h"
#include "SimCore/Geo/Parser.h"
#include "SimCore/ConditionsInterface.h" 

// Forward declaration
namespace simcore {
namespace geo {
class Parser;
} // namespace geo
} // namespace simcore

namespace ldmx {

/**
 * @class DetectorConstruction
 * @brief Implements the Geant4 detector construction
 */
class DetectorConstruction : public G4VUserDetectorConstruction {

public:
  /**
   * Class constructor.
   *
   * @param parser The parser used to parse the geometry.
   * @param parameters The parameters used to configure this class.
   */
  DetectorConstruction(simcore::geo::Parser *parser, Parameters &parameters, ConditionsInterface& ci);

  /// Class destructor
  virtual ~DetectorConstruction();

  /**
   * Construct the detector.
   *
   * @return The top-level volume of the detector (world volume). 
   */
  G4VPhysicalVolume *Construct();

  /// Instantiate the sensitive detectors and magnetic field.
  void ConstructSDandField();

  /**
   * Get the name of this detector.
   *
   * This is typically set in the detector description file used to build this
   * detector.
   *
   * @return The detector name.
   */
  std::string getDetectorName() { return detector_name_; }

private:
  /// The detector description parser
  simcore::geo::Parser *parser_;

  /// The set of parameters used to configure this class
  Parameters parameters_;

  /// The name of the detector being constructed
  std::string detector_name_{""};
};

} // namespace ldmx

#endif // SIMCORE_DETECTORCONSTRUCTION_H_
