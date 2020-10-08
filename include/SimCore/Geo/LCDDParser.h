#ifndef SIMCORE_GEO_LCDDPARSER_H_
#define SIMCORE_GEO_LCDDPARSER_H_

/*~~~~~~~~~~~~~~~~*/
/*   C++ StdLib   */
/*~~~~~~~~~~~~~~~~*/
#include <memory>

/*~~~~~~~~~*/
/*  GDML   */
/*~~~~~~~~~*/
#include "Saxana/ProcessingConfigurator.h"
#include "Saxana/SAXProcessor.h"

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Configure/Parameters.h"

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/Geo/Parser.h"

// Forward Declarations
class G4VPhysicalVolume;

namespace simcore {
namespace geo {

/**
 * The parser for reading LCDD files into Geant4.
 * 
 * This class extends the interface Parser which allows creation of the parser
 * at runtime via a factory. 
 */
class LCDDParser : public Parser {

public:
  /// Default constructor
  LCDDParser(ldmx::Parameters &parameters);

  /// Default destructor
  ~LCDDParser();

  /**
   * Retrieve the G4VPhysicalVolume associated with the most top-level
   * (world) volume.
   *
   * @return The world volume.
   */
  G4VPhysicalVolume *GetWorldVolume() final override { return world_; };

  /**
   * Get the name of the parsed detector.
   *
   * This name is typically extracted from the file containing the detector
   * description.
   *
   * @return The name of the detector.
   */
  virtual std::string getDetectorName() final override { return detector_name_; };

  /// Parse the detector geometry and read it into memory.
  virtual void read() final override;

  /**
   * Create an instance of this parser.
   * 
   * @param parameters The parameter set used to configure this parser.
   *
   * @return An instance of a LCDDParser object.
   */
  static Parser *create(ldmx::Parameters &parameters) {
    return new LCDDParser(parameters);
  }

private:
  /// The SAX parser
  std::unique_ptr<SAXProcessor> parser_;

  /// The processor configuration
  std::unique_ptr<ProcessingConfigurator> config_;

  /// The Geant4 world volume
  G4VPhysicalVolume *world_{nullptr};

  /// The name of the parsed detector
  std::string detector_name_{""};

}; // LCDDParser
} // namespace geo
} // namespace simcore

#endif // SIMCORE_GEO_LCDDPARSER_H_
