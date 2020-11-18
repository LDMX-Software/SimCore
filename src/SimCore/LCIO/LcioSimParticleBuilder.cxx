#include "SimCore/LCIO/LcioSimParticleBuilder.h"

#include "Framework/Event.h"
#include "IMPL/LCEventImpl.h" 


namespace simcore {
namespace lcio {

void LcioSimParticleBuilder::buildSimParticles(framework::Event *event) {
  // Get the trajectory container for the event.
  auto trajectories{
      (const_cast<G4Event *>(g4_event_))->GetTrajectoryContainer()};

  // Create the collection used to hold the MCParticles
  auto particle_coll{new IMPL::LCCollectionVec(EVENT::LCIO::MCPARTICLE)};

  for (auto trajectory : *trajectories->GetVector()) {
    auto particle{new IMPL::MCParticleImpl()};
    particle_coll->addElement(particle); 
    particle_map_[trajectory->GetTrackID()] = particle; 
  }

  // Fill information into the particles
  for (auto traj : *trajectories->GetVector()) {
    // Get the MCParticle from the map
    IMPL::MCParticleImpl *particle = particle_map_[traj->GetTrackID()];

    auto trajectory{static_cast<ldmx::Trajectory *>(traj)};

    particle->setGeneratorStatus(trajectory->getGenStatus());
    particle->setPDG(trajectory->GetPDGEncoding());
    particle->setCharge(trajectory->GetCharge());
    particle->setMass(trajectory->getMass()/1000);
    particle->setTime(trajectory->getGlobalTime());


    const double vtx[3] = {trajectory->getVertexPosition()[0],
                           trajectory->getVertexPosition()[1],
                           trajectory->getVertexPosition()[2]}; 
    particle->setVertex(vtx);  

    const double p[3] = {trajectory->GetInitialMomentum()[0],
                        trajectory->GetInitialMomentum()[1],
                        trajectory->GetInitialMomentum()[2]}; 
    particle->setMomentum(p); 
        
    const double ep[3] = {trajectory->getEndPointMomentum()[0],
      trajectory->getEndPointMomentum()[1],
      trajectory->getEndPointMomentum()[2]}; 
    particle->setMomentumAtEndpoint(ep); 

    double endp[] = {trajectory->getEndPoint()[0], trajectory->getEndPoint()[1],
                     trajectory->getEndPoint()[2]};
    particle->setEndpoint(endp);

    // Add the parents to this particle
    if (trajectory->GetParentID() > 0) {
      auto parent{particle_map_.find(trajectory->GetParentID())};
      if( parent != particle_map_.end() ) {
        particle->addParent(particle_map_[trajectory->GetParentID()]);
      } 
    }

    // Set sim status to indicate particle was created in simulation.
    /*if (!trajectory->getGenStatus()) {
      std::bitset<32> simStatus;
      simStatus[EVENT::MCParticle::BITCreatedInSimulation] = 1;
      particle->setSimulatorStatus(simStatus.to_ulong());
    }*/
  }
  
  // Retrieve the current LCIO event
  auto lcio_event{std::any_cast<IMPL::LCEventImpl*>(event->get())};
  
  // Add the collection
  lcio_event->addCollection(particle_coll, EVENT::LCIO::MCPARTICLE); 

}

} // namespace lcio
} // namespace simcore
