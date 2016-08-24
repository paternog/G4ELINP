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
// $Id: PrimaryGeneratorAction.hh,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4ELINP_FileReader.hh"
#include "G4ELINP_PrimaryGeneratorActionMessenger.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;
class G4ELINP_DetectorConstruction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4ELINP_PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    G4ELINP_PrimaryGeneratorAction();
    virtual ~G4ELINP_PrimaryGeneratorAction();
    
    void GeneratePrimaries(G4Event*);   
    void ReadFromFile(G4bool vBool) {fReadFromFile=vBool;};   
    void SetFileName(G4String);
    
private:
    G4ELINP_PrimaryGeneratorActionMessenger* fMessenger;

    G4GeneralParticleSource* fParticleGPS;
    G4ParticleGun* fParticleGun;
    
    G4bool fReadFromFile;
    G4String fFileName;
    static G4ELINP_FileReader* fFileReader;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

