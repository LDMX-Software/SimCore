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
     * It constructs the APrime particle and links the dark brem process to the electron.
     *
     * @see G4APrime
     * @see G4eDarkBremsstrahlung
     *
     * @note
     * This class basically does not do anything except
     * register the custom particle (G4APrime) and custom
     * process (G4eDarkBremsstrahlung).
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
            virtual ~APrimePhysics() { /* nothing on purpose */ }

            /**
             * Construct particle.
             *
             * Insert A' into the Geant4 particle table.
             * Geant4 registers all instances derived from G4ParticleDefinition
             * and deletes them at the end of processing.
             *
             * Uses the A' mass given by the parameter APrimeMass to 
             * inform the G4APrime instance what mass to use.
             *
             * @see G4APrime
             */
            void ConstructParticle();

            /**
             * Construct the process.
             *
             * Links the dark brem processs to the electron through the process manager
             * only if the dark brem process is enabled ('enable' is True).
             *
             * G4ProcessManager registers and cleans up any created processes,
             * so we can forget about it after giving them to it.
             *
             * We also add a biasing interface for the custom process.
             * This is required for our biasing framework to work with
             * this new process.
             *
             * @see G4eDarkBremsstrahlung
             */
            void ConstructProcess();

        private:

            /// dark brem process parameters
            Parameters parameters_;
    };

}

#endif
