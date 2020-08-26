#ifndef SIMCORE_INTERACTIVE_H
#define SIMCORE_INTERACTIVE_H

#include <memory> 

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Parameters.h"

namespace ldmx {

    class Process; 

    /**
     * @name Interactive 
     * @brief Class used to configure and start a Geant4 interactive session. 
    */    
    class Interactive { 

        public: 

            /// Default constructor
            Interactive(); 

            /**
             * Constructor
             *
             * @param[in] parameters Set of parameters used to configure this 
             *  session.
             */
            Interactive(Parameters& parameters);  


            /// Destructor
            ~Interactive();

            /**
             * Start an Geant4 interactive session.  This 
             * allows a user manually execute Geant4 macro 
             * commands via the command line. 
             */
            void startSession(int argc, char** argv); 

        private:

            /// Parameter set used to configure this session.
            Parameters parameters_;

            /// Process
            std::unique_ptr< Process > process_{nullptr};  


    }; 

} // ldmx

#endif // SIMCORE_INTERACTIVE_H
