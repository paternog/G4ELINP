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

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

#include "G4ELINP_EventAction.hh"
#include "G4ELINP_SensitiveDetectorHit.hh"
#include "G4ELINP_TrackingAction.hh"
#include "G4ELINP_Analysis.hh"
#include "G4ELINP_RunAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_EventAction::G4ELINP_EventAction():
fG4ELINP_SensitiveDetector_ID(-1),
fVerboseLevel(0)
{
	G4cout << "### EventAction intantiated ###" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_EventAction::~G4ELINP_EventAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4ELINP_EventAction::BeginOfEventAction(const G4Event*)
{
	newEvent = true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4ELINP_EventAction::EndOfEventAction(const G4Event* aEvent)
{    
	//instantiating The Sensitive Detector Manager
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
        
    //Hit Detection System
    if(fG4ELINP_SensitiveDetector_ID==-1) {
    	G4String G4ELINP_SensitiveDetectorName;
        if(SDman->FindSensitiveDetector(G4ELINP_SensitiveDetectorName="det",0)){
            fG4ELINP_SensitiveDetector_ID = SDman->GetCollectionID(G4ELINP_SensitiveDetectorName="det/collection");
        }
    }
    
    G4ELINP_SensitiveDetectorHitsCollection* fG4ELINP_SensitiveDetector = 0;
    G4HCofThisEvent* HCE = aEvent->GetHCofThisEvent();
    
    if(HCE)
    {
        if(fG4ELINP_SensitiveDetector_ID != -1){
            G4VHitsCollection* aHCBeamPipe = HCE->GetHC(fG4ELINP_SensitiveDetector_ID);
            fG4ELINP_SensitiveDetector = (G4ELINP_SensitiveDetectorHitsCollection*)(aHCBeamPipe);
        }
    }
    
    //instantiating The Analysis Manager
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        
    if(fG4ELINP_SensitiveDetector){
        int vNumberOfHitBeamPipe = fG4ELINP_SensitiveDetector->entries();
        for(int i=0; i<vNumberOfHitBeamPipe; i++){
            G4ELINP_SensitiveDetectorHit* aHitBP = (*fG4ELINP_SensitiveDetector)[i];
            analysisManager->FillNtupleDColumn(0,0,aHitBP->GetEnergy()/CLHEP::MeV);
            analysisManager->FillNtupleDColumn(0,1,aHitBP->GetPos().x()/CLHEP::mm);
            analysisManager->FillNtupleDColumn(0,2,aHitBP->GetPos().y()/CLHEP::mm);
            analysisManager->FillNtupleDColumn(0,3,aHitBP->GetPos().z()/CLHEP::mm);
            analysisManager->FillNtupleDColumn(0,4,aHitBP->GetMom().x());
            analysisManager->FillNtupleDColumn(0,5,aHitBP->GetMom().y());
            analysisManager->FillNtupleDColumn(0,6,aHitBP->GetMom().z());
            analysisManager->FillNtupleDColumn(0,7,aHitBP->GetTime()/CLHEP::ns);
            analysisManager->FillNtupleIColumn(0,8,aHitBP->GetType());
            analysisManager->FillNtupleIColumn(0,9,aHitBP->GetZ());
            analysisManager->FillNtupleIColumn(0,10,aHitBP->GetA());
            analysisManager->FillNtupleIColumn(0,11,aHitBP->GetTrackID());
            analysisManager->FillNtupleIColumn(0,12,aHitBP->GetTrackIDP());
            analysisManager->FillNtupleIColumn(0,13,fInteractionNumber[aHitBP->GetTrackID()]);
            analysisManager->AddNtupleRow(0);          
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
