#include "SimCore/Geo/GDMLParser.h"

namespace simcore {
namespace geo {

GDMLParser::GDMLParser(ldmx::Parameters &parameters) {
  parser_ = std::make_unique<G4GDMLParser>();
  info_ = std::make_unique<ldmx::AuxInfoReader>(parser_.get(), parameters);
  parameters_ = parameters;
}

G4VPhysicalVolume *GDMLParser::GetWorldVolume() {
  return parser_->GetWorldVolume();
}

void GDMLParser::read() {
  parser_->Read(parameters_.getParameter<std::string>("detector"),
                parameters_.getParameter<bool>("validate_detector"));
  info_->readGlobalAuxInfo();
  info_->assignAuxInfoToVolumes();
  detector_name_ = info_->getDetectorHeader()->getName(); 
}

} // namespace geo
} // namespace simcore
