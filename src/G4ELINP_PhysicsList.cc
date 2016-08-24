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
// $Id: G4ELINP_PhysicsList.cc 73290 2013-08-23 10:04:20Z gcosmo $
//
//=================================================================================
//PAOLO CARDARELLI, amendements 02/09/2014:
//1 edited G4HadronPhysicsQGSP_BIC(verboseLevel) -> G4HadronPhysicsQGSP_BIC_HP(verboseLevel)
//2 commented AddExtraPhysics();
//3 fEmPhysicsList = new G4EmStandardPhysics(verboseLevel); Option_4 added
//=================================================================================
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ELINP_PhysicsList.hh"
#include "G4ELINP_PhysicsListMessenger.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLowEPPhysics.hh"

#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4UnitsTable.hh"
#include "G4LossTableManager.hh"

#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronPhysicsINCLXX.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsShielding.hh"

#include "G4EmExtraPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4IonPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4DecayPhysics.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_PhysicsList::G4ELINP_PhysicsList() :
G4VModularPhysicsList(),
fParticleList(0),
fRaddecayList(0),
fEmPhysicsList(0),
fHadPhysicsList(0),
fCutForGamma(0.001*mm),
fCutForElectron(0.001*mm),
fCutForPositron(0.001*mm),
fDetectorCuts(0),
fTargetCuts(0),
fPMessenger(0)
{
    G4LossTableManager::Instance();
    
    defaultCutValue = 0.001*mm;
    
    fPMessenger = new G4ELINP_PhysicsListMessenger(this);
    
    SetVerboseLevel(0);
    
    fParticleList = new G4DecayPhysics(verboseLevel);    
    fRaddecayList = new G4RadioactiveDecayPhysics(verboseLevel);    
    fEmPhysicsList = new G4EmStandardPhysics_option4(verboseLevel);
    fHadPhysicsList = new G4HadronPhysicsQGSP_BIC_HP(verboseLevel);
    fHadElPhysicsList = new G4HadronElasticPhysicsHP(verboseLevel);  
	fEmExtraPhysicsList = new G4EmExtraPhysics(verboseLevel);
    fIonPhysicsList = new G4IonPhysics(verboseLevel);        
	fStoppingPhysicsList = new G4StoppingPhysics(verboseLevel);        
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_PhysicsList::~G4ELINP_PhysicsList()
{
    //delete fPMessenger;
    //delete fParticleList;
    //delete fRaddecayList;
    //delete fEmPhysicsList;
    //delete fHadPhysicsList;
    //if(fEmExtraPhysicsList){delete fEmExtraPhysicsList;}
    //if(fIonPhysicsList){delete fIonPhysicsList;}
    //if(fStoppingPhysicsList){delete fStoppingPhysicsList;}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PhysicsList::ConstructParticle()
{
    fParticleList->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PhysicsList::ConstructProcess()
{     
    ConstructParticle();

	AddTransportation();

    fEmPhysicsList->ConstructProcess();    
    fHadPhysicsList->ConstructProcess();	
	fHadElPhysicsList->ConstructProcess();
    fEmExtraPhysicsList->ConstructProcess();
    fIonPhysicsList->ConstructProcess();
    fStoppingPhysicsList->ConstructProcess();
    
    G4cout << "### G4ELINP_PhysicsList::ConstructProcess is done ###" << G4endl << G4endl;     
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PhysicsList::SelectPhysicsList(const G4String& name)
{
    if (verboseLevel>1) {
        G4cout << "G4ELINP_PhysicsList::SelectPhysicsList: <" << name << ">" << G4endl;
    }
    if (name == "QGSP_BIC") {
        delete fHadPhysicsList;
        fHadPhysicsList = new G4HadronPhysicsQGSP_BIC(verboseLevel);
    } else if (name == "QGSP_BIC_HP") {
        delete fHadPhysicsList;
        fHadPhysicsList = new G4HadronPhysicsQGSP_BIC_HP(verboseLevel);
    } else if (name == "QGSP_BERT") {
        delete fHadPhysicsList;
        fHadPhysicsList = new G4HadronPhysicsQGSP_BERT(verboseLevel);
    } else if (name == "INCLXX") {
        delete fHadPhysicsList;
        fHadPhysicsList = new G4HadronPhysicsINCLXX(verboseLevel);
    } else if (name == "FTFP_BERT") {
        delete fHadPhysicsList;
        fHadPhysicsList = new G4HadronPhysicsFTFP_BERT(verboseLevel);
    } else if (name == "QBBC") {
        delete fHadPhysicsList;
        fHadPhysicsList = new G4HadronInelasticQBBC(verboseLevel);
    } else if (name == "Shielding") {
        delete fHadPhysicsList;
        fHadPhysicsList = new G4HadronPhysicsShielding(verboseLevel);
    } else if (name == "emlivermore") {
        delete fEmPhysicsList;
        fEmPhysicsList = new G4EmLivermorePhysics(verboseLevel);
    } else if (name == "empenelope") {
        delete fEmPhysicsList;
        fEmPhysicsList = new G4EmPenelopePhysics(verboseLevel);
    } else if (name == "emstandard") {
        delete fEmPhysicsList;
        fEmPhysicsList = new G4EmStandardPhysics(verboseLevel);
    } else if (name == "emstandard_opt4") {
        delete fEmPhysicsList;
        fEmPhysicsList = new G4EmStandardPhysics_option4(verboseLevel);
    } else if (name == "LowEP") {
        delete fEmPhysicsList;
        fEmPhysicsList = new G4EmLowEPPhysics(1,name);
    } else {
        G4cout << "G4ELINP_PhysicsList WARNING wrong or unkonwn <" << name << "> Physics " << G4endl;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PhysicsList::SetCuts()
{
    SetCutValue(fCutForGamma, "gamma");
    SetCutValue(fCutForElectron, "e-");
    SetCutValue(fCutForPositron, "e+");
    G4cout << "world cuts are set" << G4endl;
    
    if(!fTargetCuts) SetTargetCut(fCutForElectron);
    G4Region* region = (G4RegionStore::GetInstance())->FindOrCreateRegion("Target");
    region->SetProductionCuts(fTargetCuts);
    G4cout << "Target cuts are set" << G4endl;
    
    if(!fDetectorCuts) SetDetectorCut(fCutForElectron);
    region = (G4RegionStore::GetInstance())->FindOrCreateRegion("Detector");
    region->SetProductionCuts(fDetectorCuts);
    G4cout << "Detector cuts are set" << G4endl << G4endl;
    
    DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PhysicsList::SetCutForGamma(G4double cut)
{
    fCutForGamma = cut;
    SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PhysicsList::SetCutForElectron(G4double cut)
{
    fCutForElectron = cut;
    SetParticleCuts(fCutForElectron, G4Electron::Electron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PhysicsList::SetCutForPositron(G4double cut)
{
    fCutForPositron = cut;
    SetParticleCuts(fCutForPositron, G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PhysicsList::SetTargetCut(G4double cut)
{
    if(!fTargetCuts) fTargetCuts = new G4ProductionCuts();    
    fTargetCuts->SetProductionCut(cut, idxG4GammaCut);
    fTargetCuts->SetProductionCut(cut, idxG4ElectronCut);
    fTargetCuts->SetProductionCut(cut, idxG4PositronCut);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_PhysicsList::SetDetectorCut(G4double cut)
{
    if(!fDetectorCuts) fDetectorCuts = new G4ProductionCuts();   
    fDetectorCuts->SetProductionCut(cut, idxG4GammaCut);
    fDetectorCuts->SetProductionCut(cut, idxG4ElectronCut);
    fDetectorCuts->SetProductionCut(cut, idxG4PositronCut);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
