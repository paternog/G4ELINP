//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4ELINP_SensitiveDetector.hh"
#include "G4ELINP_SensitiveDetectorHit.hh"
#include "G4ELINP_DetectorConstruction.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4Navigator.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_SensitiveDetector::G4ELINP_SensitiveDetector(G4String name):
G4VSensitiveDetector(name)
{
    G4String HCname;
    collectionName.insert(HCname="collection");
    fHCID = -1;
    bVarStopAndKill = false;
    fSDMessenger = new G4ELINP_SensitiveDetectorMessenger(this);
    
    G4cout << "### SensitiveDetector intantiated ###" << G4endl << G4endl;
       
    //get the value of bScoringCSPEC in DetectorConstruction
   	const G4ELINP_DetectorConstruction* detectorConstruction = static_cast<const G4ELINP_DetectorConstruction*>
        	(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    varScoringCSPEC = detectorConstruction->GetScoringCSPEC();
    varSensitiveIndex = detectorConstruction->GetSensitiveIndex();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_SensitiveDetector::~G4ELINP_SensitiveDetector() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4ELINP_SensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
    fHitsCollection = new G4ELINP_SensitiveDetectorHitsCollection(SensitiveDetectorName, collectionName[0]);
    if(fHCID<0){fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);}
    HCE->AddHitsCollection(fHCID, fHitsCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

bool G4ELINP_SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{   
    G4Track* vTrack = aStep->GetTrack();
    
    G4StepPoint* preStepPoint = aStep->GetPreStepPoint();

    if(!(preStepPoint->GetStepStatus() == fGeomBoundary)) {return true;}
    
    G4ThreeVector pos = preStepPoint->GetPosition();
    G4ThreeVector mom = preStepPoint->GetMomentumDirection();
    
    G4int vType = -1;
    if(preStepPoint->GetMass() == 0 && preStepPoint->GetCharge() == 0){
        vType = 0;
    }
    else if(preStepPoint->GetMass() != 0 && preStepPoint->GetCharge() == 0){
        vType = 1;
    }
    else if(preStepPoint->GetMass() != 0 && preStepPoint->GetCharge() > 0){
        vType = 2;
    }
    else if(preStepPoint->GetMass() != 0 && preStepPoint->GetCharge() < 0){
        vType = 3;
    }
    
    G4ELINP_SensitiveDetectorHit* aHit = new G4ELINP_SensitiveDetectorHit();

    aHit->SetMom(mom);
    aHit->SetPos(pos);
    aHit->SetType(vType);
    aHit->SetTime(preStepPoint->GetGlobalTime());
    aHit->SetEnergy(preStepPoint->GetKineticEnergy());     
    aHit->SetTrackID(vTrack->GetTrackID());
    aHit->SetTrackIDP(vTrack->GetParentID());   
    aHit->SetA(vTrack->GetDefinition()->GetAtomicMass());
    aHit->SetZ(vTrack->GetDefinition()->GetAtomicNumber());
   
    fHitsCollection->insert(aHit);
    
    //Gian: add to avoid retracking the same particle in case of scattering
    if (bVarStopAndKill && varScoringCSPEC != 0 && varSensitiveIndex == 1) {
    	vTrack->SetTrackStatus(fStopAndKill); 
    }
    
    return true;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4ELINP_SensitiveDetector::EndOfEvent(G4HCofThisEvent*) {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
