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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ELINP_PrimaryGeneratorActionMessenger.hh"
#include "G4ELINP_PrimaryGeneratorAction.hh"

#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_PrimaryGeneratorActionMessenger::G4ELINP_PrimaryGeneratorActionMessenger(G4ELINP_PrimaryGeneratorAction* stack):
fPrimaryGeneratorAction(stack)
{
	fReadFromFileCmd = new G4UIcmdWithABool("/gun/ReadFromFile", this);
  	fReadFromFileCmd->SetGuidance("Choice : (1) true, (0) false.");
  	fReadFromFileCmd->SetParameterName("choice", true);
  	fReadFromFileCmd->SetDefaultValue(false);

  	fSetFileNameCmd = new G4UIcmdWithAString("/gun/SetFileName", this);
  	fSetFileNameCmd->SetGuidance("Set File Name");
  	fSetFileNameCmd->SetParameterName("filaname", true);
  	fSetFileNameCmd->SetDefaultValue("");
  	fSetFileNameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_PrimaryGeneratorActionMessenger::~G4ELINP_PrimaryGeneratorActionMessenger()
{
  	delete fReadFromFileCmd;
  	delete fSetFileNameCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  	if(command == fReadFromFileCmd) {
    	fPrimaryGeneratorAction->ReadFromFile(fReadFromFileCmd->GetNewBoolValue(newValue));
  	} 
  	else if(command == fSetFileNameCmd) {
    	fPrimaryGeneratorAction->SetFileName(newValue);
  	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

