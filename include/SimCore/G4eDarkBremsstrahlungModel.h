/**
 * @file G4eDarkBremsstrahlungModel.h
 * @brief Class provided to simulate the dark brem cross section and interaction.
 * @author Michael Revering, University of Minnesota
 * @author Tom Eichlersmith, University of Minnesota
 */

#ifndef SIMCORE_G4EDARKBREMSSTRAHLUNGMODEL_H_
#define SIMCORE_G4EDARKBREMSSTRAHLUNGMODEL_H_

#include "Framework/Parameters.h"

// Geant
#include "G4VEmModel.hh"

// ROOT
#include "TLorentzVector.h"

namespace ldmx {
    
    /**
     * @class G4eDarkBremsstrahlungModel
     *
     * Geant4 implementation of the model for a particle undergoing a dark brem.
     *
     * This is where all the heavy lifting in terms of calculating cross sections
     * and actually having an electron do a dark brem occurs.
     */
    class G4eDarkBremsstrahlungModel : public G4VEmModel {
    
        public:
    
            /**
             * @enum DarkBremMethod
             *
             * Possible methods to use the dark brem vertices from the imported library inside of this model.
             */
            enum DarkBremMethod{
                /// Use actual electron energy and get pT from LHE (such that pT^2+me^2 < Eacc^2)
                ForwardOnly = 1,
                /// Boost LHE vertex momenta to the actual electron energy
                CMScaling   = 2, 
                /// Use LHE vertex as is
                Undefined   = 3  
            };
    
            /**
             * Constructor
             * Set the parameters for this model.
             *
             * The method integer is converted to an enum through a hard-coded
             * switch statement.
             *
             * The threshold is set to the maximum of the passed value or twice
             * the A' mass (so that it kinematically makes sense).
             *
             * The library path is immediately passed to SetMadGraphDataLibrary.
             */
            G4eDarkBremsstrahlungModel(Parameters& params,
                                       const G4ParticleDefinition* p = 0,
                                       const G4String& theName = "eDBrem");
    
            /**
             * Destructor
             *
             * Attempt to cleanup hanging pointers and leftover memory.
             */
            virtual ~G4eDarkBremsstrahlungModel();
    
            /**
             * Set the cuts using the particle definition and compute the partial sum sigma.
             *
             * Comput particleSumSigma using ComputeParticleSumSigma inputting the material,
             * half the high energy limit, and the minimum of the cut table or a quarter
             * of the high energy limit.
             *
             * @param p G4ParticleDefinition to compute partial sum sigma for
             * @param cuts energy cuts depending on material or volume or region the calculation is in
             */
            virtual void Initialise(const G4ParticleDefinition* p, const G4DataVector& cuts);
    
            /**
             * Simulates the emission of a dark photon + electron.
             *
             * Gets an energy fraction and Pt from madgraph files. 
             * The scaling of this energy fraction and Pt depends on the input method.
             *
             * ## Forward Only
             * Scales the energy so that the fraction of kinectic energy is constant, keeps the Pt constant. 
             * If the Pt is larger than the new energy, that event is skipped, and a new one is taken from the file.
             * Choses the Pz of the recoil electron to always be positive.
             *
             * ## CM Scaling
             * Scale MadGraph vertex to actual energy of electron using Lorentz boosts, and then
             * extract the momentum from that.
             *
             * ## Undefined
             * Don't scale the MadGraph vertex to the actual energy of the electron.
             *
             * If only one per event is set, then we deactivate the dark brem process,
             * ensuring only one dark brem per step and per event.
             * Needs to be reactived in the end of event action.
             *
             * @param secondaries vector of primary particle's offspring
             * @param primary particle that could go dark brem
             * @param tmin minimum energy possible for sampling
             * @param maxEnergy maximum energy possible for sampling
             */
            virtual void SampleSecondaries(std::vector<G4DynamicParticle*>* secondaries,
                                          const G4MaterialCutsCouple* ,
                                          const G4DynamicParticle* primary,
                                          G4double tmin,
                                          G4double maxEnergy);
    
        protected:
    
            /**
             * Selects random element out of the material information given.
             *
             * Uses weights for materials calculated before and stored in partialSumSigma
             *
             * @param couple G4 package containing material and other relevant information
             * @return the chosen G4Element
             */
            const G4Element* SelectRandomAtom(const G4MaterialCutsCouple* couple);
    
        private:
            
            /**
             * Set the library of dark brem events to be scaled.
             * @param file path to directory of LHE files
             */
            void SetMadGraphDataLibrary(std::string path);
    
            /**
             * Helpful typedef for boost integration.
             */
            typedef std::vector<double> StateType;
    
            /**
             * @struct Chi
             *
             * Stores parameters for chi function used in integration.
             * Implements function as member operator compatible with boost::numeric::odeint
             *
             * \f[ \chi(t) = \left( \frac{Z^2a^4t^2}{(1+a^2t)^2(1+t/d)^2}+\frac{Za_p^4t^2}{(1+a_p^2t)^2(1+t/0.71)^8}\left(\frac{1+t(m_{up}^2-1)}{4m_p^2}\right)^2\right)\frac{t-m_A^4/(4E_0^2)}{t^2} \f]
             *
             * where
             * \f$m_A\f$ = mass of A' in GeV, 
             * \f$m_e\f$ = mass of electron in GeV, 
             * \f$E_0\f$ = incoming energy of electron in GeV, 
             * \f$A\f$ = atomic number of target atom, 
             * \f$Z\f$ = atomic mass of target atom, 
             * \f[a = \frac{111.0}{m_e Z^{1/3}}\f]
             * \f[a_p = \frac{773.0}{m_e Z^{2/3}}\f]
             * \f[d = \frac{0.164}{A^{2/3}}\f]
             * \f$m_{up}\f$ = mass of up quark, and
             * \f$m_{p}\f$ = mass of proton
             */
            struct Chi {
                /// atomic number
                double A; 
                /// atomic mass
                double Z; 
                /// incoming beam energy [GeV]
                double E0;
                /// A' mass [GeV]
                double MA;
                /// electron mass [GeV]
                double Mel;
    
                /**
                 * Access function in style required by boost::numeric::odeint
                 *
                 * Calculates dxdt from t and other paramters.
                 */
                void operator()( const StateType &, StateType &dxdt , double t );
            };
    
            /**
             * @struct DiffCross
             *
             * Implementation of the differential scattering cross section.
             * Stores parameters.
             *
             * Implements function as member operator compatible with boost::numeric::odeint
             *
             * \f[ \frac{d\sigma}{dx}(x) = \sqrt{1-\frac{m_A^2}{E_0^2}}\frac{1-x+x^2/3}{m_A^2(1-x)/x+m_e^2x} \f]
             *
             * where
             * \f$m_A\f$ = mass of A' in GeV
             * \f$m_e\f$ = mass of electron in GeV
             * \f$E_0\f$ = incoming energy of electron in GeV
             */
            struct DiffCross {
                /// incoming beam energy [GeV]
                double E0; 
                /// A' mass [GeV]
                double MA; 
                /// electron mass [GeV]
                double Mel; 
    
                /**
                 * Access function in style required by boost::numeric::odeint
                 *
                 * Calculates DsigmaDx from x and other paramters.
                 */
                void operator()( const StateType &, StateType &DsigmaDx , double x );
            };
    
            /**
             * @struct OutgoingKinematics
             *
             * Data frame to store mad graph data read in from LHE files.
             */
            struct OutgoingKinematics {
                /// 4-momentum of electron in center of momentum frame for electron-A' system
                TLorentzVector electron; 
                /// 4-vector pointing to center of momentum frame
                TLorentzVector centerMomentum; 
                /// energy of electron before brem (used as key in mad graph data map)
                G4double E; 
            };
    
            /**
             * Sets the particle being looked at and checks whether it is an electron or not.
             */
            void SetParticle(const G4ParticleDefinition* p);
          
            /*
             * Parse an LHE File
             *
             * Parses an LHE file to extract the kinetic energy fraction and pt of the outgoing electron in each event. 
             * Loads the two numbers from every event into a map of vectors of pairs (mgdata). 
             * Map is keyed by energy, vector pairs are energy fraction + pt. 
             * Also creates an list of energies and placeholders (energies), so that different energies can be looped separately.
             *
             * @param fname name of LHE file to parse
             */
            void ParseLHE(std::string fname);
          
            /**
             * Fill vector of currentDataPoints_ with the same number of items as the madgraph data.
             *
             * Randomly choose a starting point so that the simulation run isn't dependent on
             * the order of LHE vertices in the library.
             */
            void MakePlaceholders();
    
            /**
             * Returns mad graph data given an energy [GeV].
             *
             * Gets the energy fraction and Pt from the imported LHE data. 
             * E0 should be in GeV, returns the total energy and Pt in GeV. 
             * Scales from the closest imported beam energy above the given value (scales down to avoid biasing issues).
             *
             * @param E0 energy of particle undergoing dark brem [GeV]
             * @return total energy and transverse momentum of particle [GeV]
             */
            G4eDarkBremsstrahlungModel::OutgoingKinematics GetMadgraphData(double E0);
    
            /**
             * Calculates the cross section per atom in GEANT4 internal units.
             * Uses WW approximation to find the total cross section, performing numerical integrals over x and theta.
             *
             * Non named parameters are not used.
             *
             * Numerical integrals are done using boost::numeric::odeint.
             *
             * Integrate Chi from \f$m_A^4/(4E_0^2)\f$ to \f$m_A^2\f$
             *
             * Integrate DiffCross from 0 to \f$min(1-m_e/E_0,1-m_A/E_0)\f$
             *
             * Total cross section is given by
             * \f[ \sigma = 4 \frac{pb}{GeV} \epsilon^2 \alpha_{EW}^3 \int \chi(t)dt \int \frac{d\sigma}{dx}(x)dx \f]
             *
             * @param E0 energy of beam (incoming particle)
             * @param Z atomic number of atom
             * @param A atomic mass of atom
             * @param cut minimum energy cut to calculate cross section
             * @return cross section (0. if outside energy cuts)
             */
            virtual G4double ComputeCrossSectionPerAtom(const G4ParticleDefinition*,
                                                       G4double E0, 
                                                       G4double Z,   G4double A,
                                                       G4double cut, G4double);
    
            /** 
             * Build the table of cross section per element. 
             *
             * The table is built for MATERIALS. 
             * This table is used by DoIt to select randomly an element in the material.
             *
             * @param material Material to build cross section table for
             * @param kineticEnergy kinetic energy of incoming particle
             * @param cut minimum energy cut on cross section
             * @return G4DataVector of elements to cross sections for the input material. 
             */
            G4DataVector* ComputePartialSumSigma(const G4Material* material,
                                                G4double kineticEnergy, G4double cut);
    
            /** Hide assignment operator */
            G4eDarkBremsstrahlungModel & operator=(const  G4eDarkBremsstrahlungModel &right);
    
            /** Hide copy constructor */
            G4eDarkBremsstrahlungModel(const  G4eDarkBremsstrahlungModel&);
    
        protected:
    
            /** definition of particle */
            const G4ParticleDefinition* particle_;
    
            /** loss in particle energy */
            G4ParticleChangeForLoss* fParticleChange_;
    
        private:
    
            /** 
             * maximum number of iterations to check before giving up on an event 
             *
             * @TODO make configurable and/or optimize somehow
             */
            unsigned int maxIterations_{10000};
    
            /** mass of the A Prime [GeV] 
             *
             * Configurable with 'APrimeMass'
             */
            double MA_;
    
            /** Threshold for non-zero xsec [GeV] 
             *
             * Configurable with 'threshold'
             */
            double threshold_;
    
            /** Epsilon value to plug into xsec calculation 
             *
             * @sa ComputeCrossSectionPerAtom for how this is used
             *
             * Configurable with 'epsilon'
             */
            double epsilon_;
    
            /** mass of an electron [GeV] */
            double Mel_;
    
            /** method for this model 
             *
             * Configurable with 'method'
             */
            DarkBremMethod method_{DarkBremMethod::Undefined};
    
            /** 
             * should we always create a totally new electron when we dark brem? 
             *
             * @TODO make this configurable? I (Tom E) can't think of a reason NOT to have it...
             * The alternative is to allow Geant4 to decide when to make a new particle
             * by checking if the resulting kinetic energy is below some threshold.
             */
            bool alwaysCreateNewElectron_{true};
    
            /**
             * Only allow the dark brem to happen once per event.
             *
             * This allows for the dark brem process to be de-activated when SampleSecondaries is called.
             *
             * The dark brem process is _always_ re-activated in the RunManager::TerminateOneEvent method.
             * This reactivation has no effect when the process is already active.
             */
            bool onlyOnePerEvent_;
    
            /** 
             * Storage of data from mad graph 
             *
             * Maps incoming electron energy to various options for outgoing kinematics.
             * This is a hefty map and is what stores **all** of the vertices
             * imported from the LHE library of dark brem vertices.
             *
             * Library is read in from configuration parameter 'darkbrem.madgraphlibrary'
             */
            std::map< double , std::vector < OutgoingKinematics > > madGraphData_;
    
            /**
             * Stores a map of current access points to mad graph data.
             *
             * Maps incoming electron energy to the index of the data vector
             * that we will get the data from.
             *
             * Also sorts the incoming electron energy so that we can find
             * the sampling energy that is closest above the actual incoming energy.
             */
            std::map< double , unsigned int > currentDataPoints_;
    
            
            /**
             * Not really sure what this physically represents
             *
             * This is calculated in ComputePartialSumSigma when
             * the Initialise method is called. It is then used
             * to help randomly choose whether a dark brem would
             * occur given a specific material + electron energy
             * calculation.
             */
            std::vector<G4DataVector*> partialSumSigma_;
      
    };
}

#endif // SIMCORE_G4EDARKBREMSSTRAHLUNGMODEL_H_
