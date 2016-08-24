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
// $Id: G4ELINP_PhysicsList.hh 2014-03-13 11:28:03Z ebagli $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef G4ELINP_PhysicsList_h
#define G4ELINP_PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"
#include <vector>

class G4VPhysicsConstructor;
class G4ProductionCuts;
class G4ELINP_PhysicsListMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4ELINP_PhysicsList: public G4VModularPhysicsList
{
public:
    G4ELINP_PhysicsList();
    virtual ~G4ELINP_PhysicsList();
    
    virtual void ConstructParticle();
    
    virtual void SetCuts();
    void SetCutForGamma(G4double);
    void SetCutForElectron(G4double);
    void SetCutForPositron(G4double);
    
    void SelectPhysicsList(const G4String& name);
    
    virtual void ConstructProcess();
    
    void SetTargetCut(G4double val);
    void SetDetectorCut(G4double val);
    
private:  
    //hide assignment operator
    G4ELINP_PhysicsList & operator = (const G4ELINP_PhysicsList &right);
    G4ELINP_PhysicsList(const G4ELINP_PhysicsList&);
    
    G4VPhysicsConstructor*  fParticleList;  
    G4VPhysicsConstructor*  fRaddecayList;
    G4VPhysicsConstructor*  fEmPhysicsList;
    G4VPhysicsConstructor*  fEmExtraPhysicsList;
    G4VPhysicsConstructor*  fHadPhysicsList;
    G4VPhysicsConstructor*  fHadElPhysicsList;
    G4VPhysicsConstructor*  fIonPhysicsList;
    G4VPhysicsConstructor*  fStoppingPhysicsList;
    
    G4double fCutForGamma;
    G4double fCutForElectron;
    G4double fCutForPositron;

    G4ProductionCuts* fDetectorCuts;
    G4ProductionCuts* fTargetCuts;
    
    G4ELINP_PhysicsListMessenger* fPMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

