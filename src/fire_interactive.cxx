
#include <memory>
#include <stdio.h>

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/Interactive.h" 

using namespace ldmx; 

int main(int argc, char** argv) {

    // If a python configuration has been passed, configure the interactive 
    // sesssion before starting. Otherwise, just start the session.
    std::unique_ptr< Interactive > session{nullptr}; 
    if (argc == 1) {
        session = std::make_unique< Interactive >(); 
    } else {
    } 
        
    // Start the interactive session
    session->startSession(argc, argv); 
}
