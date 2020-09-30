#ifndef SIMCORE_GEO_GEOPARSER_H_
#define SIMCORE_GEO_GEOPARSER_H_

/*~~~~~~~~~~~~*/
/*   Geant4   */
/*~~~~~~~~~~~~*/
#include "G4VPhysicalVolume.hh"

namespace simcore {

namespace geo {

/**
 * Interface for a parser used to load a geometry into Geant4.
 *
 * This will be used by a parser factory class to spawn an instance of the
 * desired parser.
 */
class Parser {

 public:
  /**
   * Pure virtual method used to retrieve the G4VPhysicalVolume associated with
   * the most top-level (world) volume.
   *
   * @return The world volume.
   */
  virtual G4VPhysicalVolume *GetWorldVolume() = 0;
}; // Parser

} // namespace geo
} // namespace simcore

#endif // SIMCORE_GEO_GEOPARSER_H_
