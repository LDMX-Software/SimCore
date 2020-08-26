#include "SimCore/Interactive.h" 

/*~~~~~~~~~~~~*/
/*   Geant4   */
/*~~~~~~~~~~~~*/
#include "G4UIExecutive.hh"
#include "G4RunManager.hh"
#include "G4GDMLParser.hh"
#include "G4VisExecutive.hh"

/*~~~~~~~~~~~~~~~*/
/*   Framework   */
/*~~~~~~~~~~~~~~~*/
#include "Framework/Process.h" 

/*~~~~~~~~~~~~~*/
/*   SimCore   */
/*~~~~~~~~~~~~~*/
#include "SimCore/DetectorConstruction.h"
#include "SimCore/RunManager.h"

namespace ldmx { 

    Interactive::Interactive() {
    
        // Set some defaults for ECal hex readout.  These are needed 
        // because the ECal SD instatiates the hex readout by default.  The
        // parameters here are what is used by the v12 iteration of the 
        // ECal. All distances are in mm.  
        std::map <std::string,std::any > hexReadoutParams;
        
        std::vector<double> ecalSensLayersZ = { 7.850, 13.300, 26.400, 33.500,
            47.950, 56.550, 72.250, 81.350, 97.050, 106.150, 121.850, 130.950, 
            146.650, 155.750, 171.450, 180.550, 196.250, 205.350, 221.050, 
            230.150, 245.850, 254.950, 270.650, 279.750, 298.950, 311.550, 
            330.750, 343.350, 362.550, 375.150, 394.350, 406.950, 426.150, 
            438.750
        };

        hexReadoutParams["layerZPositions"] = ecalSensLayersZ;
        hexReadoutParams["ecalFrontZ"]      = 220.;
        hexReadoutParams["moduleMinR"]      = 85.0;
        hexReadoutParams["gap"]             = 1.0;
        hexReadoutParams["nCellRHeight"]    = 35.3;
        hexReadoutParams["verbose"]         = 2;
       
        Parameters hexReadout;
        hexReadout.setParameters(hexReadoutParams); 

        parameters_.addParameter< Parameters >("ecalHexReadout", hexReadout);

        // Define a default generator.  This is simply defined to be an
        // electron gun fired from 0, 0, 0 with energy 4.0 GeV. 
        std::map< std::string, std::any > genParams;
        
        genParams["class_name"] = std::string("ldmx::ParticleGun"); 
        genParams["instance_name"] = std::string("gun"); 

        Parameters generator; 
        generator.setParameters(genParams); 

        parameters_.addParameter< std::vector< Parameters > >("generators", { generator });; 
    }

    Interactive::Interactive(Parameters& parameters)
        : parameters_(parameters) {
            Interactive(); 
    }

    Interactive::~Interactive() {};

    void Interactive::startSession(int argc, char** argv) {



        // Instantiate the run manager.  
        auto runManager{std::make_unique<RunManager>(parameters_)};

        // Start the interactive session
        auto ui{std::make_unique< G4UIExecutive >(argc, argv)};

        // Setup GDML parser and messenger.
        auto parser{std::make_unique< G4GDMLParser >()};
        auto gdmlMessenger{std::make_unique< G4GDMLMessenger >(parser.get())};

        // Set the DetectorConstruction instance used to build the detector 
        // from the GDML description. 
        runManager->SetUserInitialization( new DetectorConstruction( parser.get() , parameters_ ) );

        // Initialize G4 visualization framework.
        G4VisManager* visManager = new G4VisExecutive;
        visManager->Initialize();

        ui->SessionStart();
        
    } 
}
