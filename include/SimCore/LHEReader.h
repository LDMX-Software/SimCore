/**
 * @file LHEReader.h
 * @brief Class for reading LHE event data
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 */

#ifndef SIMCORE_LHEREADER_H_
#define SIMCORE_LHEREADER_H_

// LDMX
#include "SimCore/LHEEvent.h"
#include "Framework/Logger.h"

// STL
#include <fstream>

namespace ldmx {

    /**
     * @class LHEReader
     * @brief Reads LHE event data into an LHEEvent object
     */
    class LHEReader {

        public:

            /**
             * Class constructor.
             * @param fileName The input file name.
             */
            LHEReader(std::string& fileName);

            /**
             * Class destructor.
             */
            virtual ~LHEReader();

            /**
             * Read the next event.
             * @return The next LHE event.
             */
            LHEEvent* readNextEvent();

        private:

            /**
             * The input file stream.
             */
            std::ifstream ifs_;

            /// Enable logging for this class
            enableLogging("LHEReader");
    };

}

#endif
