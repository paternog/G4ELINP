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

#include "G4ELINP_FileReader.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_FileReader::G4ELINP_FileReader(G4String fileName)
{
    fFileName = fileName;
    inputFile.open(fFileName.data());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_FileReader::~G4ELINP_FileReader()
{
    inputFile.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4ELINP_FileReader::SetFileName(G4String vFileName)
{
    fFileName=vFileName;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4ELINP_FileReader::StoreEvents()
{    
    if(evListTime.size() == 0){       
        G4double genname,k;
        G4double weight = 0.;
        G4double energy = 0.;
        G4double time = 0.;
        G4double px = 0.;
        G4double py = 0.;
        G4double pz = 0.;
        G4double mx = 0.;
        G4double my = 0.;
        G4double mz = 0.;
        G4double sx = 0.;
        G4double sy = 0.;
        G4double sz = 0.;
        
        while(inputFile.good()){
            if(inputFile.good()) inputFile >> k;
            if(inputFile.good()) inputFile >> genname;
            if(inputFile.good()) inputFile >> weight;
            if(inputFile.good()){
                inputFile >> time;
                evListTime.push_back(time/60.*CLHEP::second);
            }
            if(inputFile.good()) inputFile >> px;
            if(inputFile.good()) inputFile >> py;
            if(inputFile.good()){
            	inputFile >> pz;
                evListPos.push_back(G4ThreeVector(px*CLHEP::meter,py*CLHEP::meter,pz*CLHEP::meter));
            }
            if(inputFile.good()){
                inputFile >> energy;
                evListEnergy.push_back(energy*CLHEP::eV);
            }
            if(inputFile.good()) inputFile >> mx;
            if(inputFile.good()) inputFile >> my;
            if(inputFile.good()){
                inputFile >> mz;
                evListMom.push_back(G4ThreeVector(mx*CLHEP::eV,my*CLHEP::eV,mz*CLHEP::eV));               
            }
            if(inputFile.good()) inputFile >> sx;
            if(inputFile.good()) inputFile >> sy;
            if(inputFile.good()){
                inputFile >> sz;
                evListSpin.push_back(G4ThreeVector(sx,sy,sz));
            }
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ThreeVector G4ELINP_FileReader::GetAnEventPosition(G4int vIndex)
{
    return evListPos.at(vIndex);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ThreeVector G4ELINP_FileReader::GetAnEventMomentum(G4int vIndex)
{
    return evListMom.at(vIndex);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ThreeVector G4ELINP_FileReader::GetAnEventSpin(G4int vIndex)
{
    return evListSpin.at(vIndex);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4ELINP_FileReader::GetAnEventTime(G4int vIndex)
{
    return evListTime.at(vIndex);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4double G4ELINP_FileReader::GetAnEventEnergy(G4int vIndex)
{
    return evListEnergy.at(vIndex);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4int G4ELINP_FileReader::GetNumberOfEvents()
{
    return evListPos.size();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
