
#include "SimCore/Persist/LcioPersistencyManager.h"

/*~~~~~~~~~~~~*/
/*   Geant4   */
/*~~~~~~~~~~~~*/
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

/*~~~~~~~~~~*/
/*   LCDD   */
/*~~~~~~~~~~*/
#include "lcdd/core/LCDDProcessor.hh"
#include "lcdd/hits/CalorimeterHit.hh"
#include "lcdd/hits/TrackerHit.hh"

/*~~~~~~~~~~*/
/*   LCIO   */
/*~~~~~~~~~~*/
#include "EVENT/LCEvent.h"
#include "EVENT/LCIO.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/SimTrackerHitImpl.h"

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Exception/Exception.h"

namespace simcore {
namespace persist {

LcioPersistencyManager::LcioPersistencyManager(framework::EventFile &file,
                                               Parameters &parameters,
                                               const int &run_number,
                                               ConditionsInterface &ci)
    : PersistencyManager(file, parameters, run_number, ci) {}

G4bool LcioPersistencyManager::Store(const G4Event *event) {

  // Retrieve the current LCIO event
  auto lcio_event{std::any_cast<IMPL::LCEventImpl *>(event_->get())};

  // Set some event information.  Most of the information is retrieved via the
  // event header associated with the event object passed to the persistency
  // manager.
  auto event_header{event_->getEventHeaderPtr()};
  lcio_event->setEventNumber(event_header->getEventNumber());
  lcio_event->setRunNumber(run_);
  lcio_event->setWeight(event_header->getWeight());
  lcio_event->setDetectorName(LCDDProcessor::instance()->getDetectorName());

  builder_->setCurrentEvent(event);
  builder_->buildSimParticles(event_);

  // Write all Geant4 hits to the LCIO event
  writeHitsCollections(event, lcio_event);

  return true;
}

G4bool LcioPersistencyManager::Store(const G4Run *run) {}

void LcioPersistencyManager::Initialize() {}

void LcioPersistencyManager::writeHitsCollections(
    const G4Event *g4_event, IMPL::LCEventImpl *lcio_event) {

  // Get the hit collections
  auto hit_coll{g4_event->GetHCofThisEvent()};
  auto coll_count{hit_coll->GetNumberOfCollections()};

  // Loop over all hit collections and add them to the LCIO event.
  for (int icoll{0}; icoll < coll_count; ++icoll) {

    // Get one of the hit collections and it's name
    auto hits{hit_coll->GetHC(icoll)};
    auto col_name{hits->GetName()};

    // Place holder for the collection that will be added to the event.
    IMPL::LCCollectionVec *lcio_coll{nullptr};

    // Process the hits depending on type.  Throw an exception if an
    // incompatible hit collection isn't found.
    if (dynamic_cast<TrackerHitsCollection *>(hits)) {
      lcio_coll = writeTrackerHitsCollection(hits);
    } else if (dynamic_cast<CalorimeterHitsCollection *>(hits)) {
      lcio_coll = writeCalorimeterHitsCollection(hits);
    } /*else {
      EXCEPTION_RAISE("InvalidHitType",
                      "Hit colletion " + col_name + " has unknown type.");
    }*/

    if (lcio_coll) {
      lcio_event->addCollection(lcio_coll, col_name);
    }
  }
}

IMPL::LCCollectionVec *
LcioPersistencyManager::writeTrackerHitsCollection(G4VHitsCollection *hits) {

  // Cast the hits to tracker hits
  auto tracker_hits{dynamic_cast<TrackerHitsCollection *>(hits)};

  // Instantiate the container used to hold the LCIO hits.
  auto lcio_coll{new IMPL::LCCollectionVec(EVENT::LCIO::SIMTRACKERHIT)};

  // Enable persistence of the momentum.
  IMPL::LCFlagImpl flag;
  flag.setBit(EVENT::LCIO::THBIT_MOMENTUM);
  lcio_coll->setFlag(flag.getFlag());

  // Add the hits to the LCIO collection
  for (int ihit{0}; ihit < tracker_hits->GetSize(); ++ihit) {

    auto hit{static_cast<TrackerHit *>(tracker_hits->GetHit(ihit))};
    // Instantiate the LCIO class that encapsualte tracker hit information.
    auto sim_tracker_hit{new IMPL::SimTrackerHitImpl()};

    // position in mm
    auto pos_vec{hit->getPosition()};
    double pos[3] = {pos_vec.x(), pos_vec.y(), pos_vec.z()};
    sim_tracker_hit->setPosition(pos);

    // momentum in GeV
    auto momentum{hit->getMomentum()};
    sim_tracker_hit->setMomentum(momentum.x() / GeV, momentum.y() / GeV,
                                 momentum.z() / GeV);

    // pathLength = distance between exit and entry points in mm
    sim_tracker_hit->setPathLength(hit->getLength());

    // dEdx in GeV (LCIO units)
    float edep = hit->getEdep();
    sim_tracker_hit->setEDep(edep / GeV);

    // time in ns
    float tEdep = hit->getTdep();
    sim_tracker_hit->setTime(tEdep);

    // Cell ID.
    sim_tracker_hit->setCellID0(hit->getId());
    lcio_coll->push_back(sim_tracker_hit);

    // Get the sim particle map and search for the MC particle associated with
    // the track ID. If found, add the MC particle to the tracker hit.
    auto particle_map{builder_->getParticleMap()};
    auto mcp{particle_map.find(hit->getTrackID())};
    if (mcp != particle_map.end()) {
      sim_tracker_hit->setMCParticle(mcp->second);
    } else {
      //EXCEPTION_RAISE("NoMCParticleException",
      //                "MCParticle for track ID " +
      //                    std::to_string(hit->getTrackID()) + " is missing.");
    }
  }
  return lcio_coll;
}

IMPL::LCCollectionVec *LcioPersistencyManager::writeCalorimeterHitsCollection(
    G4VHitsCollection *hits) {

  // Cast the hits to tracker hits
  auto calo_hits{dynamic_cast<CalorimeterHitsCollection *>(hits)};

  // Instantiate the container to hold LCIO sim calorimeter hits
  auto lcio_coll = new IMPL::LCCollectionVec(EVENT::LCIO::SIMCALORIMETERHIT);

  // Enable saving of PDG ID
  IMPL::LCFlagImpl flag;
  flag.setBit(EVENT::LCIO::CHBIT_LONG);
  flag.setBit(EVENT::LCIO::CHBIT_PDG);
  lcio_coll->setFlag(flag.getFlag());

  int nhits = calo_hits->GetSize();
  for (int ihit{0}; ihit < calo_hits->GetSize(); ++ihit) {

    auto hit{static_cast<CalorimeterHit *>(calo_hits->GetHit(ihit))};
    auto sim_calo_hit{new IMPL::SimCalorimeterHitImpl()};

    // set cellid from cal hit's id64
    const Id64bit &id64 = hit->getId64bit();
    sim_calo_hit->setCellID0(id64.getId0());
    sim_calo_hit->setCellID1(id64.getId1());

    // position
    const G4ThreeVector hitPos = hit->getPosition();
    float pos[3] = {(float)hitPos.x(), (float)hitPos.y(), (float)hitPos.z()};
    sim_calo_hit->setPosition(pos);

    // add to output collection
    lcio_coll->push_back(sim_calo_hit);

    auto contribs = hit->getHitContributions();
    for (auto contrib : contribs) {
      auto edep = contrib.getEdep() / GeV;
      auto hitTime = contrib.getGlobalTime();
      auto pdg = contrib.getPDGID();
      auto contribPos = contrib.getPosition();
      auto trackID = contrib.getTrackID();

      /*
      if (trackID <= 0) {
        //EXCEPTION_RAISE("BadTrackID", "Bad track ID found.");
      }

      // Get the sim particle map and search for the MC particle associated with
      // the track ID. If found, add the MC particle to the tracker hit.
      auto particle_map{builder_->getParticleMap()};
      auto mcp{particle_map.find(trackID)};
      if (mcp != particle_map.end()) {
        sim_calo_hit->addMCParticleContribution(
            static_cast<EVENT::MCParticle *>(mcp->second), (float)edep,
            (float)hitTime); //  (int) pdg, (float*) contribPos);
      } else {
        EXCEPTION_RAISE("NoMCParticleException", "MCParticle for track ID " +
                                                     std::to_string(trackID) +
                                                     " is missing.");
      }*/
    }
  }
  return lcio_coll;
}
} // namespace persist
} // namespace simcore
