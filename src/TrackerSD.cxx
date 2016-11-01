#include "SimApplication/TrackerSD.h"

// STL
#include <iostream>

// Geant4
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SDManager.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

// LDMX
#include "Event/RootEventWriter.h"
#include "DetDescr/IDField.h"

using event::RootEventWriter;

namespace sim {

TrackerSD::TrackerSD(G4String name, G4String theCollectionName, int theSubdetId, DetectorID* theDetId) :
    G4VSensitiveDetector(name),
    hitsCollection(0),
    currentEvent(0),
    subdetId(theSubdetId),
    detId(theDetId) {

    // Add the collection name to vector of names.
    this->collectionName.push_back(theCollectionName);

    // Register this SD with the manager.
    G4SDManager::GetSDMpointer()->AddNewDetector(this);

    // Set the subdet ID as it will always be the same for every hit.
    detId->setFieldValue("subdet", subdetId);
}

TrackerSD::~TrackerSD() {
}

G4bool TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {

    // Determine if current particle of this step is a Geantino.
    G4ParticleDefinition* pdef = aStep->GetTrack()->GetDefinition();
    bool isGeantino = false;
    if (pdef == G4Geantino::Definition() || pdef == G4ChargedGeantino::Definition()) {
        isGeantino = true;
    }

    // Get the edep from the step.
    G4double edep = aStep->GetTotalEnergyDeposit();

    // Skip steps with no energy dep which come from non-Geantino particles.
    if (edep == 0.0 && !isGeantino) {
        if (verboseLevel > 2) {
            std::cout << "TrackerSD skipping step with zero edep" << std::endl << std::endl;
        }
        return false;
    }

    // Create a new hit object.
    G4TrackerHit* hit = new G4TrackerHit();

    // Assign track ID for finding the SimParticle in post event processing.
    hit->setTrackID(aStep->GetTrack()->GetTrackID());

    // Set the edep.
    hit->setEdep(edep);

    // Set the start position.
    G4StepPoint* prePoint = aStep->GetPreStepPoint();
    //hit->setStartPosition(prePoint->GetPosition());

    // Set the end position.
    G4StepPoint* postPoint = aStep->GetPostStepPoint();
    //hit->setEndPosition(postPoint->GetPosition());

    G4ThreeVector start = prePoint->GetPosition();
    G4ThreeVector end = postPoint->GetPosition();

    // Set the mid position.
    G4ThreeVector mid = 0.5 * (start + end);
    hit->setPosition(mid.x(), mid.y(), mid.z());

    // Compute path length.
    G4double pathLength = sqrt(pow(start.x() - end.x(), 2) + pow(start.y() - end.y(), 2) + pow(start.z() - end.z(), 2));
    hit->setPathLength(pathLength);
 
    // Set the global time.
    hit->setTime(aStep->GetTrack()->GetGlobalTime());

    /*
     * Compute and set the momentum.
     */
    /*
    double mag = (prePoint->GetMomentum().mag() + postPoint->GetMomentum().mag()) / 2;
    G4ThreeVector p = (postPoint->GetPosition() - prePoint->GetPosition());
    if (mag > 0) {
        p.setMag(mag);
    }
    */
    G4ThreeVector p = postPoint->GetMomentum();
    hit->setMomentum(p.x(), p.y(), p.z());

    /*
     * Set the 32-bit ID on the hit.
     */
    int layerNumber = prePoint->GetTouchableHandle()->GetHistory()->GetVolume(2)->GetCopyNo();
    detId->setFieldValue(1, layerNumber);
    hit->setID(detId->pack());
    hit->setLayerID(layerNumber);

    /*
     * Debug print.
     */
    if (this->verboseLevel > 2) {
        std::cout << "Created new SimTrackerHit in detector " << this->GetName()
                << " with subdet ID " << subdetId << " and layer " << layerNumber << " ..." << std::endl;
        hit->Print();
        std::cout << std::endl;
    }

    // Insert hit into current hits collection.
    hitsCollection->insert(hit);

    return true;
}

void TrackerSD::Initialize(G4HCofThisEvent* hce) {

    // Setup hits collection and the HC ID.
    hitsCollection = new G4TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);
    int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, hitsCollection);
}

void TrackerSD::EndOfEvent(G4HCofThisEvent* hce) {

    // Print number of hits.
    if (this->verboseLevel > 0) {
        std::cout << GetName() << " had " << hitsCollection->entries()
                << " hits in event" << std::endl;
    }

    // Print each hit in hits collection.
    if (this->verboseLevel > 1) {
        for (int iHit = 0; iHit < hitsCollection->GetSize(); iHit++ ) {
            (*hitsCollection)[iHit]->Print();
        }
    }
}

}
