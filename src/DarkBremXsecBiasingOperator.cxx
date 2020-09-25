/**
 * @file DarkBremXsecBiasingOperator.cxx
 * @brief Geant4 Biasing Operator used to bias the occurence of dark brem
 *        events by modifying the cross-section.
 * @author Michael Revering, University of Minnesota
 * @author Tom Eichlersmith, University of Minnesota
 */

#include "SimCore/DarkBremXsecBiasingOperator.h"

//------------//
//   Geant4   //
//------------//
#include "G4BiasingProcessInterface.hh"
#include "G4Track.hh"
#include "G4VBiasingOperator.hh"
#include "G4RunManager.hh"

namespace ldmx { 

    DarkBremXsecBiasingOperator::DarkBremXsecBiasingOperator(std::string name) :
        XsecBiasingOperator(name) {
    }

    DarkBremXsecBiasingOperator::~DarkBremXsecBiasingOperator() {
    }

    void DarkBremXsecBiasingOperator::StartRun() { 
        XsecBiasingOperator::StartRun(); 
    }

    G4VBiasingOperation* DarkBremXsecBiasingOperator::ProposeOccurenceBiasingOperation(
            const G4Track* track, const G4BiasingProcessInterface* callingProcess) {

        std::string currentProcess = callingProcess->GetWrappedProcess()->GetProcessName(); 
        if (currentProcess.compare(this->getProcessToBias()) == 0) { 
            //only bias the process that we want to DARKBREM_PROCESS
                        
            //bias only the primary particle if we don't want to bias all particles
            if (not biasAll_ and track->GetParentID() != 0) return 0; 
    
            G4double interactionLength = callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();

            double dbXsecUnbiased = 1./interactionLength;
            double dbXsecBiased   = dbXsecUnbiased*xsecFactor_; 

            if ( G4RunManager::GetRunManager()->GetVerboseLevel() > 1 ) {
                std::cout << "[ DarkBremXsecBiasingOperator ]: "
                    << " Unbiased DBrem xsec: " << dbXsecUnbiased
                    << " -> Biased xsec: " << dbXsecBiased 
                    << std::endl;
            }

            //xsecOperation is a protected member variable of XsecBiasingOperator
            //  it is set in XsecBiasingOperator::StartRun()
            xsecOperation->SetBiasedCrossSection(dbXsecBiased);
            xsecOperation->Sample();
            return xsecOperation;
    
        } else return 0; 

    }
}
