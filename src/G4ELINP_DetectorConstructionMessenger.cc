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

#include "G4ELINP_DetectorConstructionMessenger.hh"
#include "G4ELINP_DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_DetectorConstructionMessenger::G4ELINP_DetectorConstructionMessenger(G4ELINP_DetectorConstruction* mpga):fDetectorTarget(mpga)
{
    fCollimatorDir = new G4UIdirectory("/collimator/");
    fCollimatorDir->SetGuidance("Detector Control.");

    fCollimatorSetupCmd = new G4UIcmdWithAnInteger("/line/set",this);
    fCollimatorSetupCmd->SetGuidance("Set line type 0->[1-3.5] MeV; 1->[3.5-20] MeV.");
    fCollimatorSetupCmd->SetParameterName("line",false);
    fCollimatorSetupCmd->SetRange("line>=0 && line<=1");
    fCollimatorSetupCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fCollimatorApertureCmd = new G4UIcmdWithADoubleAndUnit("/collimator/setAperture",this);
    fCollimatorApertureCmd->SetGuidance("Set collimator aperture.");
    fCollimatorApertureCmd->SetParameterName("aperture",false);
    fCollimatorApertureCmd->SetUnitCategory("Length");
    fCollimatorApertureCmd->SetRange("aperture>=0.0");
    fCollimatorApertureCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    for(G4int index=0; index<64; index++){
        char tempchar[64];
        sprintf(tempchar,"/collimator/setSingleDisplMean%02d",index);
        fCollimatorSingleDisplMeanCmd[index] = new G4UIcmdWithADoubleAndUnit(tempchar,this);
        fCollimatorSingleDisplMeanCmd[index]->SetGuidance("Set single aperture collimator mean.");
        sprintf(tempchar,"singlemean%02d",index);
        fCollimatorSingleDisplMeanCmd[index]->SetParameterName(tempchar,false);
        fCollimatorSingleDisplMeanCmd[index]->SetUnitCategory("Length");
        fCollimatorSingleDisplMeanCmd[index]->AvailableForStates(G4State_PreInit,G4State_Idle);
    }
    
    fCollimatorRandDisplCmd = new G4UIcmdWithAnInteger("/collimator/setRandDispl",this);
    fCollimatorRandDisplCmd->SetGuidance("Set random aperture collimator: (0) no random, (1) uniform, (2) gauss.");
    fCollimatorRandDisplCmd->SetParameterName("randdispl",false);
    fCollimatorRandDisplCmd->SetRange("randdispl>=0");
    fCollimatorRandDisplCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fCollimatorRandDisplMeanCmd = new G4UIcmdWithADoubleAndUnit("/collimator/setRandDisplMean",this);
    fCollimatorRandDisplMeanCmd->SetGuidance("Set random aperture collimator mean.");
    fCollimatorRandDisplMeanCmd->SetParameterName("randmean",false);
    fCollimatorRandDisplMeanCmd->SetUnitCategory("Length");
    fCollimatorRandDisplMeanCmd->SetRange("randmean>=0.0");
    fCollimatorRandDisplMeanCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fCollimatorRandDisplSigmaCmd = new G4UIcmdWithADoubleAndUnit("/collimator/setRandDisplSigma",this);
    fCollimatorRandDisplSigmaCmd->SetGuidance("Set random aperture collimator sigma.");
    fCollimatorRandDisplSigmaCmd->SetParameterName("randsigma",false);
    fCollimatorRandDisplSigmaCmd->SetUnitCategory("Length");
    fCollimatorRandDisplSigmaCmd->SetRange("randsigma>=0.0");
    fCollimatorRandDisplSigmaCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fCollimationEnvelopeMisalignmentCmd = new G4UIcmdWith3VectorAndUnit("/collimator/setMisalignment",this);
    fCollimationEnvelopeMisalignmentCmd->SetGuidance("Set collimator misalignment.");
    fCollimationEnvelopeMisalignmentCmd->SetParameterName("angX","angY","angZ",false);
    fCollimationEnvelopeMisalignmentCmd->SetUnitCategory("Angle");
    fCollimationEnvelopeMisalignmentCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fCollimationEnvelopeMispositionCmd = new G4UIcmdWith3VectorAndUnit("/collimator/setMisposition",this);
    fCollimationEnvelopeMispositionCmd->SetGuidance("Set collimator misposition.");
    fCollimationEnvelopeMispositionCmd->SetParameterName("posX","posY","posZ",false);
    fCollimationEnvelopeMispositionCmd->SetUnitCategory("Length");
    fCollimationEnvelopeMispositionCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  
            
    fCollimatorScoringCSPECCmd = new G4UIcmdWithAnInteger("/line/setcspec",this);
    fCollimatorScoringCSPECCmd->SetGuidance("Set scoring CSPEC 0->default; 1->cspecbox; 2->cspecbox_large.");
    fCollimatorScoringCSPECCmd->SetParameterName("cspec",false);
    fCollimatorScoringCSPECCmd->SetRange("cspec==0 || cspec==1 || cspec==2");
    fCollimatorScoringCSPECCmd->AvailableForStates(G4State_PreInit,G4State_Idle);   

    fModulesOnCmd = new G4UIcmdWithAnInteger("/line/modules",this);
    fModulesOnCmd->SetGuidance("Set modules On (1), Off (0).");
    fModulesOnCmd->SetParameterName("modules",false);
    fModulesOnCmd->SetRange("modules>=0");
    fModulesOnCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    fScoringIndexCmd = new G4UIcmdWithAnInteger("/line/setScoringIndex",this);
    fScoringIndexCmd->SetGuidance("Set scoring Index: 1->Collimator1 (default); 2->Hexapod; 3->Rack1; 4->Rack2; 5-17-> Collimator 2-14.");
    fScoringIndexCmd->SetParameterName("scoringIndex",false);
    fScoringIndexCmd->SetRange("scoringIndex > 0 && scoringIndex < 18");
    fScoringIndexCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  
    
    fSensitiveIndexCmd = new G4UIcmdWithAnInteger("/line/setSensitiveIndex",this);
    fSensitiveIndexCmd->SetGuidance("Set Sensitive Index: 1->old working (default); 2->all screen detectors; 3-> screen detectors with hole only; 3->Collimato1; 4->Hexapod; 5->Rack1; 6->Rack2.");
    fSensitiveIndexCmd->SetParameterName("sensitiveIndex",false);
    fSensitiveIndexCmd->SetRange("sensitiveIndex > 0 && sensitiveIndex < 8");
    fSensitiveIndexCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_DetectorConstructionMessenger::~G4ELINP_DetectorConstructionMessenger()
{
    delete fCollimatorDir;
    delete fCollimatorSetupCmd;
    delete fCollimatorApertureCmd;
    for(G4int index=0; index<64; index++){
        delete fCollimatorSingleDisplMeanCmd[index];
    }
    delete fCollimatorRandDisplCmd;
    delete fCollimatorRandDisplMeanCmd;
    delete fCollimatorRandDisplSigmaCmd;
    delete fCollimationEnvelopeMisalignmentCmd;
    delete fCollimationEnvelopeMispositionCmd;
    delete fCollimatorScoringCSPECCmd;
    delete fModulesOnCmd;
    delete fScoringIndexCmd;
    delete fSensitiveIndexCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4ELINP_DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if(command == fCollimatorSetupCmd) {fDetectorTarget->ResetDetectorForSetup(fCollimatorSetupCmd->GetNewIntValue(newValue));}
            
    if(command == fCollimatorApertureCmd) {fDetectorTarget->SetCollimatorAperture(fCollimatorApertureCmd->GetNewDoubleValue(newValue));}
    
    for(G4int index=0; index<64; index++) {
        if(command == fCollimatorSingleDisplMeanCmd[index]){fDetectorTarget->SetCollSingleDisplMean(index, fCollimatorSingleDisplMeanCmd[index]->GetNewDoubleValue(newValue));}
    }
      
    if(command == fCollimatorRandDisplCmd) {fDetectorTarget->SetCollRandDispl(fCollimatorRandDisplCmd->GetNewIntValue(newValue));}
    
    if(command == fCollimatorRandDisplMeanCmd) {fDetectorTarget->SetCollRandDisplMean(fCollimatorRandDisplMeanCmd->GetNewDoubleValue(newValue));}
    
    if(command == fCollimatorRandDisplSigmaCmd) {fDetectorTarget->SetCollRandDisplSigma(fCollimatorRandDisplSigmaCmd->GetNewDoubleValue(newValue));}
    
    if(command == fCollimationEnvelopeMisalignmentCmd) {fDetectorTarget->SetCollimationEnvelopeMisalignment(fCollimationEnvelopeMisalignmentCmd->GetNew3VectorValue(newValue));}
    
    if(command == fCollimationEnvelopeMispositionCmd) {fDetectorTarget->SetCollimationEnvelopeMisposition(fCollimationEnvelopeMispositionCmd->GetNew3VectorValue(newValue));}
    
    if(command == fCollimatorScoringCSPECCmd) {fDetectorTarget->SetCSPEC(fCollimatorScoringCSPECCmd->GetNewIntValue(newValue));}

    if(command == fModulesOnCmd) {fDetectorTarget->SetModules(fModulesOnCmd->GetNewIntValue(newValue));}
    
    if(command == fScoringIndexCmd) {fDetectorTarget->SetScoringIndex(fScoringIndexCmd->GetNewIntValue(newValue));}
    
    if(command == fSensitiveIndexCmd) {fDetectorTarget->SetSensitiveIndex(fSensitiveIndexCmd->GetNewIntValue(newValue));}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4String G4ELINP_DetectorConstructionMessenger::GetCurrentValue(G4UIcommand* command)
{
    G4String cv;
    
    if(command == fCollimatorApertureCmd) {
        cv = fCollimatorApertureCmd->ConvertToString(fDetectorTarget->GetCollimatorAperture(),"mm");
    }
    
    for(G4int index=0; index<64; index++) {
        if(command == fCollimatorSingleDisplMeanCmd[index]){
            cv = fCollimatorSingleDisplMeanCmd[index]->ConvertToString(fDetectorTarget->GetCollSingleDisplMean(index));
        }
    }
    
    if(command == fCollimatorRandDisplCmd) {
        cv = fCollimatorRandDisplCmd->ConvertToString(fDetectorTarget->GetCollRandDispl());
    }
    
    if(command == fCollimationEnvelopeMisalignmentCmd) {
        cv = fCollimationEnvelopeMisalignmentCmd->ConvertToString(fDetectorTarget->GetCollimationEnvelopeMisalignment(),"deg");
    }
    
    if(command == fCollimationEnvelopeMispositionCmd) {
        cv = fCollimationEnvelopeMispositionCmd->ConvertToString(fDetectorTarget->GetCollimationEnvelopeMisposition(),"mm");
    }

    return cv;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

