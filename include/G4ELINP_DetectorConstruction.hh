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
//=================================================================================
//release 2: Gian, june 2016 (see README.txt)
//=================================================================================
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef G4ELINP_DetectorConstruction_h
#define G4ELINP_DetectorConstruction_h 1
#endif

#include "G4VUserDetectorConstruction.hh"
#include "G4ELINP_DetectorConstructionMessenger.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RunManager.hh"

#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4ELINP_DetectorConstruction : public G4VUserDetectorConstruction
{
public:   
    G4ELINP_DetectorConstruction();
    ~G4ELINP_DetectorConstruction();
    
    void DefineMaterials();   
    G4VPhysicalVolume* Construct();
    void ResetDetectorForSetup(int = 0);
    
    G4LogicalVolume* GetScoringVolume() const {return fScoringVolume;}		//Gian: introduced to improve information saved in SeedsAndDose_summary.dat
	G4LogicalVolume* GetSensitiveVolume() const {return fSensitiveVolume;}	//Gian: introduced to be used in SteppingAction class
	G4int GetScoringCSPEC() const {return bScoringCSPEC;}                   //Gian: introduced as a check for StopAndKill in SensitiveDetector
	
protected:
    G4LogicalVolume* fScoringVolume;	//Gian
    G4LogicalVolume* fSensitiveVolume;	//Gian   
    G4int fScoringIndex;            	//Gian
    G4int fSensitiveIndex;            	//Gian
    
private:
    G4ELINP_DetectorConstructionMessenger* fMessenger;
    void ConstructSDandField();
    
    //World
    G4ThreeVector fWorldSize;
    G4Box* fWorldSolid;
    G4LogicalVolume* fWorldLogic;
    G4VPhysicalVolume* fWorldPhysical;
    G4Material* fWorldMaterial;
 
    G4int bLine; 
    
    G4double fCollimatorCenterDistance; //Gian, IMPORTANT: distances depends on this parameter
               
	//--------------------Collimation Chamber (Gian)------------------------------	
  	//Base Plate 
  	G4bool bBasePlate;
  	G4Material* fBasePlateMaterial;
  	G4double fBasePlateOutWidth;
  	G4double fBasePlateOutHeight;
  	G4double fBasePlateOutLength;
  	G4double fBasePlateThickness;
  	G4double fBasePlateBigHoleWidth;
  	G4double fBasePlateBigHoleLength;
  	G4double fBasePlateSmallHoleWidth;
  	G4double fBasePlateSmallHoleLength;
  	G4double fBasePlateBigHoleDistance;  
  	G4double fBasePlate1213Distance;
  	G4LogicalVolume* fBasePlateLogic; 
  	G4VPhysicalVolume* fBasePlatePhysical;
 
 	//Base Plate Windows
  	G4bool bBasePlateWindow;
  	G4Material* fBasePlateWindowMaterial;
  	G4double fBasePlateWindowOutDiameter;
  	G4double fBasePlateWindowInDiameter;
  	G4double fBasePlateWindow1Length;
  	G4double fBasePlateWindow2Length;
  	G4double fBasePlateCenterHeight;
  	G4LogicalVolume* fBasePlateWindow1Logic; 
  	G4LogicalVolume* fBasePlateWindow2Logic; 
  	G4VPhysicalVolume* fBasePlateWindow1Physical;
  	G4VPhysicalVolume* fBasePlateWindow2Physical;
  	
  	//Transversal Beam 
  	G4bool bTransvBeam;
  	G4Material* fTransvBeamMaterial;
  	G4double fTransvBeamDistance;
  	G4double fTransvBeamHeight;
  	G4double fTransvBeamLength;
  	G4LogicalVolume* fTransvBeamLogic; 
  	G4VPhysicalVolume* fTransvBeamPhysical;
  	
  	//Base Plate Envelope
    G4Material* fBasePlateEnvelopeMaterial;
	G4double fBasePlateEnvelopeRadius;
	G4double fBasePlateEnvelopeSemiLength;
	G4LogicalVolume* fBasePlateEnvelopeLogic; 
	G4VPhysicalVolume* fBasePlateEnvelopePhysical; 
  	
    //Chamber and Flanges
    //Chamber
  	G4bool bCollimChamber;
  	G4Material* fCollimChamberMaterial;
  	G4double fCollimChamberOutDiameter;
  	G4double fCollimChamberInDiameter;
  	G4double fCollimChamberLength;
  	//Flanges
  	G4double fFlangeOutOutDiameter;
  	G4double fFlangeOutInDiameter;
  	G4double fFlangeInOutDiameter;
  	G4double fFlangeInInDiameter;
  	G4double fFlangeLength;	
  	//Union Volume (Chamber + Flanges)
  	G4LogicalVolume* fCollimChamberLogic; 
  	G4VPhysicalVolume* fCollimChamberPhysical;
  	
  	//Pump
   	G4bool bCollimChamberPump;
  	G4Material* fCollimChamberPumpMaterial;
  	G4double fCollimChamberPumpDiameter;
  	G4double fCollimChamberPumpHeight;
  	G4double fCollimChamberPumpDistance;
  	G4LogicalVolume* fCollimChamberPumpLogic; 
  	G4VPhysicalVolume* fCollimChamberPumpPhysical;
  	
  	//EnterWindow (Nicola)
	G4bool bEnterWindow;
	G4Material* fEnterWindowMaterial;
	G4double fEnterWindowRadius;
	G4double fEnterWindowLength;
	G4LogicalVolume* fEnterWindowLogic;
	G4VPhysicalVolume* fEnterWindowPhysical;
  	
  	//Feet
   	G4bool bCollimChamberFoot;
  	G4Material* fCollimChamberFootMaterial;
  	G4double fCollimChamberFootLength;
  	G4double fCollimChamberFootHeight;
  	G4double fCollimChamberFootWidth;
  	G4double fCollimChamberFoot1Distance;
  	G4double fCollimChamberFootStep;
  	G4LogicalVolume* fCollimChamberFootLogic; 
  	G4VPhysicalVolume* fCollimChamberFootPhysical;
  	
  	//SpaceFab Plates
  	G4bool bSpaceFabPlates;
  	G4Material* fSpaceFabPlatesMaterial;
  	
  	G4double fSpaceFabUpperPlateWidth;
  	G4double fSpaceFabUpperPlateHeight;
  	G4double fSpaceFabUpperPlateLength;
  	G4LogicalVolume* fSpaceFabUpperPlateLogic; 
  	G4VPhysicalVolume* fSpaceFabUpperPlatePhysical;
  	
  	G4double fSpaceFabLowerPlateWidth;
  	G4double fSpaceFabLowerPlateHeight;
  	G4double fSpaceFabLowerPlateLength;
  	G4double fSpaceFabLowerPlateTickness;
  	G4LogicalVolume* fSpaceFabLowerPlateLogic; 
  	G4VPhysicalVolume* fSpaceFabLowerPlatePhysical;
  	  	  	
  	//Collimation Envolope
	G4Material* fCollimationEnvelopeMaterial;
	G4double fCollimationEnvelopeWidth;
	G4double fCollimationEnvelopeHeight;
	G4double fCollimationEnvelopeLength;
	G4LogicalVolume* fCollimationEnvelopeLogic; 
	G4VPhysicalVolume* fCollimationEnvelopePhysical; 

    G4ThreeVector fCollimationEnvelopeMisalignment;
    G4ThreeVector fCollimationEnvelopeMisposition;	
	//--------------------------------------------------------------------------
			
	// ----------------------Collimator (Nicola)--------------------------------	
	G4bool bCollimator;

	//Collimator Support
	G4Material* fCollimatorSupportMaterial;
	G4double fCollimatorSupportWidth;
	G4double fCollimatorSupportHeight;
	G4double fCollimatorSupportLength;
	G4double fCollimatorSupportThickness;
	G4double fCollimatorSupportEnvelopeHeight;
	G4double fBeamSlitShift;
	G4LogicalVolume* fCollimatorSupportEnvelopeLogic[64];
	G4VPhysicalVolume* fCollimatorSupportEnvelopePhysical;
	G4LogicalVolume* fCollimatorSupportLogic;
	G4VPhysicalVolume* fCollimatorSupportPhysical;

	//Collimator Apparatus
	//Collimator
	G4Material* fCollimatorMaterial;
	G4Material* fCollimatorBlockBaseMaterial;
	G4double fCollimatorBlockBaseWidth;
	G4double fCollimatorBlockBaseHeight;
	G4double fCollimatorBlockBaseLength;
	G4double fCollimatorDistanceAperture;
	
	G4int bCollimatorRelativeRandomDisplacement;
	G4double fCollDisplMean[64];
	G4double fCollDisplSigma[64];
	
	G4double fCollimatorEnvelopeDisplacement;
	G4LogicalVolume* fCollimatorBlockBaseLogic;
	G4VPhysicalVolume* fCollimatorBlockBaseLeftPhysical;
	G4VPhysicalVolume* fCollimatorBlockBaseRightPhysical;
	G4double fCollimatorDisplacement;
	G4double fCollimatorWidth;
	G4double fCollimatorHeight;
	G4double fCollimatorLength;
	G4LogicalVolume* fCollimatorLogic[64];
	G4VPhysicalVolume* fCollimatorLeftPhysical;
	G4VPhysicalVolume* fCollimatorRightPhysical;

	//Collimator Block
	G4Material* fCollimatorBlockMaterial;
	G4double fCollimatorBlockWidth;
	G4double fCollimatorBlockHeight;
	G4double fCollimatorBlockLateralThickness;
	G4double fCollimatorBlockSupThickness;
	G4double fCollimatorBlockGridHoleWidth;
	G4double fCollimatorBlockGridHoleHeight;
	G4double fCollimatorBlockGridStep;
	G4double fCollimatorBlockGridHoleInfDistance;
	G4double fCollimatorBlockGridHoleHorizDistance;
	G4double fCollimatorBlockDisplacement;
	G4double fCollimatorBlockInfDistance;  
	G4LogicalVolume* fCollimatorBlockLogic;
	G4VPhysicalVolume* fCollimatorBlockLeftPhysical;
	G4VPhysicalVolume* fCollimatorBlockRightPhysical;

	//Peak
	G4Material* fCollimatorPeakMaterial;
	G4double fCollimatorPeakHeight;
	G4double fCollimatorPeakLength;
	G4LogicalVolume* fCollimatorPeakLogic;
	G4VPhysicalVolume* fCollimatorPeakLeftPhysical;
	G4VPhysicalVolume* fCollimatorPeakRightPhysical;

	//Collimator Gear
	G4Material* fCollimatorGearMaterial;
	G4double fCollimatorGearThickness;
	G4double fCollimatorGearRadius;
	G4LogicalVolume* fCollimatorGearLogic;
	G4VPhysicalVolume* fCollimatorGearUpPhysical;
	G4VPhysicalVolume* fCollimatorGearDownPhysical;

	//Screw
	G4Material* fCollimatorScrewMaterial;
	G4double fCollimatorScrewRadius;
	G4double fCollimatorScrewUpHeight;
	G4double fCollimatorScrewDownHeight;
	G4LogicalVolume* fCollimatorScrewLogic;
	G4VPhysicalVolume* fCollimatorScrewUpPhysical;
	G4VPhysicalVolume* fCollimatorScrewDownPhysical;	
	//--------------------------------------------------------------------------------------
	
	//-----------------PedestalCollimationChamber and SpaceFab (Nicola)---------------------
	G4bool bPedestalCollimationChamber;
    
    //Plate1
	G4double fPedestalCollimationChamberPlate1Width;
	G4double fPedestalCollimationChamberPlate1Height;
	G4double fPedestalCollimationChamberPlate1Length;
	G4Material* fPedestalCollimationChamberPlate1Material;
	G4LogicalVolume* fPedestalCollimationChamberPlate1Logic;
	G4VPhysicalVolume* fPedestalCollimationChamberPlate1LeftPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberPlate1RightPhysical;	
	
	//Plate2
	G4double fPedestalCollimationChamberPlate2Width;
	G4double fPedestalCollimationChamberPlate2Height;
	G4double fPedestalCollimationChamberPlate2Length;
 	G4Material* fPedestalCollimationChamberPlate2Material;
	G4LogicalVolume* fPedestalCollimationChamberPlate2Logic;
	G4VPhysicalVolume* fPedestalCollimationChamberPlate2LeftPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberPlate2RightPhysical;
	
	//Plate3
	G4double fPedestalCollimationChamberPlate3Width;
	G4double fPedestalCollimationChamberPlate3Height;
	G4double fPedestalCollimationChamberPlate3Length;
 	G4Material* fPedestalCollimationChamberPlate3Material;
	G4LogicalVolume* fPedestalCollimationChamberPlate3Logic;
	G4VPhysicalVolume* fPedestalCollimationChamberPlate3LeftPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberPlate3RightPhysical;
	 
	//PedestalCollimationChamberEnvelope
	G4double fPedestalCollimationChamberEnvelopeWidth;
	G4double fPedestalCollimationChamberEnvelopeHeight;
	G4double fPedestalCollimationChamberDisplacement;
	G4Material* fPedestalCollimationChamberEnvelopeMaterial;
	G4double fPedestalCollimationChamberEnvelopeLength;
	G4LogicalVolume* fPedestalCollimationChamberEnvelopeLogic;
	G4VPhysicalVolume* fPedestalCollimationChamberEnvelopePhysical;
	
	//TransversalGirder	   
    G4double fPedestalCollimationChamberTransversalGirder1Width;
    G4double fPedestalCollimationChamberTransversalGirder1Height;
    G4double fPedestalCollimationChamberTransversalGirder1Length;
    G4double fPedestalCollimationChamberTransversalGirder2Width;
    G4double fPedestalCollimationChamberTransversalGirder2Height;
    G4double fPedestalCollimationChamberTransversalGirder2Length;
    G4double fPedestalCollimationChamberTransversalGirder3Width;
    G4double fPedestalCollimationChamberTransversalGirder3Height;
    G4double fPedestalCollimationChamberTransversalGirder3Length;
    G4double fPedestalCollimationChamberTransversalGirder4Width;
    G4double fPedestalCollimationChamberTransversalGirder4Height;
    G4double fPedestalCollimationChamberTransversalGirder4Length;
	G4double fPedestalCollimationChamberTransversalGirderShiftX;
    G4Material* fPedestalCollimationChamberTransversalGirderMaterial;
	G4LogicalVolume* fPedestalCollimationChamberTransversalGirderLogic;
	G4VPhysicalVolume* fPedestalCollimationChamberTransversalGirderFrontPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberTransversalGirderRearPhysical;
	
	//LongitudinalGirder
    G4double fPedestalCollimationChamberLongitudinalGirder1Width;
    G4double fPedestalCollimationChamberLongitudinalGirder1Height;
    G4double fPedestalCollimationChamberLongitudinalGirder1Length;
    G4double fPedestalCollimationChamberLongitudinalGirder2Width;
    G4double fPedestalCollimationChamberLongitudinalGirder2Height;
    G4double fPedestalCollimationChamberLongitudinalGirder2Length;
    G4double fPedestalCollimationChamberLongitudinalGirder3Width;
    G4double fPedestalCollimationChamberLongitudinalGirder3Height;
    G4double fPedestalCollimationChamberLongitudinalGirder3Length;
    G4double fPedestalCollimationChamberLongitudinalGirder4Width;
    G4double fPedestalCollimationChamberLongitudinalGirder4Height;
    G4double fPedestalCollimationChamberLongitudinalGirder4Length;
    G4Material* fPedestalCollimationChamberLongitudinalGirderMaterial;
	G4LogicalVolume* fPedestalCollimationChamberLongitudinalGirderLogic;
	G4VPhysicalVolume* fPedestalCollimationChamberLongitudinalGirderLeftPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberLongitudinalGirderCentralPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberLongitudinalGirderRightPhysical;
	
	//Strut
	G4double fPedestalCollimationChamberStrutWidth;
	G4double fPedestalCollimationChamberStrutHeight;
	G4double fPedestalCollimationChamberStrutLength;  
    G4double fPedestalCollimationChamberStrutShiftZ;
    G4double fPedestalCollimationChamberStrutExternalHoleWidth;
    G4double fPedestalCollimationChamberStrutExternalHoleHeight;
    G4double fPedestalCollimationChamberStrutExternalHoleLength;
    G4double fPedestalCollimationChamberStrutInternalHoleWidth;
    G4double fPedestalCollimationChamberStrutInternalHoleHeight;
    G4double fPedestalCollimationChamberStrutInternalHoleLength;
    G4double fPedestalCollimationChamberStrutInternalHoleShiftY;
	G4Material* fPedestalCollimationChamberStrutMaterial;
	G4LogicalVolume* fPedestalCollimationChamberStrutLogic;
	G4VPhysicalVolume* fPedestalCollimationChamberStrutLeftFrontPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberStrutLeftRearPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberStrutRightFrontPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberStrutRightRearPhysical;
	   
	//ExternalScrew
	G4double fPedestalCollimationChamberExternalScrewRadius;
	G4double fPedestalCollimationChamberExternalScrewHeight;
	G4Material* fPedestalCollimationChamberExternalScrewMaterial;
	G4LogicalVolume* fPedestalCollimationChamberExternalScrewLogic;
	G4VPhysicalVolume* fPedestalCollimationChamberExternalScrewLeftFrontPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberExternalScrewLeftRearPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberExternalScrewRightFrontPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberExternalScrewRightRearPhysical;
	   
	//CentralScrew
	G4double fPedestalCollimationChamberCentralScrewRadius;
	G4double fPedestalCollimationChamberCentralScrewHeight;
	G4Material* fPedestalCollimationChamberCentralScrewMaterial;
	G4LogicalVolume* fPedestalCollimationChamberCentralScrewLogic;
	G4VPhysicalVolume* fPedestalCollimationChamberCentralScrewLeftPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberCentralScrewRightPhysical;
	   
	//Block
	G4double fPedestalCollimationChamberBlockWidth;
	G4double fPedestalCollimationChamberBlockHeight;
	G4double fPedestalCollimationChamberBlockLength;
	G4double fPedestalCollimationChamberBlockThickness;
	G4Material* fPedestalCollimationChamberBlockMaterial;
	G4Material* fPedestalCollimationChamberBlockInsideMaterial;
	G4LogicalVolume* fPedestalCollimationChamberBlockLogic;
	G4LogicalVolume* fPedestalCollimationChamberBlockInsideLogic;
	G4VPhysicalVolume* fPedestalCollimationChamberBlockLeftPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberBlockRightPhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberBlockLeftInsidePhysical;
	G4VPhysicalVolume* fPedestalCollimationChamberBlockRightInsidePhysical;
	
	//Plate
	G4double fSpaceFabPlateWidth;
	G4double fSpaceFabPlateHeight;
	G4double fSpaceFabPlateLength;
	G4double fSpaceFabPlateHoleWidth;
	G4double fSpaceFabPlateHoleHeight;
	G4double fSpaceFabPlateHoleLength;
	G4Material* fSpaceFabPlateMaterial;
	G4LogicalVolume* fSpaceFabPlateLogic;
	G4VPhysicalVolume* fSpaceFabPlatePhysical;
	
	//Binary
	G4double fSpaceFabBinaryWidth;
	G4double fSpaceFabBinaryHeight;
	G4double fSpaceFabBinaryLength;
	G4double fSpaceFabBinaryShiftX;
	G4double fSpaceFabBinaryShiftZ;
	G4Material* fSpaceFabBinaryMaterial;
	G4LogicalVolume* fSpaceFabBinaryLogic;
	G4VPhysicalVolume* fSpaceFabBinaryFrontPhysical;
	G4VPhysicalVolume* fSpaceFabBinaryCentralPhysical;
	G4VPhysicalVolume* fSpaceFabBinaryRearPhysical;
	
	//CartDown
	G4double fSpaceFabCartDownWidth;
	G4double fSpaceFabCartDownHeight;
	G4double fSpaceFabCartDownLength;
	G4double fSpaceFabCartFrontRearShiftX;
	G4double fSpaceFabCartCentralShiftX;
	G4Material* fSpaceFabCartDownMaterial;
	G4LogicalVolume* fSpaceFabCartDownLogic;
	G4VPhysicalVolume* fSpaceFabCartDownFrontPhysical;
	G4VPhysicalVolume* fSpaceFabCartDownCentralPhysical;
	G4VPhysicalVolume* fSpaceFabCartDownRearPhysical;
	
	//CartUp
	G4double fSpaceFabCartUpWidth;
	G4double fSpaceFabCartUpHeight;
	G4double fSpaceFabCartUpLength;
	G4Material* fSpaceFabCartUpMaterial;
	G4LogicalVolume* fSpaceFabCartUpLogic;
	G4VPhysicalVolume* fSpaceFabCartUpFrontPhysical;
	G4VPhysicalVolume* fSpaceFabCartUpCentralPhysical;
	G4VPhysicalVolume* fSpaceFabCartUpRearPhysical;
    //----------------------------------------------------------------------------------

	//----------------------Pedestals M30 M31 (Nicola)----------------------------------	
	G4double fPedestalM30FrontPositionZ;
	G4double fPedestalM30RearPositionZ;
	G4double fPedestalM31FrontPositionZ;
	G4double fPedestalM31RearPositionZ;
	
	//Plate1	
	G4double fPedestalPlate1Width;
	G4double fPedestalPlate1Height;
	G4double fPedestalPlate1Length;
	G4Material* fPedestalPlate1Material;
	G4LogicalVolume* fPedestalPlate1Logic;
	G4VPhysicalVolume* fPedestalPlate1M30FrontPhysical;
	G4VPhysicalVolume* fPedestalPlate1M30RearPhysical;
	G4VPhysicalVolume* fPedestalPlate1M31FrontPhysical;
	G4VPhysicalVolume* fPedestalPlate1M31RearPhysical;
	
	//Plate2	
	G4double fPedestalPlate2Width;
	G4double fPedestalPlate2Height;
	G4double fPedestalPlate2Length;
	G4Material* fPedestalPlate2Material;
	G4LogicalVolume* fPedestalPlate2Logic;
	G4VPhysicalVolume* fPedestalPlate2M30FrontPhysical;
	G4VPhysicalVolume* fPedestalPlate2M30RearPhysical;
	G4VPhysicalVolume* fPedestalPlate2M31FrontPhysical;
	G4VPhysicalVolume* fPedestalPlate2M31RearPhysical;
	
	//Block
	G4double fPedestalBlockWidth;
	G4double fPedestalBlockHeight;
	G4double fPedestalBlockLength;
	G4double fPedestalBlockThickness;	
	G4Material* fPedestalBlockMaterial;
	G4Material* fPedestalBlockInsideMaterial;
	G4LogicalVolume* fPedestalBlockLogic;
	G4LogicalVolume* fPedestalBlockInsideLogic;
	G4VPhysicalVolume* fPedestalBlockM30FrontPhysical;
	G4VPhysicalVolume* fPedestalBlockM30RearPhysical;
	G4VPhysicalVolume* fPedestalBlockM31FrontPhysical;
	G4VPhysicalVolume* fPedestalBlockM31RearPhysical;
	G4VPhysicalVolume* fPedestalBlockM30FrontInsidePhysical;
	G4VPhysicalVolume* fPedestalBlockM30RearInsidePhysical;
	G4VPhysicalVolume* fPedestalBlockM31FrontInsidePhysical;
	G4VPhysicalVolume* fPedestalBlockM31RearInsidePhysical;
	
	//Plate3
	G4double fPedestalPlate3Width;
	G4double fPedestalPlate3Height;
	G4double fPedestalPlate3Length;
	G4Material* fPedestalPlate3Material;
	G4LogicalVolume* fPedestalPlate3Logic;
	G4VPhysicalVolume* fPedestalPlate3M30FrontPhysical;
	G4VPhysicalVolume* fPedestalPlate3M30RearPhysical;
	G4VPhysicalVolume* fPedestalPlate3M31FrontPhysical;
	G4VPhysicalVolume* fPedestalPlate3M31RearPhysical;
	
	//boolean varaible to activate M30 and M31 modules
	G4bool bModulesFeOn;
	//----------------------------------------------------------------------------------
		    
    //Beam pipe
    G4bool bBeamPipe;
    
    G4Material* fBeamPipeMaterial;   
    G4Material* fBeamPipeVacuum;   
    G4Material* fLeadPipeCapMaterial; 
    
    G4double fBeamPipeA0InnerRadius;
    G4double fBeamPipeA0OuterRadius;    
    
    G4double fBeamPipeA0Length;      	//Gian, IMPORTANT: distances depends on this parameter
    G4VSolid* fBeamPipeA0Solid;
    G4LogicalVolume* fBeamPipeA0Logic;
    G4VPhysicalVolume* fBeamPipeA0Physical;
    G4VPhysicalVolume* fBeamPipeA0InsidePhysical;
    
    G4double fBeamPipeA1Length;         //Gian, IMPORTANT: distances depends on this parameter
    G4VSolid* fBeamPipeA1Solid;
    G4LogicalVolume* fBeamPipeA1Logic;
    G4VPhysicalVolume* fBeamPipeA1Physical;
    G4VPhysicalVolume* fBeamPipeA1InsidePhysical;
       
    //EndLine pipe (Gian)
    G4double fBeamPipeA3Length;         
    G4VSolid* fBeamPipeA3Solid;
    G4LogicalVolume* fBeamPipeA3Logic;
    G4VPhysicalVolume* fBeamPipeA3Physical;
    G4VPhysicalVolume* fBeamPipeA3InsidePhysical;
    
    //EndLine Window (Gian)
	G4bool bA3Window;
	G4Material* fA3WindowMaterial;
	G4double fA3WindowLength;
	G4LogicalVolume* fA3WindowLogic;
	G4VPhysicalVolume* fA3WindowPhysical;

    //Concrete
    G4bool bConcrete;
    G4bool bA1ConcreteDetail;       
    G4Material *fConcreteMaterial;
    
    G4double fA1LeadPipeCapWidth;
	G4double fA1LeadPipeCapHeight;
	G4double fA1LeadPipeCapLength;
    
    G4double fConcreteA0Distance;
    G4double fConcreteA0RadiusPipe;

	G4double fConcreteA1Distance;
    G4double fConcreteA1Length;
    G4double fConcreteA1Width;
    G4double fConcreteA1Height;
    G4double fConcreteA1RadiusPipe;
 
    G4LogicalVolume* fConcreteFloorLogic; 
    G4LogicalVolume* fConcreteTopLogic;
    G4LogicalVolume* fConcreteRightWallLogic;
    G4LogicalVolume* fConcreteLeftWallLogic;
    G4LogicalVolume* fA1LeadPipeCapLogic;
    
    G4VPhysicalVolume* fConcreteFloorPhysical;
    G4VPhysicalVolume* fConcreteTopPhysical;
    G4VPhysicalVolume* fConcreteRightWallPhysical;
    G4VPhysicalVolume* fConcreteLeftWallPhysical;
    G4VPhysicalVolume* fA1LeadPipeCapPhysical;
        
    G4LogicalVolume* fConcreteA0Logic;
    G4VPhysicalVolume* fConcreteA0Physical;
    
    G4LogicalVolume* fConcreteA1Logic;
    G4VPhysicalVolume* fConcreteA1Physical;
    
    G4double fCollimChamberConcreteA1Distance; //Gian added
    G4double fConcreteA1CSPECDistance;		   //Gian added
    
    //G4LogicalVolume* fConcreteA2Logic; 	//EDIT: A2 replaced with A0 to make hole in the last wall
    G4VPhysicalVolume* fConcreteA2Physical;
       
    //Walls 
   	G4double fRoomWidth; 
	G4double fRoomLength;
	G4double fRoomHeight;

	G4double fBeamHeight;     //beamline Height from floor
	G4double fRoomShiftX;     //shift on x-axis of the room center with respect to beam direction axis
 	G4double fRoomShiftY;     //shift on y-axis of the room center with respect to beam direction axis

	G4double fFloorThickness;
	G4double fTopThickness;   //ceiling thickness
	G4double fWallThickness;
	G4double fWallThickness2; //only for HE line, left wall has different thickness
	G4double fWallHeight;     //height of side walls

	//Girders and Pedestals
	G4bool bModulesOn;

    G4double fGirderHeight;
    G4double fGirderWidth;
    G4double fGirderThickness;
    G4double fGirderJointLength;
    G4double fPedestalHeight;
    G4double fPedestalWidth;
    G4double fPedestalLength;
    G4double fPedestalThickness;
    G4double fGirderY;   
    G4double fGirderM31Y;        
    G4double fPedestalY;
    G4double fPedestalM31Y;
    G4double fM27AGirderLength;
    G4double fM31GirderLength;
    
    G4double fM27ADistance;
    G4double fM30Distance;  
    G4double fM31Distance;
    G4double fM32Distance;
    G4double fM33Distance;
    G4double fM34Distance;
    
    G4double fM27AMagnetWidth;
    G4double fM27AMagnetHeight;
    G4double fM27AMagnetLength;
    G4double fM27AMagnetPoleWidth;
    G4double fM27AMagnetPoleHeight;
    G4double fM27AMagnetCoilWidth;
    G4double fM27AMagnetCoilHeight;
	G4double fM27AMagnetCoilLength;
	G4double fM27AMagnetCoilThickness;
	
	G4double fM34MagnetRadius;
	G4double fM34MagnetInnerRadius;
    G4double fM34MagnetOuterRadius;
    G4double fM34MagnetHeight;
    G4double fM34MagnetThetaStart;
    G4double fM34MagnetThetaEnd;
    G4double fM34MagnetCoilHeight;
    G4double fM34MagnetCoilThickness; 
	G4double fM34MagnetCoilThetaStart;
    G4double fM34MagnetCoilThetaEnd;
    G4double fM34MagnetCoilHoleThetaStart;
    G4double fM34MagnetCoilHoleThetaEnd;
   
    G4double fThetaElectron;
    G4double fThetaM34;
    
    G4double fM32GirderLength;
    G4double fM32X;    
    G4double fM33GirderLength;
    G4double fM33X;
    G4double fM34GirderLength;
    G4double fM34X; 

    G4Material* fGirderMaterial;
    G4Material* fPedestalMaterial;
    G4Material* fPedestalInsideMaterial;
    G4Material* fMagnetMaterial;
    G4Material* fCoilMaterial;
   
	G4LogicalVolume* fGirderJointLogic;
   	G4LogicalVolume* fPedestalLogic;
   	G4LogicalVolume* fPedestalM31Logic;
   	G4LogicalVolume* fPedestalInsideLogic;   	
   	G4LogicalVolume* fPedestalM30InsideLogic;
   	G4LogicalVolume* fPedestalM31InsideLogic;   	
    G4LogicalVolume* fM31GirderLogic;
    G4LogicalVolume* fM27AGirderLogic;
    G4LogicalVolume* fM32GirderLogic;
    G4LogicalVolume* fM32Logic;        
    G4LogicalVolume* fM33GirderLogic;
    G4LogicalVolume* fM33Logic;
    G4LogicalVolume* fM34GirderLogic;
    G4LogicalVolume* fM34Logic;
    G4LogicalVolume* fM27AMagnetLogic;    
    G4LogicalVolume* fM27AMagnetBaseLogic; 
    G4LogicalVolume* fM27AMagnetCoilLogic;         
    G4LogicalVolume* fM34MagnetLogic;
    G4LogicalVolume* fM34MagnetCoilLogic;
        
    G4VPhysicalVolume* fM31LGirderPhysical;
    G4VPhysicalVolume* fM31RGirderPhysical;
    G4VPhysicalVolume* fM27ALGirderPhysical;
    G4VPhysicalVolume* fM27ARGirderPhysical;
    G4VPhysicalVolume* fM32LGirderPhysical;
    G4VPhysicalVolume* fM32RGirderPhysical;
    G4VPhysicalVolume* fM33LGirderPhysical;
    G4VPhysicalVolume* fM33RGirderPhysical;
    G4VPhysicalVolume* fM34LGirderPhysical;
    G4VPhysicalVolume* fM34RGirderPhysical;                     
    G4VPhysicalVolume* fM31Joint1Physical;
    G4VPhysicalVolume* fM31Joint2Physical;
    G4VPhysicalVolume* fM27AJoint1Physical;
    G4VPhysicalVolume* fM27AJoint2Physical;
    G4VPhysicalVolume* fM32Joint1Physical;
    G4VPhysicalVolume* fM32Joint2Physical;
    G4VPhysicalVolume* fM33Joint1Physical;
    G4VPhysicalVolume* fM33Joint2Physical;   
    G4VPhysicalVolume* fM34Joint1Physical;
    G4VPhysicalVolume* fM34Joint2Physical;                          
    G4VPhysicalVolume* fM27APedestal1Physical;
    G4VPhysicalVolume* fM27APedestal1InsidePhysical;
    G4VPhysicalVolume* fM27APedestal2Physical; 
    G4VPhysicalVolume* fM27APedestal2InsidePhysical;
    G4VPhysicalVolume* fM32Pedestal1Physical;
    G4VPhysicalVolume* fM32Pedestal1InsidePhysical; 
    G4VPhysicalVolume* fM32Pedestal2Physical; 
    G4VPhysicalVolume* fM32Pedestal2InsidePhysical;  
    G4VPhysicalVolume* fM32Physical;        
    G4VPhysicalVolume* fM33Pedestal1Physical;
    G4VPhysicalVolume* fM33Pedestal1InsidePhysical; 
    G4VPhysicalVolume* fM33Pedestal2Physical; 
    G4VPhysicalVolume* fM33Pedestal2InsidePhysical;  
    G4VPhysicalVolume* fM33Physical;
    G4VPhysicalVolume* fM34Pedestal1Physical;
    G4VPhysicalVolume* fM34Pedestal1InsidePhysical; 
    G4VPhysicalVolume* fM34Pedestal2Physical; 
    G4VPhysicalVolume* fM34Pedestal2InsidePhysical;  
    G4VPhysicalVolume* fM34Physical;        
    G4VPhysicalVolume* fM27AMagnetPhysical;
    G4VPhysicalVolume* fM27AMagnetBasePhysical;
    G4VPhysicalVolume* fM27AMagnetCoilTopPhysical;
    G4VPhysicalVolume* fM27AMagnetCoilBottomPhysical;       
    G4VPhysicalVolume* fM34MagnetPhysical;
    G4VPhysicalVolume* fM34MagnetCoilTopPhysical;
    G4VPhysicalVolume* fM34MagnetCoilBottomPhysical;  
    
    //CSPEC
	G4int bScoringCSPEC;
	
    G4double fCSPECInnerRadius;
    G4double fCSPECOuterRadius;
    G4double fCSPECPbInnerRadius;  		
    G4double fCSPECPbOuterRadius;       
    G4double fCSPECSheetInnerRadius;    
    G4double fCSPECSheetOuterRadius;    
    G4double fCSPECLength;
    G4double fCSPECWindowLength;       
         
    G4LogicalVolume* fCSPECLogic;
    G4LogicalVolume* fCSPECPbLogic;     //Gian added
    G4LogicalVolume* fCSPECSheetLogic;  //Gian added
    G4LogicalVolume* fCSPECWindowLogic; 
    G4LogicalVolume* fTransparentCSPECLogic;
                        
    G4VPhysicalVolume* fCSPECPhysical;
    G4VPhysicalVolume* fCSPECPbPhysical;
    G4VPhysicalVolume* fCSPECSheetPhysical;
    G4VPhysicalVolume* fCSPECWindow0Physical;
    G4VPhysicalVolume* fCSPECWindow1Physical;
    G4VPhysicalVolume* fTransparentCSPECPhysical;

    //Transparent Detectors          
    G4bool bTransparentDetector;
  
    G4double fTransparentDetectorWidth;
    G4double fTransparentDetectorHeight;
    G4double fTransparentDetectorLength;
    
    G4LogicalVolume* fTransparentDetectorLogic;
    G4VPhysicalVolume* fTransparentDetectorPhysical;
    
    G4LogicalVolume* fTransparentDetectorBoxLogic;       
    G4VPhysicalVolume* fTransparentDetectorBoxPhysical;

    //detectors for Rack dose evalaution 
	G4double fRackWidth;
	G4double fRackLength;
	G4double fRack1Height;
	G4double fRack2Height;
	
	G4double fRack1PositionX;
    G4double fRack1PositionY;
    G4double fRack1PositionZ;
    G4LogicalVolume* fTransparentDetectorRack1Logic;
    G4VPhysicalVolume* fTransparentDetectorRack1Physical;
        
    G4double fRack2PositionX;
    G4double fRack2PositionY;
    G4double fRack2PositionZ;
   	G4LogicalVolume* fTransparentDetectorRack2Logic;	
	G4VPhysicalVolume* fTransparentDetectorRack2Physical;
    
    //detectors for Hexapod dose evalaution 
	G4double fHexapodWidth;
	G4double fHexapodLength;
	G4double fHexapodHeight;	
    G4double fHexapodPositionX;
    G4double fHexapodPositionY;   
    G4double fHexapodPositionZ;        
    G4LogicalVolume* fTransparentDetectorHexapodLogic;
	G4VPhysicalVolume* fTransparentDetectorHexapodPhysical;    
	
	//----------------------------------------set methods used by the messenger------------------------------------------------    
public:
	//collimator nominal aperture
    void SetCollimatorAperture(G4double vAperture) {fCollimatorDistanceAperture = vAperture;}
    G4double GetCollimatorAperture() {return fCollimatorDistanceAperture;}
    
    //displacement with respect to the nominal aperture, manually
    void SetCollSingleDisplMean(G4int index, G4double vDistance) {fCollDisplMean[index] = vDistance;}
    G4double GetCollSingleDisplMean(G4int index) {return fCollDisplMean[index];}
    
    //displacement with respect to the nominal aperture, randomly
    void SetCollRandDispl(G4int vNumber) {bCollimatorRelativeRandomDisplacement = vNumber;}
    G4int GetCollRandDispl() {return bCollimatorRelativeRandomDisplacement;}

	//set the same mean and signa for all the collimators 
    void SetCollRandDisplMean(G4double vDistance) {for(int index=0; index<64; index++) {fCollDisplMean[index] = vDistance;}}         
    void SetCollRandDisplSigma(G4double vAperture) {for(int index=0; index<64; index++) {fCollDisplSigma[index] = vAperture;}} 
    
    //set mean and signa indipendently for each collimator (not used and not implemented in the Messenger)
    void SetCollRandDisplMean(G4int index, G4double vDistance) {fCollDisplMean[index] = vDistance;} 	
    G4double GetCollRandDisplMean(G4int index) {return fCollDisplMean[index];}
    void SetCollRandDisplSigma(G4int index, G4double vAperture) {fCollDisplSigma[index] = vAperture;}
    G4double GetCollRandDisplSigma(G4int index) {return fCollDisplSigma[index];}
    
    //collimation chamber misposition/misaligment
    void SetCollimationEnvelopeMisalignment(G4ThreeVector vMisalignment) {fCollimationEnvelopeMisalignment = vMisalignment;}
    G4ThreeVector GetCollimationEnvelopeMisalignment() {return fCollimationEnvelopeMisalignment;}
    void SetCollimationEnvelopeMisposition(G4ThreeVector vMisposition) {fCollimationEnvelopeMisposition = vMisposition;}
    G4ThreeVector GetCollimationEnvelopeMisposition() {return fCollimationEnvelopeMisposition;}
    
    //transparent detector
    void SetCSPEC(int aInt) {bScoringCSPEC = aInt;}
     
    //girders and pedestals
	void SetModules(G4int var) {bModulesOn = var;}  
	
	//index for select the Dose Scoring Volume
	void SetScoringIndex(G4int scoringIndex) {fScoringIndex = scoringIndex;}
	G4int GetScoringIndex() const {return fScoringIndex;}
	
	//index for select the Sensitive Volumes
	void SetSensitiveIndex(G4int sensitiveIndex) {fSensitiveIndex = sensitiveIndex;}
	G4int GetSensitiveIndex() const {return fSensitiveIndex;}
    //---------------------------------------------------------------------------------------------------------------------------	

};
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
