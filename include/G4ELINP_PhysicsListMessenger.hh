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

#ifndef G4ELINP_PhysicsListMessenger_h
#define G4ELINP_PhysicsListMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class G4ELINP_PhysicsList;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4ELINP_PhysicsListMessenger: public G4UImessenger
{

public:  
	G4ELINP_PhysicsListMessenger(G4ELINP_PhysicsList*);
  	virtual ~G4ELINP_PhysicsListMessenger();
    
  	virtual void SetNewValue(G4UIcommand*, G4String);
    
private: 
  	G4ELINP_PhysicsList* fPPhysicsList;

  	G4UIdirectory* fPhysDir;  

  	G4UIcmdWithADoubleAndUnit* fGammaCutCmd;
  	G4UIcmdWithADoubleAndUnit* fElectCutCmd;
  	G4UIcmdWithADoubleAndUnit* fProtoCutCmd;    
  	G4UIcmdWithADoubleAndUnit* fAllCutCmd;    
  	G4UIcmdWithADoubleAndUnit* fMCutCmd;
  	G4UIcmdWithADoubleAndUnit* fECutCmd;
  	G4UIcmdWithAString*        fPListCmd;
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

