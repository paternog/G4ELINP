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
// Gian: july 2016
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ELINP_SteppingAction.hh"
#include "G4ELINP_EventAction.hh"
#include "G4ELINP_DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

#include "G4String.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_SteppingAction::G4ELINP_SteppingAction(G4ELINP_EventAction* eventAction):
G4UserSteppingAction(),
fEventAction(eventAction),
fSensitiveVolume(0)
{
	G4cout << "### SteppingAction intantiated ###" << G4endl;
	
	//get the Sensitive Volume   
	if (!fSensitiveVolume) { 
   		const G4ELINP_DetectorConstruction* detectorConstruction = static_cast<const G4ELINP_DetectorConstruction*>
        	(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    	fSensitiveVolume = detectorConstruction->GetSensitiveVolume();
    	G4cout << "### Sensitivite Volume name is: " << fSensitiveVolume->GetName() <<  " ###" << G4endl;
  	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_SteppingAction::~G4ELINP_SteppingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_SteppingAction::UserSteppingAction(const G4Step* step)
{
  	//get the volume of the current step
  	G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  	
  	//get trackID
  	G4int ID = step->GetTrack()->GetTrackID();
  	
  	//set temp: 0 if this is not a new track, -1 default
  	G4int temp = -1;
  	std::map<G4int,G4int>::iterator it;
  	it = fNinteractions.find(ID);
    if (it != fNinteractions.end()) { 	//if ID is found
  	  temp = 0;
  	}
  	
  	//if we have a new track or a new event -> reset variable values to 0
  	if (temp == -1 || fEventAction->GetNewEventFlag() == true) { 	
  		fNinteractions[ID] = 0;						
  		fSTEPinSD[ID] = 0;	
  		fEventAction->SetNewEventFlag() == false;	//switch newEvent Flag
  	}
  	
  	//increment interaction Number
    if (HadInteraction(step)) fNinteractions[ID]++; 	
  	
    //set Interaction Number for the particle when it enters the Sensitive Volume		
    if ((volume == fSensitiveVolume) && fSTEPinSD[ID] == 0) {
		fEventAction->SetInteractionNumber(ID, fNinteractions[ID]);
		fSTEPinSD[ID]++;
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool G4ELINP_SteppingAction::HadInteraction(const G4Step* step)
{
    G4String Process; 
    if(step->GetPostStepPoint()->GetProcessDefinedStep() != NULL) {
    	Process = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    }
    else {Process = "UserLimit";}
   
    if(Process == "Transportation" || Process == "CoupledTransportation") return false;
    
    else return true;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
