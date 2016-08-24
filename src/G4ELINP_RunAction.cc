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

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4ELINP_RunAction.hh"
#include "G4ELINP_RunActionMessenger.hh"
#include "G4ELINP_Analysis.hh"
#include "G4ELINP_Run.hh"
#include "G4ELINP_DetectorConstruction.hh"

#include "G4SDManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_RunAction::G4ELINP_RunAction():
G4UserRunAction(),
fMessenger(0)
{
	G4RunManager::GetRunManager()->SetPrintProgress(100000);
    
    fMessenger = new G4ELINP_RunActionMessenger(this);

    fFileName = "G4ELINP";
    
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    G4cout << "Using " << analysisManager->GetType() << " as Analysis Manager" << G4endl << G4endl;
    
    //Creating ntuples
    analysisManager->CreateNtuple("part","Particle");
    analysisManager->CreateNtupleDColumn("e");
    analysisManager->CreateNtupleDColumn("posx");
    analysisManager->CreateNtupleDColumn("posy");
    analysisManager->CreateNtupleDColumn("posz");
    analysisManager->CreateNtupleDColumn("momx");
    analysisManager->CreateNtupleDColumn("momy");
    analysisManager->CreateNtupleDColumn("momz");
    analysisManager->CreateNtupleDColumn("t");
    analysisManager->CreateNtupleIColumn("type");
    analysisManager->CreateNtupleIColumn("Z");
    analysisManager->CreateNtupleIColumn("A");
    analysisManager->CreateNtupleIColumn("trackID");
    analysisManager->CreateNtupleIColumn("trackIDP");
    analysisManager->CreateNtupleIColumn("interactions");
    analysisManager->FinishNtuple();

  	//add new units for dose
  	const G4double milligray = 1.e-3*gray;
  	const G4double microgray = 1.e-6*gray;
  	const G4double nanogray  = 1.e-9*gray;  
  	const G4double picogray  = 1.e-12*gray;
   
  	new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
  	new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
  	new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
  	new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);    
  	
  	G4cout << "### RunAction intantiated ###" << G4endl;    	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_RunAction::~G4ELINP_RunAction(){delete G4AnalysisManager::Instance();}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run* G4ELINP_RunAction::GenerateRun(){return new G4ELINP_Run;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile(fFileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_RunAction::EndOfRunAction(const G4Run* run)
{
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();    
    analysisManager->Write();
    analysisManager->CloseFile();
    
    G4int nofEvents = run->GetNumberOfEvent();
  	if (nofEvents == 0) return;
  
  	//results
  	const G4ELINP_Run* g4eliRun = static_cast<const G4ELINP_Run*>(run);
  	G4int nbGoodEvents = g4eliRun->GetNbGoodEvents(); 
  	G4double sumDose   = g4eliRun->GetSumDose();                           
        
  	//print
  	if (IsMaster())
  	{
    	G4cout
     	<< G4endl
     	<< "--------------------End of Global Run-----------------------"
     	<< G4endl
     	<< " The run had " << nofEvents << " events";
  	}
  	else
  	{
    	G4cout
     	<< G4endl
     	<< "--------------------End of Local Run------------------------"
     	<< G4endl
     	<< " The run had " << nofEvents << " events";
  	}      
  	
  	G4cout
    << "; Nb of 'good' events: " << nbGoodEvents << G4endl 
    << G4endl
    << " Total dose in solid : " << sumDose/gray << " Gray" << G4endl  
    << "------------------------------------------------------------" << G4endl 
    << G4endl;
    
    //Gian: introduced to improve information saved in SeedsAndDose_summary.dat
    const G4ELINP_DetectorConstruction* detectorConstruction = static_cast<const G4ELINP_DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  	G4String DoseDetName = detectorConstruction->GetScoringVolume()->GetName();
    
    if (IsMaster())
  	{
    	std::ofstream fFileOut;
        fFileOut.open("SeedsAndDose_summary.dat",std::ofstream::out | std::ofstream::app);
        fFileOut << G4Random::getTheSeed() << " | " << sumDose/gray << " Gray" << " | " << DoseDetName << std::endl;
        fFileOut.close();
  	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_RunAction::SetFileName(G4String filename){if(filename != ""){fFileName = filename;}}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
