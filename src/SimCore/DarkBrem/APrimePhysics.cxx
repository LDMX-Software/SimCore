/**
 * @file APrimePhysics.cxx
 * @brief Class which defines basic APrime physics
 * @author Michael Revering, University of Minnesota
 * @author Tom Eichlersmith, University of Minnesota
 */

#include "SimCore/DarkBrem/APrimePhysics.h"

// LDMX
#include "SimCore/DarkBrem/G4APrime.h"

// Geant4
#include "G4Electron.hh"
#include "G4MuonMinus.hh"
#include "G4ProcessManager.hh"

namespace simcore {
namespace darkbrem {

const std::string APrimePhysics::NAME = "APrime";

APrimePhysics::APrimePhysics(const framework::config::Parameters &params)
    : G4VPhysicsConstructor(APrimePhysics::NAME), parameters_{params} {
  ap_mass_ = parameters_.getParameter<double>("ap_mass", 0.) * MeV;
  enable_ = parameters_.getParameter<bool>("enable", false);
  muons_ = parameters_.getParameter<bool>("muons", false);
}

void APrimePhysics::ConstructParticle() {
  /**
   * Insert A-prime into the Geant4 particle table.
   * For now we flag it as stable.
   *
   * Geant4 registers all instances derived from G4ParticleDefinition and
   * deletes them at the end of the run.
   */
  G4APrime::APrime(ap_mass_);
}

void APrimePhysics::ConstructProcess() {
  // add process to electron if LHE file has been provided
  if (enable_) {
    /*
     * In G4 speak, a "discrete" process is one that only happens at the end of
     * steps. we want the DB to be discrete because it is not a "slow braking"
     * like ionization, the electron suddenly has the interaction and loses a
     * lot of its energy.
     *
     * The first argument to this function is the process we are adding.
     *      The process manager handles cleaning up the processes,
     *      so we just give it a new pointer.
     * The second argument is the "ordering" index.
     *      This index determines when the process is called w.r.t. the other
     * processes that could be called at the end of the step. Not providing the
     * second argument means that the ordering index is given a default value of
     * 1000 which seems to be safely above all the internal/default processes.
     */
    G4ParticleDefinition* particle_def{G4Electron::ElectronDefinition()};
    if (muons_) {
      particle_def = G4MuonMinus::Definition();
    }
    std::cout << "[ APrimePhysics ] : Connecting dark brem to " 
      << particle_def->GetParticleName() << " "
      << particle_def->GetPDGEncoding() << std::endl;
    auto proc = new G4eDarkBremsstrahlung(parameters_);
    G4int ret = particle_def->GetProcessManager()->AddDiscreteProcess(proc);
    //G4int ret = particle_def->GetProcessManager()->AddProcess(proc,-1,1,1);
    if (ret < 0) {
      EXCEPTION_RAISE("DarkBremReg","Particle process manager returned non-zero status "
          +std::to_string(ret)
          + " when attempting to register dark brem to it.");
    } else {
      std::cout << "[ APrimePhysics ] : successfully put dark brem in index " 
        << ret << " of process table." << std::endl;
    }
    /**
     * have our custom dark brem process go first in any process ordering
     */
    particle_def->GetProcessManager()->SetProcessOrderingToFirst(proc,
        G4ProcessVectorDoItIndex::idxAll);
    std::cout << "[ APrimePhysics ] : set dark brem process ordering to first" << std::endl;
  }
}

}  // namespace darkbrem
}  // namespace simcore
