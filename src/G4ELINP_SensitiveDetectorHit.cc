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

#include "G4ELINP_SensitiveDetectorHit.hh"

#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifdef G4MULTITHREADED
G4ThreadLocal G4Allocator<G4ELINP_SensitiveDetectorHit>* G4ELINP_SensitiveDetectorHitAllocator = 0;
#else
G4Allocator<G4ELINP_SensitiveDetectorHit> G4ELINP_SensitiveDetectorHitAllocator;
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_SensitiveDetectorHit::G4ELINP_SensitiveDetectorHit()
{
    fTime = 0.;
    fPos = G4ThreeVector(0.,0.,0.);
    fMom = G4ThreeVector(0.,0.,0.);
    fEnergy = 0.;
    fType = -1;
    fInteraction = 1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_SensitiveDetectorHit::~G4ELINP_SensitiveDetectorHit(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

G4ELINP_SensitiveDetectorHit::G4ELINP_SensitiveDetectorHit(const G4ELINP_SensitiveDetectorHit &right):G4VHit()
{
    fTrackID = right.fTrackID;
    fTrackIDP = right.fTrackIDP;
    fPos = right.fPos;
    fMom = right.fMom;
    fTime = right.fTime;
    fEnergy = right.fEnergy;
    fType = right.fType;
    fA = right.fA;
    fZ = right.fZ;
    fInteraction = right.fInteraction;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

const G4ELINP_SensitiveDetectorHit& G4ELINP_SensitiveDetectorHit::operator=(const G4ELINP_SensitiveDetectorHit &right)
{
    fTrackID = right.fTrackID;
    fTrackIDP = right.fTrackIDP;
    fPos = right.fPos;
    fMom = right.fMom;
    fTime = right.fTime;
    fEnergy = right.fEnergy;
    fType = right.fType;
    fA = right.fA;
    fZ = right.fZ;
    fInteraction = right.fInteraction;
    
    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int G4ELINP_SensitiveDetectorHit::operator==(const G4ELINP_SensitiveDetectorHit &/*right*/) const
{
    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4ELINP_SensitiveDetectorHit::Draw()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager)
    {
        G4Circle circle(fPos);
        circle.SetScreenSize(2);
        circle.SetFillStyle(G4Circle::filled);
        G4Colour colour(1.,1.,0.);
        G4VisAttributes attribs(colour);
        circle.SetVisAttributes(attribs);
        pVVisManager->Draw(circle);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

const std::map<G4String,G4AttDef>* G4ELINP_SensitiveDetectorHit::GetAttDefs() const
{
    G4bool isNew;
    std::map<G4String,G4AttDef>* store = G4AttDefStore::GetInstance("G4ELINP_SensitiveDetectorHit",isNew);
    if (isNew) {
        G4String ID("ID");
        (*store)[ID] = G4AttDef(ID,"ID","Physics","","G4int");
        
        G4String IDP("IDP");
        (*store)[IDP] = G4AttDef(fTrackIDP,"IDP","Physics","","G4int");
        
        G4String Time("t");
        (*store)[Time] = G4AttDef(Time,"Time","Physics","G4BestUnit","G4double");
        
        G4String Pos("pos");
        (*store)[Pos] = G4AttDef(Pos, "Position","Physics","G4BestUnit","G4ThreeVector");

        G4String Mom("mom");
        (*store)[Mom] = G4AttDef(Pos, "Position","Physics","G4BestUnit","G4ThreeVector");

        G4String En("en");
        (*store)[En] = G4AttDef(En, "Energy","Physics","G4BestUnit","G4double");
 
        G4String A("A");
        (*store)[A] = G4AttDef(A,"A","Physics","","G4int");
        
        G4String Z("Z");
        (*store)[Z] = G4AttDef(Z,"Z","Physics","","G4int");
        
    }
    return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

std::vector<G4AttValue>* G4ELINP_SensitiveDetectorHit::CreateAttValues() const
{
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    
    values->push_back(G4AttValue("ID",G4UIcommand::ConvertToString(fTrackID),""));
    
    values->push_back(G4AttValue("IDP",G4UIcommand::ConvertToString(fTrackIDP),""));

    values->push_back(G4AttValue("t",G4BestUnit(fTime,"Time"),""));
    
    values->push_back(G4AttValue("pos",G4BestUnit(fPos,"Length"),""));

    values->push_back(G4AttValue("mom",G4BestUnit(fMom,"Energy"),""));

    values->push_back(G4AttValue("en",G4BestUnit(fEnergy,"Energy"),""));

    values->push_back(G4AttValue("type","",""));
        
    values->push_back(G4AttValue("A",G4UIcommand::ConvertToString(fA),""));
    
    values->push_back(G4AttValue("Z",G4UIcommand::ConvertToString(fZ),""));
    
    return values;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void G4ELINP_SensitiveDetectorHit::Print()
{
    G4cout << " Hit at time " << fTime/ns
    << " (nsec) - pos(x,y,z) " << fPos/mm
    << " (mm) - mom(x,y,z) " << fMom/eV << " eV" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
