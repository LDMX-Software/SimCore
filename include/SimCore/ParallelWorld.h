/**
 * 
 */

#ifndef SIMCORE_PARALLELWORLD_H_
#define SIMCORE_PARALLELWORLD_H_

//---------------//
//   C++StdLib   //
//---------------//
#include <string>

//------------//
//   Geant4   //
//------------//
#include "G4GDMLParser.hh"
#include "G4String.hh"
#include "G4VUserParallelWorld.hh"

//-------------//
//   ldmx-sw   //
//-------------//
#include "SimCore/AuxInfoReader.h"
#include "Framework/Logger.h"

namespace ldmx { 

    class ParallelWorld : public G4VUserParallelWorld { 

        public: 

            /** Constructor */
            ParallelWorld(G4GDMLParser* parser, G4String worldName);

            /** Destructor */
            ~ParallelWorld();

            /** */
            void Construct();

            /** */
            void ConstructSD();

        private:

            /// Enable logging for us
            enableLogging("ParallelWorld");

            /** GDML parser */
            G4GDMLParser* parser_{nullptr};
            
            /** The auxiliary GDML info reader. */
            AuxInfoReader* auxInfoReader_{nullptr};

    }; // ParallelWorld
}

#endif // SIMCORE_PARALLELWORLD_H_

