/**
 * @file APrimePhysics.h
 * @brief Class which defines basic APrime physics
 * @author Michael Revering, University of Minnesota
 * @author Tom Eichlersmith, University of Minnesota
 */

#ifndef SIMCORE_APRIMEPHYSICS_H_
#define SIMCORE_APRIMEPHYSICS_H_

// Geant4
#include "G4VPhysicsConstructor.hh"

// LDMX
#include "Framework/Parameters.h"

namespace ldmx {

    /**
     * @class APrimePhysics
     * @brief Defines basic APrime physics
     *
     * It constructs the APrime particle and links the dark brem process
     * to the electron.
     *
     * @note
     * This class basically does not do anything except define
     * a dummy particle so that event generation works properly.
     */
    class APrimePhysics : public G4VPhysicsConstructor {

        public:

            /**
             * Class constructor.
             *
             * @param name The name of the physics.
             */
            APrimePhysics(Parameters params, const G4String& name = "APrime");

            /**
             * Class destructor.
             *
             * Nothing right now.
             */
            virtual ~APrimePhysics();

            /**
             * Construct particles.
             *
             * Instatiates the APrime particle by calling the accessor method.
             */
            void ConstructParticle();

            /**
             * Construct the process.
             *
             * Links the dark brem processs to the electron through the process manager.
             */
            void ConstructProcess();

        private:

            /// dark brem process parameters
            Parameters parameters_;

            /// Definition of the APrime particle.
            G4ParticleDefinition* aprimeDef_;
    };

}

#endif
