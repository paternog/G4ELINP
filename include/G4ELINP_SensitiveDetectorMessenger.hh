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

#ifndef G4ELINP_SensitiveDetectorMessenger_h
#define G4ELINP_SensitiveDetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4ELINP_SensitiveDetector;
class G4UIdirectory;
class G4UIcmdWithABool;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4ELINP_SensitiveDetectorMessenger : public G4UImessenger
{
public:
	G4ELINP_SensitiveDetectorMessenger(G4ELINP_SensitiveDetector*);
  	~G4ELINP_SensitiveDetectorMessenger();
    
  	void SetNewValue(G4UIcommand*, G4String);
    
private: 
  	G4ELINP_SensitiveDetector* fSenDet;  
  	G4UIdirectory* fSenDetDir; 
  	G4UIcmdWithABool* fUserStopAndKillCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
