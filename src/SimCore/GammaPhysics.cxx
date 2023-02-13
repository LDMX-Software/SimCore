/**
 * @file GammaPhysics.cxx
 * @brief Class used to enhanced the gamma physics list.
 * @author Jeremy McCormick, SLAC National Accelerator Laboratory
 * @author Omar Moreno, SLAC National Accelerator Laboratory
 */

#include "SimCore/GammaPhysics.h"

namespace simcore {

GammaPhysics::GammaPhysics(const G4String& name)
    : G4VPhysicsConstructor(name) {}

GammaPhysics::~GammaPhysics() {}

// needed for GEANT4 10.3.0 and later
#ifndef aParticleIterator
#define aParticleIterator \
  ((subInstanceManager.offset[g4vpcInstanceID])._aParticleIterator)
#endif

void GammaPhysics::SetPhotonNuclearAsFirstProcess() const {
  auto processManager{GetGammaProcessManager()};
  const auto processes{processManager->GetProcessList()};
  for (int i{0}; i < processes->size(); ++i) {
    const auto process{(*processes)[i]};
    const auto processName{process->GetProcessName()};
    if (processName == "photonNuclear") {
      processManager->SetProcessOrderingToFirst(
          process, G4ProcessVectorDoItIndex::idxAll);
    }
  }
}

G4ProcessManager* GammaPhysics::GetGammaProcessManager() const {
  aParticleIterator->reset();

  while ((*aParticleIterator)()) {
    auto particle{aParticleIterator->value()};
    G4String particleName = particle->GetParticleName();
    if (particleName == "gamma") {
      return aParticleIterator->value()->GetProcessManager();
    }
  }
}
void GammaPhysics::AddGammaMuMu() {
  auto processManager{GetGammaProcessManager()};

  // Add the gamma -> mumu to the physics list.
  processManager->AddDiscreteProcess(&gammaConvProcess);
}

void GammaPhysics::ConstructProcess() {
  // aParticleIterator->reset();

  auto processManager{GetGammaProcessManager()};

  SetPhotonNuclearAsFirstProcess();
  AddGammaMuMu();
}
}  // namespace simcore
