
#include "SimCore/Geo/LCDDParser.h"

/*~~~~~~~~~~*/
/*   LCDD   */
/*~~~~~~~~~~*/
#include "lcdd/bfield/FieldManager.hh"
#include "lcdd/core/LCDDHeaderRecord.hh"
#include "lcdd/subscribers/volumeExtendedSubscriber.hh"

/*~~~~~~~~~*/
/*  GDML   */
/*~~~~~~~~~*/
#include "G4Processor/GDMLProcessor.h"
#include "Saxana/RCObjectHandle.h"
#include "Saxana/SAXSubscriber.h"
#include "Saxana/SAXSubscriberPool.h"

extern "C" void LCDDLibLoad();

namespace simcore {
namespace geo {

LCDDParser::LCDDParser(ldmx::Parameters &parameters) {
  // Initialize the SAX parser.
  parser_ = std::make_unique<SAXProcessor>();
  parser_->Initialize();

  // Load the LCDD tag handlers.
  LCDDLibLoad();

  // Add the subscriber for the extended GDML volume tag.
  RCObjectHandle<SAXSubscriber> obj{new volumeExtendedSubscriber()};
  auto pool{const_cast<SAXSubscriberPool *>(parser_->GetSubscriberPool())};
  pool->AddSubscriber("volume", obj);

  // Create the configuration
  config_ = std::make_unique<ProcessingConfigurator>();
  config_->SetURI(parameters.getParameter<std::string>("detector"));

  // Configure the parser
  parser_->Configure(config_.get());
}

LCDDParser::~LCDDParser() { parser_->Finalize(); }

void LCDDParser::read() {
  // Run he parser
  parser_->Run();

  // Get the header to extract the detector name.
  auto header{LCDDProcessor::instance()->getHeader()};
  detector_name_ = header->getDetectorName();

  // Get the world volume created by the parser.
  try {
    world_ = const_cast<G4VPhysicalVolume *>(
        GDMLProcessor::GetInstance()->GetWorldVolume());
  } catch (std::exception &e) {
  }

  // Setup the magnetic field
  FieldManager::instance()->setup();
}

} // namespace geo
} // namespace simcore
