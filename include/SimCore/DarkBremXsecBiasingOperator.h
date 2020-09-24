/**
 * @file DarkBremXsecBiasingOperator.h
 * @brief Geant4 Biasing Operator used to bias the occurence of dark brem
 *        events by modifying the cross-section.
 * @author Michael Revering, University of Minnesota
 * @author Tom Eichlersmith, University of Minnesota
 */

#ifndef SIMCORE_DARKBREMXSECBIASINGOPERATOR_H
#define SIMCORE_DARKBREMXSECBIASINGOPERATOR_H

//----------//
//   LDMX   //
//----------//
#include "SimCore/XsecBiasingOperator.h"

class G4Track;
class G4BiasingProcessInterface;
class G4VBiasingOperation;

namespace ldmx { 

    class DarkBremXsecBiasingOperator : public XsecBiasingOperator { 

        public: 

            /** 
             * Constructor 
             *
             * Calls base class constructor.
             */
            DarkBremXsecBiasingOperator(std::string name);

            /** 
             * Destructor 
             *
             * Blank right now
             */
            ~DarkBremXsecBiasingOperator();

            /** 
             * Method called at the beginning of a run. 
             *
             * @sa XsecBiasingOperator::StartRun()
             */
            void StartRun();

            /** 
             * This the following protected member variables from XsecBiasingOperator:
             *  - biasAll_ : If true, bias all particles connected to the dark brem process;
             *      otherwise, only bias the primary particle (ParentID == 0)
             *  - xsecFactor_ : Factor to multiply cross section by
             *  - xsecOperator : Geant4 biasing operator to use
             *
             * @param[in] track const pointer to track to Bias
             * @param[in] callingProcess process that might be biased by this operator
             * @return Method that returns the biasing operation that will be used
             *         to bias the occurence of events.
             */
            G4VBiasingOperation* ProposeOccurenceBiasingOperation(const G4Track* track,
                    const G4BiasingProcessInterface* callingProcess);

        protected:

            /** DEBUG FUNCTION
             * This function is called by the biasing interface class during PostStepDoIt.
             * You can observe the particle change that was produced by the process
             * and the weight that will be multiplied into this particle change.
             *
             * This is called inside G4VBiasingOperator::ReportOperationApplied
             * which is called inside G4BiasingProcessInterface::PostStepDoIt
            void OperationApplied(const G4BiasingProcessInterface* callingProcess,
                    G4BiasingAppliedCase biasingCase,
                    G4VBiasingOperation* operationApplied,
                    G4double weight,
                    G4VBiasingOperation* finalStateOpApplied,
                    const G4VParticleChange* particleChangeProduced 
                    ) {
                std::string currentProcess = callingProcess->GetWrappedProcess()->GetProcessName(); 
                if (currentProcess.compare(this->getProcessToBias()) == 0) { 
                    std::cout << "DB Final State Biasing Operator Applied: " 
                        << callingProcess->GetProcessName()
                        << " -> " << weight*particleChangeProduced->GetWeight()
                        << std::endl;
                }
            }
             */

            /// Return the name of the process this operator biases
            virtual std::string getProcessToBias() { return DARKBREM_PROCESS; }

        private: 

            /// Geant4 name of dark brem process
            static const std::string DARKBREM_PROCESS;

    };  // DarkBremXsecBiasingOperator
}

#endif // SIMCORE_DARKBREMXSECBIASINGOPERATOR_H
