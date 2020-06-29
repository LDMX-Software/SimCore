/**
 * @file G4eDarkBremsstrahlung.h
 * @brief Class providing the Dark Bremsstrahlung process class.
 * @author Michael Revering, University of Minnesota
 * @author Tom Eichlersmith, University of Minnesota
 */

#ifndef SIMCORE_G4EDARKBREMSSTRAHLUNG_H_
#define SIMCORE_G4EDARKBREMSSTRAHLUNG_H_

#include "Framework/Parameters.h"

// Geant
#include "G4VEnergyLossProcess.hh"

class G4String;
class G4ParticleDefinition;

namespace ldmx {
    
    /**
     * @class G4eDarkBremsstrahlung
     *
     * Class that represents the dark brem process.
     * A electron or positron is allowed to brem a dark photon
     */
    class G4eDarkBremsstrahlung : public G4VEnergyLossProcess {
    
        public:
      
            /**
             * Constructor
             *
             * Sets this process up
             */
            G4eDarkBremsstrahlung(Parameters& params, const G4String& name = "eDBrem");
      
            /**
             * Destructor
             */
            virtual ~G4eDarkBremsstrahlung() { /*Nothing on purpose*/ }
      
            /** 
             * Checks if the passed particle should be able to do this process
             *
             * @return true if particle is electron
             */
            virtual G4bool IsApplicable(const G4ParticleDefinition& p);
      
            /**
             * Reports the file name and the method (in string form)
             *
             * @TODO actually write this up in a helpful way
             */
            virtual void PrintInfo();
     
        protected:
      
            /** Setup this process to get ready for simulation */
            virtual void InitialiseEnergyLossProcess(const G4ParticleDefinition*,
                                                     const G4ParticleDefinition*);
      
            /** Has this process been setup yet? */
            G4bool isInitialised;
     
        private:
      
            /** remove ability to assign this object */
            G4eDarkBremsstrahlung & operator=(const G4eDarkBremsstrahlung &right);
    
            /** remove ability to copy construct */
            G4eDarkBremsstrahlung(const G4eDarkBremsstrahlung&);
    
            /// Parameters to pass onto the dark brem model upon initialisation
            Parameters& parameters_;
    
    };

}

#endif // SIMCORE_G4EDARKBREMSSTRAHLUNG_H_
