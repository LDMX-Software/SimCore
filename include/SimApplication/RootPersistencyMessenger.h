/**
 * @file RootPersistencyMessenger.h
 * @brief Class providing macro commands for ROOT persistency
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef SIMAPPLICATION_ROOTPERSISTENCYMESSENGER_H_
#define SIMAPPLICATION_ROOTPERSISTENCYMESSENGER_H_

// Geant4
#include "G4UImessenger.hh"
#include "G4UIcmdWithABool.hh"

// LDMX
#include "SimApplication/RootPersistencyManager.h"

namespace ldmx {

    /**
     * @class RootPersistencyMessenger
     * @brief Provides macro commands for RootPersistencyManager
     */
    class RootPersistencyMessenger : public G4UImessenger {

        public:

            /**
             * Class constructor.
             * @param rootIO The ROOT persistency manager.
             */
            RootPersistencyMessenger(RootPersistencyManager* rootIO);

            /**
             * Class destructor.
             */
            virtual ~RootPersistencyMessenger();

            /**
             * Process macro command.
             * @param command The applicable UI command.
             * @param newValues The argument values.
             */
            void SetNewValue(G4UIcommand* command, G4String newValues);

        private:

            /** ROOT persistency macro directory. */
            G4UIdirectory* rootDir_{nullptr};

            /** Command used to specify the name of the output file. */
            G4UIcommand* rootFileCmd_{nullptr};

            /** Command used to set the verbosity level. */
            G4UIcommand* verboseCmd_{nullptr};

            /** Command to disable the ROOT IO. */
            G4UIcommand* disableCmd_{nullptr};

            /** Command used to re-enable the ROOT IO after being disabled. */
            G4UIcommand* enableCmd_{nullptr};

            /** Command used to specify the ROOT file compression level. */
            G4UIcommand* comprCmd_{nullptr};
            
            /** 
             * Command used to enable/disable saving of the hit contributions
             * to an Ecal hit.
             */
            G4UIcmdWithABool* hitContribsCmd_{nullptr};

            /** Command used to compress the Ecal hit contributions. */
            G4UIcmdWithABool* compressContribsCmd_{nullptr};

            /** Command allowing a user to specify a collection name to drop. */
            G4UIcmdWithAString* dropCmd_{nullptr}; 

            /** Pointer to ROOT persistency manager. */
            RootPersistencyManager* rootIO_{nullptr};
    };

}

#endif
