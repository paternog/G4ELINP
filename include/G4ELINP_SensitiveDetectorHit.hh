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

#ifndef G4ELINP_SensitiveDetectorHit_h
#define G4ELINP_SensitiveDetectorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4AttDef;
class G4AttValue;

class G4ELINP_SensitiveDetectorHit : public G4VHit
{
public:
    G4ELINP_SensitiveDetectorHit();
    
    virtual ~G4ELINP_SensitiveDetectorHit();
    
    G4ELINP_SensitiveDetectorHit(const G4ELINP_SensitiveDetectorHit &right);
    const G4ELINP_SensitiveDetectorHit& operator=(const G4ELINP_SensitiveDetectorHit &right);
    
    int operator==(const G4ELINP_SensitiveDetectorHit &right) const;
    
    inline void *operator new(size_t);
    inline void operator delete(void *aHit);
    
    virtual void Draw();
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    virtual void Print();
    
private:
    G4int fTrackID;
    G4int fTrackIDP;
    G4double fTime;
    G4ThreeVector fPos;
    G4ThreeVector fMom;
    G4double fEnergy;
    G4int fType;
    G4int fA;
    G4int fZ;
    G4int fInteraction;
   
public:
    inline void SetTrackID(G4int z) {fTrackID = z;}
    inline G4int GetTrackID() const {return fTrackID;}

    inline void SetTrackIDP(G4int z) {fTrackIDP = z;}
    inline G4int GetTrackIDP() const {return fTrackIDP;}

    inline void SetTime(G4double t) {fTime = t;}
    inline G4double GetTime() const {return fTime;}
    
    inline void SetPos(G4ThreeVector xyz) {fPos = xyz;}
    inline G4ThreeVector GetPos() const {return fPos;}
    
    inline void SetMom(G4ThreeVector xyz) {fMom = xyz;}
    inline G4ThreeVector GetMom() const {return fMom;}
    
    inline void SetEnergy(G4double energy) {fEnergy = energy;}
    inline G4double GetEnergy() const {return fEnergy;}
       
    inline void SetType(G4int type) {fType = type;}
    inline G4int GetType() const {return fType;}
    
    inline void SetA(G4int A) {fA = A;}
    inline G4int GetA() const {return fA;}
    
    inline void SetZ(G4int z) {fZ = z;}
    inline G4int GetZ() const {return fZ;}
    
    inline void SetInteraction(G4int val) {fInteraction = val;}
    inline G4int GetInteraction() const {return fInteraction;}
};

typedef G4THitsCollection<G4ELINP_SensitiveDetectorHit> G4ELINP_SensitiveDetectorHitsCollection;

#ifdef G4MULTITHREADED
extern G4ThreadLocal G4Allocator<G4ELINP_SensitiveDetectorHit>* G4ELINP_SensitiveDetectorHitAllocator;
#else
extern G4Allocator<G4ELINP_SensitiveDetectorHit> G4ELINP_SensitiveDetectorHitAllocator;
#endif

inline void* G4ELINP_SensitiveDetectorHit::operator new(size_t)
{
#ifdef G4MULTITHREADED
    if(!G4ELINP_SensitiveDetectorHitAllocator) G4ELINP_SensitiveDetectorHitAllocator = new G4Allocator<G4ELINP_SensitiveDetectorHit>;
    return (void *) G4ELINP_SensitiveDetectorHitAllocator->MallocSingle();
#else
    void* aHit;
    aHit = (void*)G4ELINP_SensitiveDetectorHitAllocator.MallocSingle();
    return aHit;
#endif
}

inline void G4ELINP_SensitiveDetectorHit::operator delete(void* aHit)
{
#ifdef G4MULTITHREADED
	G4ELINP_SensitiveDetectorHitAllocator->FreeSingle((G4ELINP_SensitiveDetectorHit*) aHit);
#else
   	G4ELINP_SensitiveDetectorHitAllocator.FreeSingle((G4ELINP_SensitiveDetectorHit*) aHit);
#endif
}

#endif
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

