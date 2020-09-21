/**
 * @file G4Session.h
 * @brief Classes which redirect the output of G4cout and G4cerr
 * @author Tom Eichlersmith, University of Minnesota
 */

#ifndef SIMCORE_G4SESSION_H_
#define SIMCORE_G4SESSION_H_

#include <iostream>
#include <fstream>

// Geant4
#include "G4UIsession.hh"

// LDMX
#include "Framework/Logger.h"

namespace ldmx {

    /**
     * @class LoggedSession
     *
     * Log the output of Geant4 to the central logger.
     */
    class LoggedSession : public G4UIsession {
        
        public:

            /**
             * Constructor
             */
            LoggedSession() { }

            /**
             * Destructor
             */
            ~LoggedSession() { }

            /**
             * Required hook for Geant4
             *
             * Does nothing
             */
            G4UIsession* SessionStart() { return nullptr; }

            /**
             * Redirects cout to file
             */
            G4int ReceiveG4cout(const G4String& message);

            /**
             * Redirects cerr to file
             */
            G4int ReceiveG4cerr(const G4String& message);

        private:

            /// Use this class to log messages from Geant4
            enableLogging("Geant4");

    }; //LoggedSession

    /**
     * @class BatchSession
     *
     * Do _nothing_ with G4cout and G4cerr messages. This is made to improve performance.
     */
    class BatchSession : public G4UIsession {
        
        public:

            /**
             * Constructor
             */
            BatchSession() { }

            /**
             * Destructor
             */
            ~BatchSession() { }

            /**
             * Required hook for Geant4
             *
             * Does nothing
             */
            G4UIsession* SessionStart() { return nullptr; }

            /**
             * Does nothing with input
             */
            G4int ReceiveG4cout(const G4String&) { return 0; }

            /**
             * Does nothing with input
             */
            G4int ReceiveG4cerr(const G4String&) { return 0; }

    };

}

#endif
