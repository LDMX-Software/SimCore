
#include "SimCore/DarkBrem/DMG4Model.h"

#include "Framework/Exception/Exception.h"
#include "Framework/Logger.h"
#include "SimCore/DarkBrem/G4APrime.h"

namespace simcore {
namespace darkbrem {

DMG4Model::DMG4Model(framework::config::Parameters &params)
    : G4eDarkBremsstrahlungModel(params) {
  double apmass = G4APrime::APrime()->GetPDGMass()/CLHEP::GeV;
  double threshold = std::max(params.getParameter<double>("threshold"), 2.*apmass);
  epsilon_ = params.getParameter<double>("epsilon");
  dm_model_ = std::make_unique<DarkPhotons>(apmass,threshold);
  dm_model_->PrepareTable();
}

void DMG4Model::PrintInfo() const {
  G4cout << " DMG4 DarkPhotons Model" << G4endl;
  G4cout << "   Threshold [GeV]: " << dm_model_->GetEThresh() << G4endl;
  G4cout << "   Epsilon:         " << dm_model_->Getepsil() << G4endl;
}

void DMG4Model::RecordConfig(ldmx::RunHeader &h) const {
  h.setFloatParameter("Minimum Threshold to DB [GeV]", dm_model_->GetEThresh());
  h.setFloatParameter("DB Xsec Epsilon", dm_model_->Getepsil());
}

G4double DMG4Model::ComputeCrossSectionPerAtom(
    G4double electronKE, G4double A, G4double Z) {
  electronKE /= GeV; //DMG4 uses GeV internally
  if (electronKE < dm_model_->GetEThresh()) return 0.;  // outside viable region for model
  return epsilon_*epsilon_*dm_model_->GetSigmaTot(electronKE)/dm_model_->GetMeanFreePathFactor();
}

/**
 * Almost direct copy of DMG4::DMProcessDMBrem::PostStepDoIt
 *
 * ## Differences
 * - removed references to myDarkMatter in favor of dm_model_
 * - use our A' definition instead of DMG4's
 * - kill parent lepton and produce new one for easier extraction of kinematics
 */
void DMG4Model::GenerateChange(
    G4ParticleChange &particleChange, const G4Track &track,
    const G4Step &step) {
  const G4double incidentE = track.GetTotalEnergy();
  G4ThreeVector incidentDir = track.GetMomentumDirection();
  const G4double incidentKinE = track.GetKineticEnergy();

  G4double XAcc=0., angles[2];
  if(dm_model_->GetParentPDGID() == 11)
    if(dm_model_->Decay()) {
      XAcc = dm_model_->SimulateEmissionWithAngle2(incidentE/GeV, angles);
    } else {
      XAcc = dm_model_->SimulateEmission(incidentE/GeV, angles);
    }
  if(dm_model_->GetParentPDGID() == 13) {
    // 2-dim sampling, angles are for the recoil muon
    //XAcc = dm_model_->SimulateEmissionByMuon(incidentE/GeV, angles);
    // 2-step sampling, angles are for the recoil muon
    XAcc = dm_model_->SimulateEmissionByMuon2(incidentE/GeV, angles); 
  }

  // Check if it failed? In this case XAcc = 0

  G4double recoilE = incidentKinE - incidentE * XAcc,
           recoilTheta = 0.,
           recoilPhi = 0.;
  G4double DMTheta = angles[0], DMPhi = angles[1];
  if(dm_model_->GetParentPDGID() == 13) {
    recoilTheta = angles[0];
    recoilPhi = angles[1];
    DMTheta = 0.;
    DMPhi = 0.;
  }
  G4double DME = incidentE * XAcc;
  G4double DMM = dm_model_->GetMA()*GeV;
  G4double DMKinE = DME*DME - DMM*DMM;
  if(DMKinE < 0.) DMKinE = 0.;
  DMKinE = sqrt(DMKinE);

  // Initialize DM direction vector:
  G4ThreeVector DMDirection(0., 0., .1);
  DMDirection.setMag(1.);
  DMDirection.setTheta( DMTheta );
  DMDirection.setPhi( DMPhi );
  DMDirection.rotateUz(incidentDir);
  // Initialize new projectile particle direction vector:
  G4ThreeVector projDirection(0., 0., 1.);
  projDirection.setMag(1.);
  projDirection.setTheta( recoilTheta );
  projDirection.setPhi( recoilPhi );
  projDirection.rotateUz(incidentDir);

  G4DynamicParticle *dphoton = new G4DynamicParticle(G4APrime::APrime(), DMDirection, DMKinE);
  G4DynamicParticle *el = new G4DynamicParticle(track.GetDefinition(), projDirection,recoilE);
  particleChange.SetNumberOfSecondaries(2);
  particleChange.AddSecondary(dphoton);
  particleChange.AddSecondary(el);
  particleChange.ProposeTrackStatus(fStopAndKill);
}

}  // namespace darkbrem
}  // namespace simcore
