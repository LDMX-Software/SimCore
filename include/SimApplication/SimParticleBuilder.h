#ifndef SIMAPPLICATION_SIMPARTICLEBUILDER_H_
#define SIMAPPLICATION_SIMPARTICLEBUILDER_H_ 1

// LDMX
#include "SimApplication/UserTrackInformation.h"
#include "Event/SimParticle.h"

// STL
#include <map>

class SimParticleBuilder {

    public:

        typedef std::map<G4int, SimParticle*> SimParticleMap;

        SimParticleBuilder();

        virtual ~SimParticleBuilder();

        void buildSimParticles();

        SimParticle* findSimParticle(G4int);

        void assignTrackerHitSimParticles();

    private:

        void buildSimParticle(SimParticle* p, TrackSummary* info);

    private:

        SimParticleMap particleMap;
};

#endif
