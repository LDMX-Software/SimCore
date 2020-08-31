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
     * An electron is allowed to brem a dark photon
     *
     * @TODO allow positrons to dark brem as well
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
             * Reports the parameters to G4cout.
             *
             * @note Needs to match variable definitions in python configuration class.
             */
            virtual void PrintInfo();
     
        protected:
      
            /** Setup this process to get ready for simulation 
             *
             * If the model hasn't been initialized before, we create
             * a new G4eDarkBremsstrahlungModel and pass parameters to it.
             * This model is stored in the list of models for this process
             * and in the list of all EM models. Geant4 cleans up the list
             * of all EM models at the end of processing.
             *
             * The low energy limit of the model is set to  the minimum kinetic energy
             * of this process (which I believe is zero), and the high energy limit
             * is set to 4 GeV. The secondary threshold for the model is set to zero,
             * and the LPM flag is set to false.
             *
             * @TODO make high energy limit for the model configurable
             *
             * @param p unused
             * @param q unused
             */
            virtual void InitialiseEnergyLossProcess(const G4ParticleDefinition* p,
                                                     const G4ParticleDefinition* q);
      
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
