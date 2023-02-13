#ifndef SIMCORE_BERTINI_NOTHINGHARD_MODEL_H
#define SIMCORE_BERTINI_NOTHINGHARD_MODEL_H
#include <G4CascadeInterface.hh>
#include <G4HadronInelasticProcess.hh>
/* #include <G4PhotoNuclearXS.hh> */
#include <G4CascadeInterface.hh>
#include <G4CrossSectionDataSetRegistry.hh>
#include <G4HadProjectile.hh>
#include <G4Nucleus.hh>
#include <G4PhotoNuclearCrossSection.hh>
#include <G4ProcessManager.hh>

#include "SimCore/PhotonuclearModel.h"
namespace simcore {
class BertiniNothingHardProcess : public G4CascadeInterface {
 public:
  explicit BertiniNothingHardProcess(G4double threshold)
      : threshold_{threshold} {}
  virtual ~BertiniNothingHardProcess() = default;

  virtual G4HadFinalState* ApplyYourself(const G4HadProjectile& projectile,
                                         G4Nucleus& targetNucleus) override {
    while (true) {
      std::cout << "Applying BertiniNothingHard!\n";
      theParticleChange.Clear();
      theParticleChange.SetStatusChange(stopAndKill);

      G4CascadeInterface::ApplyYourself(projectile, targetNucleus);
      if (targetNucleus.GetZ_asInt() < 74) {
        return &theParticleChange;
      }
      /* double totalEnergy {0}; */
      int Nhard = 0;
      for (int i{0}; i < theParticleChange.GetNumberOfSecondaries(); ++i) {
        const auto secondary{theParticleChange.GetSecondary(i)->GetParticle()};
        const auto pdg{secondary->GetDefinition()->GetPDGEncoding()};
        if (pdg > 10000) {
          continue;
        }
        const auto energy{secondary->GetKineticEnergy()};
        if (energy > threshold_) {
          Nhard++;
        }
      }
      if (Nhard == 0) {
        return &theParticleChange;
      } else {
        // Geant4 won't explicitly clear this for us

        for (int i{0}; i < theParticleChange.GetNumberOfSecondaries(); ++i) {
          auto secondary{theParticleChange.GetSecondary(i)->GetParticle()};
          delete secondary;
        }
      }
    }
    // The secondaries will be returned in G4HadFinalState &theResult -
    // initialise this.  The original track will always be discontinued and
    // secondaries followed.
  }

 private:
  G4double threshold_;
};

class BertiniNothingHardModel : public PhotonuclearModel {
 public:
  BertiniNothingHardModel(const std::string& name,
                          const framework::config::Parameters& parameters)
      : PhotonuclearModel{name, parameters} {
    nothing_hard_threshold_ =
        parameters.getParameter<double>("nothing_hard_threshold");
  }
  void ConstructModel(G4ProcessManager* processManager) override {
    auto photoNuclearProcess{
        new G4HadronInelasticProcess("photonNuclear", G4Gamma::Definition())};
    auto model{new BertiniNothingHardProcess{nothing_hard_threshold_}};
    model->SetMaxEnergy(10 * CLHEP::GeV);
    auto crossSectionRegistry{G4CrossSectionDataSetRegistry::Instance()};
    auto crossSection{
        crossSectionRegistry->GetCrossSectionDataSet("PhotoNuclearXS")};
    if (!crossSection) {
      crossSection = new G4PhotoNuclearCrossSection{};
    }
    photoNuclearProcess->AddDataSet(crossSection);
    photoNuclearProcess->RegisterMe(model);
    processManager->AddDiscreteProcess(photoNuclearProcess);
  }

 private:
  double nothing_hard_threshold_;
};
}  // namespace simcore

#endif /* SIMCORE_BERTINI_NOTHINGHARD_MODEL_H */
