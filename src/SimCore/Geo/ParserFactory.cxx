
#include "SimCore/Geo/ParserFactory.h"

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/Geo/GDMLParser.h"
#include "SimCore/Geo/LCDDParser.h"

namespace simcore {
namespace geo {

ParserFactory *ParserFactory::instance_ = nullptr;

ParserFactory *ParserFactory::getInstance() {
  if (!instance_)
    instance_ = new ParserFactory;
  return instance_;
}

ParserFactory::ParserFactory() {
  registerParser("gdml", &GDMLParser::create);
  registerParser("lcdd", &LCDDParser::create);
}

void ParserFactory::registerParser(const std::string &name, createFunc create) {
  parser_map[name] = create;
}

Parser *ParserFactory::createParser(const std::string &name,
                                    ldmx::Parameters &parameters) {
  auto it{parser_map.find(name)};
  if (it != parser_map.end())
    return it->second(parameters);
  return nullptr;
}

} // namespace geo
} // namespace simcore
