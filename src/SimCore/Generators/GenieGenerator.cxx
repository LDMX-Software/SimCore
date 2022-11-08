/**
 * @file GenieGenerator.cxx
 * @brief Simple GENIE event generator.
 * @author Wesley Ketchum, FNAL
 */

#include "SimCore/Generators/GenieGenerator.h"
#include "SimCore/UserPrimaryParticleInformation.h"

// GENIE
#include "Framework/Utils/AppInit.h"
#include "GENIE/Framework/Utils/RunOpt.h"
#include "Framework/Utils/XSecSplineList.h"
#include "GENIE/Framework/Interaction/InitialState.h"
#include "Framework/EventGen/EventRecord.h"
#include "Framework/GHEP/GHepParticle.h"


#include "Framework/Conventions/XmlParserStatus.h"
#include "Framework/Conventions/GBuild.h"
#include "Framework/Conventions/Controls.h"
#include "Framework/EventGen/EventRecord.h"
#include "Framework/EventGen/GFluxI.h"
#include "Framework/EventGen/GEVGDriver.h"
#include "Framework/EventGen/GMCJDriver.h"
#include "Framework/EventGen/GMCJMonitor.h"
#include "Framework/Interaction/Interaction.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/Ntuple/NtpWriter.h"
#include "Framework/Ntuple/NtpMCFormat.h"
#include "Framework/Numerical/RandomGen.h"
#include "Framework/Numerical/Spline.h"
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/Utils/AppInit.h"
#include "Framework/Utils/RunOpt.h"
#include "Framework/Utils/XSecSplineList.h"
#include "Framework/Utils/StringUtils.h"
#include "Framework/Utils/PrintUtils.h"
#include "Framework/Utils/SystemUtils.h"
#include "Framework/Utils/CmdLnArgParser.h"


// Geant4
#include "G4Event.hh"
#include "G4PhysicalConstants.hh"

// ROOT
#include <TParticle.h>
#include <TVector3.h>
#include <TLorentzVector.h>

// standard
#include <iostream>
#include <sstream>

namespace simcore {
namespace generators {

void GenieGenerator::fillConfig(const framework::config::Parameters& p)
{

  energy_      = p.getParameter<double>("energy");// * GeV;

  targets_     = p.getParameter< std::vector<int> >("targets");
  abundances_  = p.getParameter< std::vector<double> >("abundances");

  time_        = p.getParameter<double>("time");// * ns;
  position_    = p.getParameter< std::vector<double> >("position");
  direction_   = p.getParameter< std::vector<double> >("direction");

  tune_        = p.getParameter<std::string>("tune");
  spline_file_ = p.getParameter<std::string>("spline_file");
  seed_        = p.getParameter<int>("seed");


  for(size_t i=0; i<position_.size(); ++i)
    position_[i] = position_[i];// * mm;
}

bool GenieGenerator::validateConfig()
{
  bool ret=true;
  
  if (targets_.size()==0 || abundances_.size()==0)
    {
      std::cout << "targets and/or abundances sizes are zero."
		<< "  " << targets_.size() << ", " << abundances_.size()
		<< std::endl;
      ret=false;
    }
  if (targets_.size() != abundances_.size())
    {
      std::cout << "targets and abundances sizes unequal."
		<< "  " << targets_.size() << " != " << abundances_.size()
		<< std::endl;
      ret=false;
    }

  if (position_.size()!=3 || direction_.size()!=3)
    {
      std::cout << "position and/or direction sizes are not 3."
		<< "  " << position_.size() << ", " << direction_.size()
		<< std::endl;
      ret=false;
    }


  //normalize abundances
  float abundance_sum=0;
  for( auto a : abundances_)
    abundance_sum+=a;

  if( std::abs(abundance_sum)<1e-6)
    {
      std::cout << "abundances list sums to zero? " << abundance_sum << std::endl;
      ret=false;
    }
  
  for(size_t i_a; i_a<abundances_.size(); ++i_a)
    abundances_[i_a] = abundances_[i_a]/abundance_sum;


  double dir_total_sq=0;
  for( auto d : direction_)
    dir_total_sq += d*d;
  
  if( dir_total_sq<1e-6)
    {
      std::cout << "direction vector is zero or negative? "
		<< "(" << direction_[0] << "," << direction_[1] << "," << direction_[2] << ")"
		<< std::endl;
      ret=false;
    }
  for(size_t i_d; i_d<direction_.size(); ++i_d)
    direction_[i_d] = direction_[i_d]/std::sqrt(dir_total_sq);
  
  
  return ret;
}

void GenieGenerator::initializeGENIE()
{
  char *inarr[3] = {const_cast<char*>(""),
		    const_cast<char*>("--event-generator-list"),
		    const_cast<char*>("EM")};
  genie::RunOpt::Instance()->ReadFromCommandLine(3,inarr);
  
  genie::RunOpt::Instance()->SetTuneName(tune_);
  if ( ! genie::RunOpt::Instance()->Tune() ) {
    EXCEPTION_RAISE("ConfigurationException","No TuneId in RunOption.");
  }
  genie::RunOpt::Instance()->BuildTune();

  genie::utils::app_init::RandGen(seed_);
  genie::utils::app_init::XSecTable(spline_file_, false);
  genie::GHepRecord::SetPrintLevel(1);

  evg_driver_.SetEventGeneratorList("EM");
  evg_driver_.SetUnphysEventMask(*genie::RunOpt::Instance()->UnphysEventMask());
  
}

  
GenieGenerator::GenieGenerator(const std::string& name,
			       const framework::config::Parameters& p)
    : PrimaryGenerator(name, p)
{
  
  fillConfig(p);

  if(!validateConfig())
    EXCEPTION_RAISE("ConfigurationException","Configuration not valid.");

  initializeGENIE();
  
}

GenieGenerator::~GenieGenerator() {}
  
void GenieGenerator::GeneratePrimaryVertex(G4Event* event)
{  
  //for now, just the first. later picking a target from list
  genie::InitialState initial_state(targets_.front(),11);
  evg_driver_.Configure(initial_state);

  //setup the initial election
  TParticle initial_e;
  initial_e.SetPdgCode(11);
  auto elec_i_p = std::sqrt(energy_*energy_ - initial_e.GetMass()*initial_e.GetMass());
  initial_e.SetMomentum(elec_i_p*direction_[0],
			elec_i_p*direction_[1],
			elec_i_p*direction_[2],
			energy_);
  TLorentzVector e_p4;
  initial_e.Momentum(e_p4);

  //GENIE magic
  genie::EventRecord *genie_event = NULL;
  while(!genie_event)
    genie_event = evg_driver_.GenerateEvent(e_p4);

  //setup the primary vertex now
  G4PrimaryVertex* vertex = new G4PrimaryVertex();
  vertex->SetPosition(position_[0],
		      position_[1],
		      position_[2]);
  vertex->SetWeight(genie_event->Weight());
  
  //loop over the entries and add to the G4Event
  int nentries = genie_event->GetEntries();

  for(int i_p=0; i_p<nentries; ++i_p)
    {
      genie::GHepParticle *p = (genie::GHepParticle*)(*genie_event)[i_p];

      //make sure it's a final state particle
      if (p->Status()!=1)
	continue;

      std::cout << "\tAdding particle " << p->Pdg() << " with status " << p->Status() << " energy " << p->E() << " ..." << std::endl;

      G4PrimaryParticle* primary = new G4PrimaryParticle();
      primary->SetPDGcode(p->Pdg());
      primary->Set4Momentum(p->Px()*CLHEP::GeV, p->Py()*CLHEP::GeV, p->Pz()*CLHEP::GeV, p->E()*CLHEP::GeV);
      primary->SetProperTime(time_*CLHEP::ns);

      UserPrimaryParticleInformation* primaryInfo =
	new UserPrimaryParticleInformation();
      primaryInfo->setHepEvtStatus(1);
      primary->SetUserInformation(primaryInfo);
      
      vertex->SetPrimary(primary);
    }

  //add the vertex to the event
  event->AddPrimaryVertex(vertex);

  delete genie_event;
  
}

void GenieGenerator::RecordConfig(const std::string& id, ldmx::RunHeader& rh) {
  rh.setStringParameter(id+" Class", "simcore::generators::GenieGenerator");
}

}  // namespace generators
}  // namespace simcore

DECLARE_GENERATOR(simcore::generators::GenieGenerator)
