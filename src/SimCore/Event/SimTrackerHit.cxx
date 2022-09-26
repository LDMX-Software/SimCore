#include "SimCore/Event/SimTrackerHit.h"

ClassImp(ldmx::SimTrackerHit)

namespace ldmx {
SimTrackerHit::SimTrackerHit() {}

SimTrackerHit::~SimTrackerHit() { clear(); }

void SimTrackerHit::Print() const {
  std::cout << "SimTrackerHit { "
            << "id: " << id_ << ", "
            << "layerID: " << layerID_ << ", "
            << "moduleID: " << moduleID_ << ", "
            << "position: ( " << x_ << ", " << y_ << ", " << z_ << " ), "
            << "edep: " << edep_ << ", "
            << "time: " << time_ << ", "
            << "momentum: ( " << px_ << ", " << py_ << ", " << pz_ << " )"
            << " }" << std::endl;
}

void SimTrackerHit::clear() {
  id_ = 0;
  layerID_ = 0;
  moduleID_ = 0;
  edep_ = 0;
  time_ = 0;
  px_ = 0;
  py_ = 0;
  pz_ = 0;
  x_ = 0;
  y_ = 0;
  z_ = 0;
  energy_ = 0;
  pathLength_ = 0;
  trackID_ = -1;
  pdgID_ = 0;
}

void SimTrackerHit::setPosition(const float x, const float y, const float z) {
  this->x_ = x;
  this->y_ = y;
  this->z_ = z;
}

void SimTrackerHit::setMomentum(const float px, const float py,
                                const float pz) {
  this->px_ = px;
  this->py_ = py;
  this->pz_ = pz;
}

void SimTrackerHit::attach(fire::io::Data<SimTrackerHit>& d) {
  d.attach("id", id_);
  d.attach("layerID", layerID_);
  d.attach("moduleID", moduleID_);
  d.attach("edep", edep_);
  d.attach("time", time_);
  d.attach("px", px_);
  d.attach("py", py_);
  d.attach("pz", pz_);
  d.attach("energy", energy_);
  d.attach("x", x_);
  d.attach("y", y_);
  d.attach("z", z_);
  d.attach("pathLength", pathLength_);
  d.attach("trackID", trackID_);
  d.attach("pdgID", pdgID_);
}
} // namespace ldmx
