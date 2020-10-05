/**
 * @file DetectorConstruction.h
 * @brief Class implementing the Geant4 detector construction
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef SIMCORE_DETECTORCONSTRUCTION_H_
#define SIMCORE_DETECTORCONSTRUCTION_H_

// LDMX
//#include "AuxInfoReader.h"

// Biasing
#include "SimCore/DarkBremXsecBiasingOperator.h"
#include "SimCore/ElectroNuclearXsecBiasingOperator.h"
#include "SimCore/GammaToMuPairXsecBiasingOperator.h"
#include "SimCore/PhotoNuclearXsecBiasingOperator.h"

// Geant4
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4VUserDetectorConstruction.hh"

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Configure/Parameters.h"

/*~~~~~~~~~~~~~~*/
/*   DetDescr   */
/*~~~~~~~~~~~~~~*/
//#include "DetDescr/DetectorHeader.h" 

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/Geo/Parser.h" 

// Forward declaration
namespace simcore {
namespace geo { 
class Parser; 
}
}

namespace ldmx {

/**
 * @class DetectorConstruction
 * @brief Implements the Geant4 detector construction
 *
 * @note
 * This class reads in a detector description from a GDML file
 * using the basic <i>G4GDMLParser</i> and instantiates supplemental
 * information using the AuxInfoReader.
 *
 * @see AuxInfoReader
 */
class DetectorConstruction : public G4VUserDetectorConstruction {

public:
  /**
   * Class constructor.
   * @param parser The parser used to parse the geometry. 
   */
  DetectorConstruction(simcore::geo::Parser *parser, Parameters &parameters);

  /**
   * Class destructor.
   */
  virtual ~DetectorConstruction();

  /**
   * Construct the detector.
   * @return The top volume of the detector.
   */
  G4VPhysicalVolume *Construct();

  /**
   */
  void ConstructSDandField();

  /**
   * Get the name of this detector. 
   *
   * This is typicall set in the detector description file used to build this
   * detector.
   *
   * @return The detector name.
   */
  std::string getDetectorName() { return parser_->getDetectorName(); }

private:
  /**
   * The GDML parser defining the detector.
   */
  simcore::geo::Parser *parser_;

  /**
   * The auxiliary GDML info reader.
   */
  //AuxInfoReader *auxInfoReader_;

  /// The set of parameters used to configure this class
  Parameters parameters_;
};

} // namespace ldmx

#endif
