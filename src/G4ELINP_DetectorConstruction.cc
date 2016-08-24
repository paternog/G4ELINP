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

#include "G4ELINP_DetectorConstruction.hh"
#include "G4ELINP_SensitiveDetector.hh"

#include "G4RunManager.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Cons.hh"
#include "G4Polyhedra.hh"
#include "G4Trd.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4AssemblyVolume.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4PSDoseDeposit.hh"
#include "G4PSEnergyDeposit.hh"

#include <math.h>
#include <sstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ELINP_DetectorConstruction::G4ELINP_DetectorConstruction():
fWorldLogic(0)
{
    
    //instantiate the messenger
    fMessenger = new G4ELINP_DetectorConstructionMessenger(this);
    
    //define custom materials
    DefineMaterials();
    
    //World size
    fWorldSize = G4ThreeVector(10000. * CLHEP::cm, 10000. * CLHEP::cm, 10000. * CLHEP::cm);
    
    //scoring screen selection (defualt value, can be changed through a macro)
    bScoringCSPEC = 0;	//0 -> Large scoring screen (place CSPEC), 1 -> CSPEC box scoring, 2 -> large CSPEC box scoring
    
    //initialize scoring and sensitive volumes
    fScoringVolume = 0;
    fSensitiveVolume = 0;
    fScoringIndex = 1;
    fSensitiveIndex = 1;
  
    //Collimation Chamber activation
    bCollimChamber = true;
    bCollimChamberPump = true;
    bCollimChamberFoot = true;
    bSpaceFabPlates = true;
    bBasePlate = true;
    bBasePlateWindow = true;
    bTransvBeam = true;
    bPedestalCollimationChamber = true;
    bCollimator = true; 
    
    //Windows
    bEnterWindow = true;
    bA3Window = true;
      
    //Beam Pipe, Concrete, Transparent Detector and Modules activation
    bBeamPipe = true;
    bConcrete = true;
    bA1ConcreteDetail = true; 
    bModulesFeOn = true;           					//for modules M30 and M31 (Ferrara modules)
    bModulesOn = true;								//for modules M27A, M32, M33, M34 (defualt value, can be changed through a macro)
    bTransparentDetector = true; 

	//select line and set the geometry (defualt value, can be changed through a macro)
	bLine = 1;										//0 -> Low Energy, 1 -> High Energy 
	ResetDetectorForSetup(bLine);	
	
	//set default collimator configuration
	fCollimatorDistanceAperture = 25. * CLHEP::mm;	//Aperture between the two slits (max 25 mm)
	bCollimatorRelativeRandomDisplacement = false;  
	SetCollRandDisplMean(0. * CLHEP::mm);
	SetCollRandDisplSigma(0.001 * CLHEP::mm);
	
	//set default collimation chamber misposition/misaligment
	fCollimationEnvelopeMisalignment = G4ThreeVector(0., 0., 0.); //maximum values (2., 2., 10.)
    fCollimationEnvelopeMisposition = G4ThreeVector(0. * CLHEP::mm, 0. * CLHEP::mm, 0. * CLHEP::mm); //maximum values (20., 20., 20.)

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

G4ELINP_DetectorConstruction::~G4ELINP_DetectorConstruction(){}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ELINP_DetectorConstruction::ResetDetectorForSetup(int line)
{

	bLine=line;
	
	//Collimator center distance
	fCollimatorCenterDistance = 1020.1 * CLHEP::cm;
    if(bLine==1) fCollimatorCenterDistance = 912.7 * CLHEP::cm;

	//--------------------Collimation Chamber (Gian)---------------------------
    //Base Plate
    fBasePlateOutWidth = 148. * CLHEP::mm;
    fBasePlateOutHeight = 41. * CLHEP::mm; //reduced from 48 mm
    fBasePlateOutLength = 733. * CLHEP::mm;
    fBasePlateThickness = 6. * CLHEP::mm;
    fBasePlateBigHoleWidth = 96. * CLHEP::mm;
    fBasePlateBigHoleLength = 180. * CLHEP::mm;
    fBasePlateSmallHoleWidth = 65. * CLHEP::mm; //increased from 55 mm
    fBasePlateSmallHoleLength = 31. * CLHEP::mm;
	fBasePlateBigHoleDistance = 426.5 * CLHEP::mm;
	fBasePlate1213Distance = 240 * CLHEP::mm;
	
	//Base Plate Windows 
	fBasePlateWindowOutDiameter = 195. * CLHEP::mm;
  	fBasePlateWindowInDiameter = 130. * CLHEP::mm;
  	fBasePlateWindow1Length = 30. * CLHEP::mm;
  	fBasePlateWindow2Length = 20. * CLHEP::mm;
  	fBasePlateCenterHeight = 66. * CLHEP::mm;;
  	
  	//Transversal Beam 
  	fTransvBeamDistance = 425. * CLHEP::mm;
  	fTransvBeamHeight = 42. * CLHEP::mm;
  	fTransvBeamLength = 6. * CLHEP::mm;
  	
    //Chamber and Flanges
  	fCollimChamberOutDiameter = 206. * CLHEP::mm;
  	fCollimChamberInDiameter = 200. * CLHEP::mm;
  	fCollimChamberLength = 736. * CLHEP::mm;;	
  	fFlangeOutOutDiameter = 253.2 * CLHEP::mm;
  	fFlangeOutInDiameter = 40. * CLHEP::mm;
  	fFlangeInOutDiameter = 253.2 * CLHEP::mm;
  	fFlangeInInDiameter = 200. * CLHEP::mm;
  	fFlangeLength = 24.5 * CLHEP::mm;
  	
  	//EnterWindow (Nicola)
	fEnterWindowRadius = fFlangeOutInDiameter * 0.5;
	fEnterWindowLength = 15.0 * CLHEP::mm;
	
	//A3 window (Gian)
	fA3WindowLength = 0.150 * CLHEP::mm;

	//Pump
  	fCollimChamberPumpDiameter = 204. * CLHEP::mm;
  	fCollimChamberPumpHeight = 342. * CLHEP::mm;;
  	fCollimChamberPumpDistance = 563. * CLHEP::mm;
  	
  	//Feet
  	fCollimChamberFootLength = 6. * CLHEP::mm;
  	fCollimChamberFootHeight = 97. * CLHEP::mm;
  	fCollimChamberFootWidth = 206. * CLHEP::mm;
  	fCollimChamberFoot1Distance = 165.5 * CLHEP::mm;
  	fCollimChamberFootStep = 124. * CLHEP::mm;  	
  	
  	//SpaceFab Plates
  	fSpaceFabUpperPlateWidth = fCollimChamberFootWidth;
  	fSpaceFabUpperPlateHeight = 8. * CLHEP::mm;;
	fSpaceFabUpperPlateLength = fCollimChamberFootStep * 4. + fCollimChamberFootLength * 5.;	
  	fSpaceFabLowerPlateWidth = fCollimChamberFootWidth;
  	fSpaceFabLowerPlateHeight = 36 * CLHEP::mm;
  	fSpaceFabLowerPlateLength = 459.5 * CLHEP::mm;
  	fSpaceFabLowerPlateTickness = 18 * CLHEP::mm;   
    //------------------------------------------------------------------------
   
	//------------------------------NICOLA------------------------------------
	//Collimator Support (Nicola)
	fCollimatorSupportWidth = 134. * CLHEP::mm;
	fCollimatorSupportHeight = 51.08 * CLHEP::mm; 			 //added contribute from semi cilinder on the the top of the CollimatorSupport's side (=pigreco*radius/4)
	fCollimatorSupportLength = 30. * CLHEP::mm;
	fCollimatorSupportThickness = 8. * CLHEP::mm;
	fCollimatorSupportEnvelopeHeight = 98. * CLHEP::mm;
	
	//Collimator (Nicola)
	fCollimatorBlockBaseWidth = 40. * CLHEP::mm;
	fCollimatorBlockBaseHeight = 38. * CLHEP::mm;
	fCollimatorBlockBaseLength = 20. * CLHEP::mm;
	fCollimatorWidth = 40. * CLHEP::mm;
	fCollimatorHeight = 40. * CLHEP::mm;
	fCollimatorLength = 20. * CLHEP::mm;
	fCollimatorDisplacement = 1.5 * CLHEP::mm; 				 //displacement between Collimator and BlockBase on the inner side

	fCollimatorBlockWidth = 35 * CLHEP::mm;
	fCollimatorBlockHeight = 54 * CLHEP::mm;
	fCollimatorBlockLateralThickness = 3.5 * CLHEP::mm;
	fCollimatorBlockSupThickness = 4. * CLHEP::mm;
	fCollimatorBlockGridHoleWidth = 5 * CLHEP::mm;
	fCollimatorBlockGridHoleHeight = 34.93 * CLHEP::mm; 	 //equivalent volume
	fCollimatorBlockGridStep = 6. * CLHEP::mm;
	fCollimatorBlockGridHoleInfDistance = 12.57 * CLHEP::mm; //consequence of equivalent volume
	fCollimatorBlockGridHoleHorizDistance = 4. * CLHEP::mm;
	fCollimatorBlockDisplacement = 3. * CLHEP::mm; 			 //displacement between Block and BlockBase on the inner side
	fCollimatorBlockInfDistance = fCollimatorBlockBaseHeight - (fCollimatorBlockHeight - fCollimatorBlockSupThickness - fCollimatorBlockGridHoleHeight);  // distance between BlockBase's inferior side and Block inferior side
	fCollimatorPeakHeight = 8. * CLHEP::mm;
	fCollimatorPeakLength = 16.2 * CLHEP::mm;
		
	//Collimator Gear
	fCollimatorGearThickness = 10. * CLHEP::mm;
	fCollimatorGearRadius = 7.85 *CLHEP::mm;

	//Collimator Screw
	fCollimatorScrewRadius = 2. * CLHEP::mm;
	fCollimatorScrewUpHeight = 31.3 * CLHEP::mm;	 		//height without taking into account the thickness of the CollimatorSupport
	fCollimatorScrewDownHeight = 16.6 * CLHEP::mm; 	 		//height without taking into account the thickness of the CollimatorSupport

	//Envelope Displacement
	fCollimatorEnvelopeDisplacement = fCollimatorGearThickness * 0.5; //displacement of the Collimator's simmetry axis with respect to the Envelope simmetry axis
		
	//--------------------------------------------------------------------------------------		   
		
	//-----------------PedestalCollimationChamber and SpaceFab (Nicola)---------------------	
	//Plate1
	fPedestalCollimationChamberPlate1Width = 700. * CLHEP::mm;  
	fPedestalCollimationChamberPlate1Height = 25. * CLHEP::mm;
	fPedestalCollimationChamberPlate1Length = 700.  * CLHEP::mm;
	
	//Plate2
	fPedestalCollimationChamberPlate2Width = 630. * CLHEP::mm;
	fPedestalCollimationChamberPlate2Height = 30. * CLHEP::mm;
	fPedestalCollimationChamberPlate2Length = 630. * CLHEP::mm;
	
	//Plate3
	fPedestalCollimationChamberPlate3Width = 480. * CLHEP::mm; 
	fPedestalCollimationChamberPlate3Height = 60. * CLHEP::mm;
	fPedestalCollimationChamberPlate3Length = 950. * CLHEP::mm;	
	
	//TransversalGirder
    fPedestalCollimationChamberTransversalGirder1Width = 1300. * CLHEP::mm;
    fPedestalCollimationChamberTransversalGirder1Height = 160. * CLHEP::mm;
    fPedestalCollimationChamberTransversalGirder1Length = 160. * CLHEP::mm;
    fPedestalCollimationChamberTransversalGirder2Width = 1300. * CLHEP::mm;
    fPedestalCollimationChamberTransversalGirder2Height = fPedestalCollimationChamberTransversalGirder1Height - 2 * 9. * CLHEP::mm;
    fPedestalCollimationChamberTransversalGirder2Length = fPedestalCollimationChamberTransversalGirder1Length - 2 * 9. * CLHEP::mm;
    fPedestalCollimationChamberTransversalGirder4Width = 1300. * CLHEP::mm;
    fPedestalCollimationChamberTransversalGirder4Height = 0.5 * fPedestalCollimationChamberTransversalGirder2Height * 1.4142;
    fPedestalCollimationChamberTransversalGirder4Length = 0.5 * fPedestalCollimationChamberTransversalGirder2Length * 1.4142;
    fPedestalCollimationChamberTransversalGirder3Width = 1300. * CLHEP::mm;
    fPedestalCollimationChamberTransversalGirder3Height = fPedestalCollimationChamberTransversalGirder4Height + 2 * 5.5 * CLHEP::mm;
    fPedestalCollimationChamberTransversalGirder3Length = fPedestalCollimationChamberTransversalGirder4Length + 2 * 5.5 * CLHEP::mm;
	fPedestalCollimationChamberTransversalGirderShiftX = 70. * CLHEP::mm;
    
    //LongitudinalGirder
    fPedestalCollimationChamberLongitudinalGirder1Width = 160. * CLHEP::mm;
    fPedestalCollimationChamberLongitudinalGirder1Height = 160. * CLHEP::mm;
    fPedestalCollimationChamberLongitudinalGirder1Length = 600. * CLHEP::mm - 2 * fPedestalCollimationChamberTransversalGirder1Length;
    fPedestalCollimationChamberLongitudinalGirder2Width = fPedestalCollimationChamberLongitudinalGirder1Width - 2 * 9. * CLHEP::mm;
    fPedestalCollimationChamberLongitudinalGirder2Height = fPedestalCollimationChamberLongitudinalGirder1Height - 2 * 9. * CLHEP::mm;
    fPedestalCollimationChamberLongitudinalGirder2Length = 600. * CLHEP::mm - 2 * fPedestalCollimationChamberTransversalGirder1Length;
    fPedestalCollimationChamberLongitudinalGirder4Width = 0.5 * fPedestalCollimationChamberLongitudinalGirder2Width * 1.4142;
    fPedestalCollimationChamberLongitudinalGirder4Height = 0.5 * fPedestalCollimationChamberLongitudinalGirder2Height * 1.4142;
    fPedestalCollimationChamberLongitudinalGirder4Length = 600. * CLHEP::mm - 2 * fPedestalCollimationChamberTransversalGirder1Length;
    fPedestalCollimationChamberLongitudinalGirder3Width = fPedestalCollimationChamberLongitudinalGirder4Width + 2 * 5.5 * CLHEP::mm;
    fPedestalCollimationChamberLongitudinalGirder3Height = fPedestalCollimationChamberLongitudinalGirder4Height + 2 * 5.5 * CLHEP::mm;
    fPedestalCollimationChamberLongitudinalGirder3Length = 600. * CLHEP::mm - 2 * fPedestalCollimationChamberTransversalGirder1Length;
	
	//Strut
    fPedestalCollimationChamberStrutWidth = 290. * CLHEP::mm;
	fPedestalCollimationChamberStrutHeight = 175. * CLHEP::mm; 
    fPedestalCollimationChamberStrutLength = 120. * CLHEP::mm;
    fPedestalCollimationChamberStrutShiftZ = 55. * CLHEP::mm; 
    fPedestalCollimationChamberStrutExternalHoleWidth = 210. * CLHEP::mm;
    fPedestalCollimationChamberStrutExternalHoleHeight = 135. * CLHEP::mm; 
    fPedestalCollimationChamberStrutExternalHoleLength = 80. * CLHEP::mm + 2. * CLHEP::mm; //2 mm added for solving overlapping problems
    fPedestalCollimationChamberStrutInternalHoleWidth = 120. * CLHEP::mm;
    fPedestalCollimationChamberStrutInternalHoleHeight = 80. * CLHEP::mm;
    fPedestalCollimationChamberStrutInternalHoleLength = fPedestalCollimationChamberStrutLength - fPedestalCollimationChamberStrutExternalHoleLength + 10.* CLHEP::mm;
    fPedestalCollimationChamberStrutInternalHoleShiftY = 55. * CLHEP::mm;
	
	//ExternalScrew
	fPedestalCollimationChamberExternalScrewRadius = 30. * CLHEP::mm;
	fPedestalCollimationChamberExternalScrewHeight = 73. * CLHEP::mm;  
	
	//CentralScrew
	fPedestalCollimationChamberCentralScrewRadius = 30. * CLHEP::mm;
    fPedestalCollimationChamberCentralScrewHeight = fPedestalCollimationChamberExternalScrewHeight + (fPedestalCollimationChamberStrutHeight - fPedestalCollimationChamberTransversalGirder1Height);
	
	//Block
	fPedestalCollimationChamberBlockWidth = 400. * CLHEP::mm;
	fPedestalCollimationChamberBlockHeight = 610. * CLHEP::mm;
	fPedestalCollimationChamberBlockLength = 400. * CLHEP::mm;	
	fPedestalCollimationChamberBlockThickness = 10. * CLHEP::mm;	

	//Plate	
	fSpaceFabPlateWidth = 1295. * CLHEP::mm;
	fSpaceFabPlateHeight = 10. * CLHEP::mm;
	fSpaceFabPlateLength = 810. * CLHEP::mm;
	fSpaceFabPlateHoleWidth = 180. * CLHEP::mm;
	fSpaceFabPlateHoleHeight = 12. * CLHEP::mm;
	fSpaceFabPlateHoleLength = 110. * CLHEP::mm;
	
	//Binary
	fSpaceFabBinaryWidth = 880. * CLHEP::mm;
	fSpaceFabBinaryHeight = 38. * CLHEP::mm; //CHECK!!!!
	fSpaceFabBinaryLength = 150. * CLHEP::mm;
	fSpaceFabBinaryShiftX = 35. * CLHEP::mm;
	fSpaceFabBinaryShiftZ = 90. * CLHEP::mm;
	
	//CartDown
	fSpaceFabCartDownWidth = 150. * CLHEP::mm;
	fSpaceFabCartDownHeight = 23. * CLHEP::mm;
	fSpaceFabCartDownLength = 150. * CLHEP::mm; 
	fSpaceFabCartFrontRearShiftX = 98. * CLHEP::mm;
	fSpaceFabCartCentralShiftX = 524. * CLHEP::mm;
	
	//CartUp
	fSpaceFabCartUpWidth = 150. * CLHEP::mm;
	fSpaceFabCartUpHeight = 54.5 * CLHEP::mm;
	fSpaceFabCartUpLength = 330. * CLHEP::mm; 

	//PedestalCollimationChamberEnvelope
	fPedestalCollimationChamberEnvelopeWidth = 1440. * CLHEP::mm; 
	fPedestalCollimationChamberEnvelopeHeight = 973. * CLHEP::mm + fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight;  
	fPedestalCollimationChamberEnvelopeLength = fPedestalCollimationChamberPlate3Length;
	fPedestalCollimationChamberDisplacement = 22.517 * CLHEP::mm;	
	//----------------------------------------------------------------------------------

	//----------------------Pedestals M30 M31 (Nicola)----------------------------------
	//Pedestals M30 M31
	if(bLine==1){
		fPedestalM30FrontPositionZ = 11417. * CLHEP::mm;
		fPedestalM30RearPositionZ = 13027. * CLHEP::mm;
		fPedestalM31FrontPositionZ = 14131. * CLHEP::mm;
		fPedestalM31RearPositionZ = 15989. * CLHEP::mm;
	}
	else{
		fPedestalM30FrontPositionZ = 12524. * CLHEP::mm;
		fPedestalM30RearPositionZ = 14134. * CLHEP::mm;
		fPedestalM31FrontPositionZ = 15266. * CLHEP::mm;
		fPedestalM31RearPositionZ = 17124. * CLHEP::mm;
	}	
	
	//Plate1
	fPedestalPlate1Width = 700 * CLHEP::mm;
	fPedestalPlate1Height = 25 * CLHEP::mm;
	fPedestalPlate1Length = 700. * CLHEP::mm;
	
	//Plate2
	fPedestalPlate2Width = 630. * CLHEP::mm;
	fPedestalPlate2Height = 30. * CLHEP::mm;
	fPedestalPlate2Length = 630. * CLHEP::mm;
	
	//Block
	fPedestalBlockWidth = 400. * CLHEP::mm;
	fPedestalBlockHeight = 815. * CLHEP::mm;
	fPedestalBlockLength = 400. * CLHEP::mm;
	fPedestalBlockThickness = 10. * CLHEP::mm;
	
	//Plate3
	fPedestalPlate3Width = 800. * CLHEP::mm;
	fPedestalPlate3Height = 60. * CLHEP::mm;
	fPedestalPlate3Length = 480. * CLHEP::mm;
	//----------------------------------------------------------------------------------	
	                    
    //Beam Pipe
    fBeamPipeA0InnerRadius = 2. * CLHEP::cm;
    fBeamPipeA0OuterRadius = 2.16 * CLHEP::cm;
    
    fBeamPipeA0Length = fCollimatorCenterDistance - 0.5 * fCollimChamberLength - 2 * fFlangeLength;    	
    
	fBeamPipeA1Length = 1303. * CLHEP::mm;
    if(bLine==1){
        fBeamPipeA1Length = 1273. * CLHEP::mm;
	}
    
    //Concrete
    if(bLine==1){
		//High Energy line variables definition for walls
		fRoomWidth = 1000. * CLHEP::cm;
		fRoomLength = 1239. * CLHEP::cm;
		fRoomHeight = 500. * CLHEP::cm;

		fBeamHeight = 150 * CLHEP::cm;      //beamline Height from floor
		fRoomShiftX = -177.5 * CLHEP::cm;   //shift on x-axis of the room center with respect to beam direction axis 
		fRoomShiftY = fRoomHeight * 0.5 - fBeamHeight; // shift on y-axis of the room center with respect to beam direction axis	

		fFloorThickness = 150. * CLHEP::cm;
		fTopThickness = 150. * CLHEP::cm;   //ceiling thickness
		fWallThickness = 150. * CLHEP::cm;
		fWallThickness2 = 200. * CLHEP::cm; //only for HE line, left wall has different thickness
		fWallHeight =  800.* CLHEP::cm;     //height of side walls

		fConcreteA0RadiusPipe = 2.16 * CLHEP::cm;
		fConcreteA0Distance = 260. * CLHEP::cm + fWallThickness * 0.5;
		
		fCollimChamberConcreteA1Distance = 27.3 * CLHEP::cm; //Gian added
		fConcreteA1CSPECDistance = 25.1 * CLHEP::cm;         //Gian added
	}
	else{
		//Low Energy line variables definition for walls
		fRoomWidth = 850. * CLHEP::cm;
		fRoomLength = 3850. * CLHEP::cm;
		fRoomHeight = 500. * CLHEP::cm;
	
		fBeamHeight = 150 * CLHEP::cm;     	//beamline Height from floor
		fRoomShiftX = 147. * CLHEP::cm;    	//shift on x-axis of the room center with respect to beam direction axis
		fRoomShiftY = fRoomHeight * 0.5 - fBeamHeight; // shift on y-axis of the room center with respect to beam direction axis

		fFloorThickness = 150. * CLHEP::cm;
		fTopThickness = 150. * CLHEP::cm;  	//ceiling thickness
		fWallThickness = 150. * CLHEP::cm;
		fWallThickness2 = 150. * CLHEP::cm;	//only for HE line, left wall has different thickness
		fWallHeight =  800.* CLHEP::cm;    	//height of side walls
	
		fConcreteA0RadiusPipe = 2.16 * CLHEP::cm;
		fConcreteA0Distance = 611. * CLHEP::cm + fWallThickness * 0.5;
		
		fCollimChamberConcreteA1Distance = 30.3 * CLHEP::cm; //Gian added
		fConcreteA1CSPECDistance = 25.4 * CLHEP::cm;         //Gian added
	}

	//A1 shielding block concrete - Same for HE and LE lines
 	fConcreteA1Length = 100. * CLHEP::cm;
	fConcreteA1Width = 200. * CLHEP::cm;
	fConcreteA1Height = 250. * CLHEP::cm;
	fConcreteA1RadiusPipe = 2.16 * CLHEP::cm;
	fConcreteA1Distance = fCollimatorCenterDistance + fCollimChamberLength * 0.5 + 2. * fFlangeLength + fCollimChamberConcreteA1Distance + fConcreteA1Length * 0.5;
	fA1LeadPipeCapWidth = 140. * CLHEP::mm;
	fA1LeadPipeCapHeight = 140. * CLHEP::mm;
	fA1LeadPipeCapLength = 250. * CLHEP::mm;
    
    //Girders, Pedestals and vacuum chambers
    fGirderHeight = 214. * CLHEP::mm;
    fGirderWidth = 124. * CLHEP::mm;
    fGirderThickness = 12. * CLHEP::mm;
    fGirderJointLength  = 246. * CLHEP::mm;
    
    fPedestalHeight  = 645. * CLHEP::mm;      
    fPedestalWidth = 300. * CLHEP::mm;
    fPedestalLength = 300. * CLHEP::mm; 
    fPedestalThickness = 10. * CLHEP::mm;
    
    fGirderY = - 400. * CLHEP::mm - fGirderHeight * 0.5;
    fGirderM31Y = 1132. * CLHEP::mm - fBeamHeight;
    fPedestalY = - fBeamHeight + fPedestalHeight * 0.5;

    fM27AGirderLength = 1900. * CLHEP::mm;
    fM31GirderLength = 2417. * CLHEP::mm;
    
    fCSPECInnerRadius = 200. * CLHEP::mm;
    fCSPECOuterRadius = 205. * CLHEP::mm;
    fCSPECPbInnerRadius = 205. * CLHEP::mm;
    fCSPECPbOuterRadius = 215. * CLHEP::mm;
    fCSPECSheetInnerRadius = 215. * CLHEP::mm;
    fCSPECSheetOuterRadius = 216.5 * CLHEP::mm;
    fCSPECLength = 2607. * CLHEP::mm;
    fCSPECWindowLength = 20. * CLHEP::mm;
    
    fM30Distance = fPedestalM30FrontPositionZ + (fPedestalM30RearPositionZ - fPedestalM30FrontPositionZ) * 0.5;
	fM31Distance = fPedestalM31FrontPositionZ + (fPedestalM31RearPositionZ - fPedestalM31FrontPositionZ) * 0.5;
	   
    if(bLine==1){
		//H.E. line
		fThetaElectron = 0.12967 * CLHEP::rad;
		fThetaM34 = 0.12967 * CLHEP::rad + (3.14159 * CLHEP::rad * 0.5 - 0.12967 * CLHEP::rad) * 0.5;
		
		fM32GirderLength = 889. * CLHEP::mm * cos(fThetaElectron); //check!!!
		fM33GirderLength = 2600. * CLHEP::mm;
		fM34GirderLength = 2350. * CLHEP::mm;
						
		fM27ADistance = fConcreteA0Distance + fWallThickness * 0.6 + fM27AGirderLength * 0.5;
		fM32Distance = fConcreteA1Distance - fConcreteA1Length * 0.5 -  (156. * CLHEP::mm) * cos(fThetaElectron) - fM32GirderLength * 0.5;
		fM33Distance = fConcreteA1Distance + fConcreteA1Length * 0.5 + 294. * CLHEP::mm + fM33GirderLength * 0.5;
		fM34Distance = fConcreteA1Distance + fConcreteA1Length * 0.5 + 4383. * CLHEP::mm; 
		
		fM32X = 503.4 * CLHEP::mm; 
		fM33X = 908. * CLHEP::mm; 
		fM34X = 1765. * CLHEP::mm; 
		
		//M27A MAgnet
		fM27AMagnetWidth = 392.6 * CLHEP::mm;
		fM27AMagnetHeight = 196. * CLHEP::mm;
		fM27AMagnetLength = 683. * CLHEP::mm;
		fM27AMagnetPoleWidth = 114. * CLHEP::mm;
		fM27AMagnetPoleHeight = 53. * CLHEP::mm;
	
		fM27AMagnetCoilWidth = 264. * CLHEP::mm; 		//overall
		fM27AMagnetCoilHeight = 38.1 * CLHEP::mm;
		fM27AMagnetCoilThickness = 74.3 * CLHEP::mm; 	//single donut width
		fM27AMagnetCoilLength = fM27AMagnetLength + 2. * fM27AMagnetCoilThickness;
		
		//M34 Magnet
		fM34MagnetRadius = 1600 *CLHEP::mm;
		fM34MagnetInnerRadius = fM34MagnetRadius - 310 * CLHEP::mm;
        fM34MagnetOuterRadius = fM34MagnetRadius + 310 * CLHEP::mm;
        fM34MagnetHeight = 452 * CLHEP::mm;
        fM34MagnetThetaStart = -3.14159 * CLHEP::rad + fThetaElectron + (112./fM34MagnetRadius)*CLHEP::rad;
        fM34MagnetThetaEnd = 3.14159 * CLHEP::rad * 0.5 - fThetaElectron - (200./fM34MagnetRadius)*CLHEP::rad;
        
        fM34MagnetCoilHeight = 98 * CLHEP::mm;
		fM34MagnetCoilThickness = 106 * CLHEP::mm; 		//single donut width
		fM34MagnetCoilThetaStart = -3.14159 * CLHEP::rad + fThetaElectron;
        fM34MagnetCoilThetaEnd = 3.14159 * CLHEP::rad * 0.5 - fThetaElectron;
        fM34MagnetCoilHoleThetaStart = -3.14159 * CLHEP::rad + fThetaElectron + (110./fM34MagnetRadius)*CLHEP::rad;
        fM34MagnetCoilHoleThetaEnd = 3.14159 * CLHEP::rad * 0.5 - fThetaElectron - (195./fM34MagnetRadius)*CLHEP::rad;
	}
	else{
		//L.E. line TO BE DEFINED !!
		fM27ADistance = fConcreteA1Distance - fConcreteA1Length * 0.5 - 3683. * CLHEP::mm - fM27AGirderLength * 0.5;
	}
	
	//Detector SCREEN 
   	fTransparentDetectorWidth = fRoomWidth;   
  	fTransparentDetectorHeight = fRoomHeight; 
   	fTransparentDetectorLength = 1. * CLHEP::cm;
            
    //Detector RACKS
    fRackWidth = 60. * CLHEP::cm;
    fRackLength = 80. * CLHEP::cm;
    fRack1Height = 70. * CLHEP::cm;
    fRack2Height = 95. * CLHEP::cm;
   
	fRack1PositionX = 0.;
	fRack1PositionY = -fBeamHeight + fRack1Height * 0.5;
	fRack1PositionZ = fM30Distance;

	fRack2PositionX = 0.;
	fRack2PositionY = -fBeamHeight + fRack2Height * 0.5;
	fRack2PositionZ = fM31Distance;

 	//Detector Hexapod	
 	fHexapodWidth = 10. * CLHEP::cm;
	fHexapodLength = 10. * CLHEP::cm;
	fHexapodHeight = 10. * CLHEP::cm;
		  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

void G4ELINP_DetectorConstruction::DefineMaterials()
{
    
    //definition of vacuum
    G4double z = 7.;
    G4double a = 14.007 * CLHEP::g/CLHEP::mole;
    G4double density = CLHEP::universe_mean_density;
    G4double pressure = 1.E-6 * 1.E-3 * CLHEP::bar;	//10-6 mbar
    G4double temperature = 300. * CLHEP::kelvin;
    G4Material* Vacuum = new G4Material("Vacuum", z, a, density, kStateGas, temperature, pressure);
    
    //getting NIST materials
    G4Material* G4_Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
    G4Material* G4_W = G4NistManager::Instance()->FindOrBuildMaterial("G4_W");
    G4Material* G4_Pb = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");
    G4Material* G4_Concrete = G4NistManager::Instance()->FindOrBuildMaterial("G4_CONCRETE");
    G4Material* G4_AIR = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
    G4Material* G4_Cu = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu");
    G4Material* G4_Be = G4NistManager::Instance()->FindOrBuildMaterial("G4_Be");
    G4Material* G4_Kapton = G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");

    //definition of stainless steel (not in NIST) for pipes and sand for pedestals 
    G4Element* C  = G4NistManager::Instance()->FindOrBuildElement("C");
    G4Element* Si = G4NistManager::Instance()->FindOrBuildElement("Si");
    G4Element* Cr = G4NistManager::Instance()->FindOrBuildElement("Cr");
    G4Element* Mn = G4NistManager::Instance()->FindOrBuildElement("Mn");
    G4Element* Ni = G4NistManager::Instance()->FindOrBuildElement("Ni");
    G4Element* Fe = G4NistManager::Instance()->FindOrBuildElement("Fe");
    G4Element* H  = G4NistManager::Instance()->FindOrBuildElement("H");    
	G4Element* O  = G4NistManager::Instance()->FindOrBuildElement("O"); 
	G4Element* Na = G4NistManager::Instance()->FindOrBuildElement("Na"); 
	G4Element* K  = G4NistManager::Instance()->FindOrBuildElement("K"); 	
	G4Element* Ca = G4NistManager::Instance()->FindOrBuildElement("Ca"); 	
	G4Element* Al = G4NistManager::Instance()->FindOrBuildElement("Al"); 	
	
	//Stainless Steel
    G4double density_SS;
    G4int ncomponents_SS;
    G4double fractionmass;
    G4Material* StainlessSteel = new G4Material("StainlessSteel", density_SS = 8.06 * CLHEP::g/CLHEP::cm3, ncomponents_SS = 6);
    StainlessSteel->AddElement(C, fractionmass = 0.001);
    StainlessSteel->AddElement(Si, fractionmass = 0.007);
    StainlessSteel->AddElement(Cr, fractionmass = 0.18);
    StainlessSteel->AddElement(Mn, fractionmass = 0.01);
    StainlessSteel->AddElement(Fe, fractionmass = 0.712);
    StainlessSteel->AddElement(Ni, fractionmass = 0.09);
       
    //Sand composition and density from Compendium of Material Composition Data for Radiation Transport Modeling, 
    //Homeland Security, Revision 1 -PIET-43741-TM-963 PNNL-15870 Rev. 1
    G4double density_Sand;
    G4int ncomponents_Sand;
    G4double fractionmass_Sand;
    G4Material* Sand = new G4Material("Sand", density_Sand = 1.700 * CLHEP::g/CLHEP::cm3, ncomponents_Sand = 9);
    Sand->AddElement(H, fractionmass_Sand = 0.007833);
    Sand->AddElement(C, fractionmass_Sand = 0.003360);
    Sand->AddElement(O, fractionmass_Sand = 0.536153);
    Sand->AddElement(Al, fractionmass_Sand = 0.034401);      
    Sand->AddElement(Na, fractionmass_Sand = 0.01763);    
    Sand->AddElement(Si, fractionmass_Sand = 0.365067); 
    Sand->AddElement(K, fractionmass_Sand = 0.011622);
    Sand->AddElement(Ca, fractionmass_Sand = 0.011212);
    Sand->AddElement(Fe, fractionmass_Sand = 0.013289);
  
    //Nicola: Definition of second collimator's material (compound of 97% W, 2% Ni, 1% Fe)
    G4double density_CollimatorMaterial;
    G4int ncomponents_CollimatorMaterial;
    G4double fractionmass_CollimatorMaterial;
    G4Material* CollimatorMaterial = new G4Material("CollimatorMaterial", density_CollimatorMaterial = 18.5 * CLHEP::g/CLHEP::cm3, ncomponents_CollimatorMaterial = 3);
    CollimatorMaterial->AddMaterial(G4_W, fractionmass_CollimatorMaterial = 0.97);
    CollimatorMaterial->AddElement(Ni, fractionmass_CollimatorMaterial = 0.02);
    CollimatorMaterial->AddElement(Fe, fractionmass_CollimatorMaterial = 0.01);
    
    //air
    fWorldMaterial = G4_AIR;
    
	//Beam Pipe
    fBeamPipeMaterial = StainlessSteel;
    fConcreteMaterial = G4_Concrete;
    fLeadPipeCapMaterial = G4_Pb;
    fBeamPipeVacuum = Vacuum;
    
    //Collimation Chamber (Gian)
    fBasePlateMaterial = StainlessSteel; 
    fBasePlateWindowMaterial = StainlessSteel; 
    fTransvBeamMaterial = StainlessSteel; 
    fBasePlateEnvelopeMaterial = Vacuum;
    fCollimChamberMaterial = StainlessSteel; 
   	fCollimChamberPumpMaterial = StainlessSteel; 
    fCollimChamberFootMaterial = StainlessSteel; 
    fSpaceFabPlatesMaterial = StainlessSteel; 
    fCollimationEnvelopeMaterial = G4_AIR;
    
    //Windows
    fEnterWindowMaterial = G4_Al;
    fA3WindowMaterial = G4_Be;
    
    //Collimator (Nicola)
    fCollimatorMaterial = CollimatorMaterial;      
    fCollimatorSupportMaterial = StainlessSteel;   
    fCollimatorBlockBaseMaterial = StainlessSteel;
    fCollimatorGearMaterial = StainlessSteel;      
    fCollimatorScrewMaterial = StainlessSteel; 
    fCollimatorBlockMaterial = StainlessSteel;     
    fCollimatorPeakMaterial = StainlessSteel;  
    
    //PedestalCollimationChamber and SpaceFab (Nicola)
    fPedestalCollimationChamberEnvelopeMaterial = fBeamPipeVacuum;
    fPedestalCollimationChamberPlate1Material = fConcreteMaterial;
    fPedestalCollimationChamberPlate2Material = StainlessSteel;
    fPedestalCollimationChamberPlate3Material = StainlessSteel;
    fPedestalCollimationChamberTransversalGirderMaterial = G4_Al;
    fPedestalCollimationChamberLongitudinalGirderMaterial = G4_Al;
    fPedestalCollimationChamberStrutMaterial = StainlessSteel;
    fPedestalCollimationChamberExternalScrewMaterial = StainlessSteel; 
    fPedestalCollimationChamberCentralScrewMaterial = StainlessSteel;
    fPedestalCollimationChamberBlockMaterial = StainlessSteel;
    fPedestalCollimationChamberBlockInsideMaterial = G4_AIR;
    fSpaceFabPlateMaterial = StainlessSteel;
    fSpaceFabBinaryMaterial = StainlessSteel;
    fSpaceFabCartDownMaterial = StainlessSteel;
    fSpaceFabCartUpMaterial = StainlessSteel;

    //Pedestals M30 and M31 (Nicola)
    fPedestalPlate1Material = fConcreteMaterial;
    fPedestalPlate2Material = StainlessSteel;
    fPedestalPlate3Material = StainlessSteel;
    fPedestalBlockMaterial = StainlessSteel;
    fPedestalBlockInsideMaterial = G4_AIR;
 
    //Girders and Pedestals
    fGirderMaterial = G4_Al;
    fPedestalMaterial = StainlessSteel;
    fPedestalInsideMaterial = Sand;
    
    //Magnet
    fMagnetMaterial = StainlessSteel;
    fCoilMaterial = G4_Cu;
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* G4ELINP_DetectorConstruction::Construct()
{
    
    //World
    fWorldSolid = new G4Box("World", fWorldSize.x()/2., fWorldSize.y()/2., fWorldSize.z()/2.);
    
    fWorldLogic = new G4LogicalVolume(fWorldSolid, fWorldMaterial, "World");
    
    fWorldLogic->SetVisAttributes(G4VisAttributes::Invisible);
    
    fWorldPhysical = new G4PVPlacement(0, G4ThreeVector(), fWorldLogic, "World", 0, false, 0);
                                          
    //---------------------------------------Collimation Chamber (Gian)-------------------------------------       
    //Collimation Envelope
    G4VisAttributes* fCollimationEnvelopeVisAttribute = new G4VisAttributes();
    fCollimationEnvelopeVisAttribute->SetForceSolid(false);
    
    fCollimationEnvelopeWidth = fFlangeOutOutDiameter;
    fCollimationEnvelopeHeight = fCollimChamberPumpHeight + fCollimChamberOutDiameter + fCollimChamberFootHeight + fSpaceFabUpperPlateHeight + fSpaceFabLowerPlateHeight;
    fCollimationEnvelopeLength = fCollimChamberLength + fFlangeLength * 4.;
        
    G4Box* fCollimationEnvelopeSolid = new G4Box("CollimationEnvelopeSolid", fCollimationEnvelopeWidth * 0.5, fCollimationEnvelopeHeight * 0.5, fCollimationEnvelopeLength * 0.5);
    
    fCollimationEnvelopeLogic = new G4LogicalVolume(fCollimationEnvelopeSolid, fCollimationEnvelopeMaterial, "CollimationEnvelopeLogic");
    
    fCollimationEnvelopeLogic->SetVisAttributes(fCollimationEnvelopeVisAttribute);
    fCollimationEnvelopeLogic->SetVisAttributes(G4VisAttributes::GetInvisible());
    
    G4double fCollimationEnvelopeShift = (fCollimChamberPumpHeight - fCollimChamberFootHeight) * 0.5 - (fSpaceFabUpperPlateHeight + fSpaceFabLowerPlateHeight) * 0.5; 	
	G4ThreeVector fCollimationEnvelopePositionVector = G4ThreeVector(0., fCollimationEnvelopeShift, fCollimatorCenterDistance) + fCollimationEnvelopeMisposition;
	
	G4RotationMatrix* fCollimationEnvelopeRotationMatrix = new G4RotationMatrix(0., 0., 0.);
	fCollimationEnvelopeRotationMatrix->rotateX(fCollimationEnvelopeMisalignment.x() * CLHEP::deg);
    fCollimationEnvelopeRotationMatrix->rotateY(fCollimationEnvelopeMisalignment.y() * CLHEP::deg);
    fCollimationEnvelopeRotationMatrix->rotateZ(fCollimationEnvelopeMisalignment.z() * CLHEP::deg);
    
    fCollimationEnvelopePhysical = new G4PVPlacement(fCollimationEnvelopeRotationMatrix, fCollimationEnvelopePositionVector, fCollimationEnvelopeLogic, "fCollimationEnvelopePhysical", fWorldLogic, false, 0);    
    
    //change BeamPipeLength to avoid superposition in case of Collimation Chamber misaligment/misposition    
    G4double fBeamPipeLengthVariation = 0.;
    if (std::max(abs(fCollimationEnvelopeMisalignment.x()), abs(fCollimationEnvelopeMisalignment.y())) > 0. || abs(fCollimationEnvelopeMisposition.z()) > 0.){
		fBeamPipeLengthVariation += 30. * CLHEP::mm; //arbitrary reasonable value
	}	   
    fBeamPipeA0Length -= fBeamPipeLengthVariation;
    fBeamPipeA1Length -= fBeamPipeLengthVariation;
     	
    //Chanber and Flanges       
    if(bCollimChamber) {
    	G4VisAttributes* fCollimChamberVisAttribute = new G4VisAttributes();
    	fCollimChamberVisAttribute->SetForceSolid(true);
       
    	G4Tubs* fFlangeOutSolid = new G4Tubs("FlangeOut",
                                            fFlangeOutInDiameter * 0.5,
                                            fFlangeOutOutDiameter * 0.5,
                                            fFlangeLength * 0.5,
                                            0 * CLHEP::deg,
                                            360 * CLHEP::deg);
                                            
        G4Tubs* fFlangeInSolid = new G4Tubs("FlangeIn",
                                            fFlangeInInDiameter * 0.5,
                                            fFlangeInOutDiameter * 0.5,
                                            fFlangeLength * 0.5,
                                            0 * CLHEP::deg,
                                            360 * CLHEP::deg);
                                            
        G4Tubs* fChamberSolid = new G4Tubs("Chamber",
                                            fCollimChamberInDiameter * 0.5,
                                            fCollimChamberOutDiameter * 0.5,
                                            fCollimChamberLength * 0.5,
                                            0 * CLHEP::deg,
                                            360 * CLHEP::deg);
                                                         
        G4UnionSolid* fFlangeUnionSolid = new G4UnionSolid("FlangeUnion", fFlangeOutSolid, fFlangeInSolid, 0, G4ThreeVector(0., 0., fFlangeLength));            
        
        G4RotationMatrix* fFlangeUnionRotationMatrix = new G4RotationMatrix(0., 180. * CLHEP::deg, 0.); 
        
        G4UnionSolid* fFlangeUnionChamberSolid = new G4UnionSolid("FlangeUnionChamber", fChamberSolid, fFlangeUnionSolid, fFlangeUnionRotationMatrix, G4ThreeVector(0., 0., fCollimChamberLength * 0.5 + fFlangeLength * 1.5));            
        
        G4UnionSolid* fCollimChamberSolid = new G4UnionSolid("CollimChamberSolid", fFlangeUnionChamberSolid, fFlangeUnionSolid, 0, G4ThreeVector(0., 0., -(fCollimChamberLength * 0.5 + fFlangeLength * 1.5))); 
                     
        fCollimChamberLogic = new G4LogicalVolume(fCollimChamberSolid, fCollimChamberMaterial, "CollimChamberLogic");
        
        fCollimChamberLogic->SetVisAttributes(fCollimChamberVisAttribute);
        
        G4ThreeVector fCollimChamberPositionVector = G4ThreeVector(0., -fCollimationEnvelopeShift, 0.);

        fCollimChamberPhysical = new G4PVPlacement(0, fCollimChamberPositionVector, fCollimChamberLogic, "CollimChamberPhysical", fCollimationEnvelopeLogic, false, 0);
	}
	
	//EnterWindow (Nicola)
    if(bEnterWindow) {
		G4VisAttributes* fEnterWindowVisAttribute = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
		fEnterWindowVisAttribute->SetForceSolid(true);

		G4Tubs* fEnterWindowSolid = new G4Tubs("EnterWindowSolid",
												0.,
												fEnterWindowRadius,
												fEnterWindowLength * 0.5,
												0 * CLHEP::deg,
												360 * CLHEP::deg);
	
		fEnterWindowLogic = new G4LogicalVolume(fEnterWindowSolid, fEnterWindowMaterial, "EnterWindowLogic");

		fEnterWindowLogic->SetVisAttributes(fEnterWindowVisAttribute);

		G4ThreeVector fEnterWindowPositionVector = G4ThreeVector(0., -fCollimationEnvelopeShift, -(fCollimationEnvelopeLength * 0.5 - fEnterWindowLength * 0.5));

		fEnterWindowPhysical = new G4PVPlacement(0, fEnterWindowPositionVector, fEnterWindowLogic, "EnterWindowPhysical", fCollimationEnvelopeLogic, false, 0);
    }		

    //Pump
    if(bCollimChamberPump) {
    	G4VisAttributes* fCollimChamberPumpVisAttribute = new G4VisAttributes();
    	fCollimChamberPumpVisAttribute->SetForceSolid(true);
    	
    	G4Tubs* fCollimChamberPumpSolid = new G4Tubs("CollimChamberPumpSolid",
                                            0.,
                                            fCollimChamberPumpDiameter * 0.5,
                                            fCollimChamberPumpHeight * 0.5,
                                            0 * CLHEP::deg,
                                            360 * CLHEP::deg);
                                                            
        fCollimChamberPumpLogic = new G4LogicalVolume(fCollimChamberPumpSolid, fCollimChamberPumpMaterial, "CollimChamberPumpLogic");
        
        fCollimChamberPumpLogic->SetVisAttributes(fCollimChamberPumpVisAttribute);
        
        G4RotationMatrix* fCollimChamberPumpRotationMatrix = new G4RotationMatrix(0., 90. * CLHEP::deg, 0.); 
        
        G4ThreeVector fCollimChamberPumpPositionVector = G4ThreeVector(0., (fCollimationEnvelopeHeight - fCollimChamberPumpHeight) * 0.5, fCollimChamberPumpDistance - fCollimChamberLength * 0.5);

        fCollimChamberPumpPhysical = new G4PVPlacement(fCollimChamberPumpRotationMatrix, fCollimChamberPumpPositionVector, fCollimChamberPumpLogic, "CollimChamberPumpPhysical", fCollimationEnvelopeLogic, false, 0);   	
    }
       
    //Feet
    if(bCollimChamberFoot) {
    	G4VisAttributes* fCollimChamberFootVisAttribute = new G4VisAttributes();
    	fCollimChamberFootVisAttribute->SetForceSolid(true);
    	
    	G4Box* fCollimChamberFootSolid = new G4Box("CollimChamberFootSolid", fCollimChamberFootWidth * 0.5, fCollimChamberFootHeight * 0.5, fCollimChamberFootLength * 0.5);
    	
    	fCollimChamberFootLogic = new G4LogicalVolume(fCollimChamberFootSolid, fCollimChamberFootMaterial, "CollimChamberFootLogic");
        
        fCollimChamberFootLogic->SetVisAttributes(fCollimChamberFootVisAttribute);
        
        for(int j1=1;j1<6;j1++) {
               
        	G4ThreeVector fCollimChamberFootPositionVector = G4ThreeVector(0., 
        														-(fCollimationEnvelopeHeight - fCollimChamberFootHeight) * 0.5 + (fSpaceFabUpperPlateHeight + fSpaceFabLowerPlateHeight), 
        														(-fCollimChamberLength*0.5+fCollimChamberFoot1Distance+(j1-1)*fCollimChamberFootStep+(double(j1)-0.5)*fCollimChamberFootLength));
        	        	
        	fCollimChamberFootPhysical = new G4PVPlacement(0, fCollimChamberFootPositionVector, fCollimChamberFootLogic, "CollimChamberFootPhysical", fCollimationEnvelopeLogic, false, 0);
       }  	   	  	
    }       
    
	//SpaceFab Plates
    if(bSpaceFabPlates) {
    	G4VisAttributes* fSpaceFabPlatesVisAttribute = new G4VisAttributes();
    	fSpaceFabPlatesVisAttribute->SetForceSolid(true);
    	   	
    	//Upper Plate
    	G4Box* fSpaceFabUpperPlateSolid = new G4Box("SpaceFabUpperPlateSolid", fSpaceFabUpperPlateWidth * 0.5, fSpaceFabUpperPlateHeight * 0.5, fSpaceFabUpperPlateLength * 0.5);
    	
    	fSpaceFabUpperPlateLogic = new G4LogicalVolume(fSpaceFabUpperPlateSolid, fSpaceFabPlatesMaterial, "SpaceFabUpperPlateLogic");
    	
    	fSpaceFabUpperPlateLogic->SetVisAttributes(fSpaceFabPlatesVisAttribute);
    	
    	G4ThreeVector fSpaceFabUpperPlatePositionVector = G4ThreeVector(0., 
    															-(fCollimationEnvelopeHeight * 0.5 - fSpaceFabLowerPlateHeight - fSpaceFabUpperPlateHeight * 0.5),
    															(-fCollimChamberLength*0.5+fCollimChamberFoot1Distance+2.*fCollimChamberFootStep+2.5*fCollimChamberFootLength));
    	
    	fSpaceFabUpperPlatePhysical = new G4PVPlacement(0, fSpaceFabUpperPlatePositionVector, fSpaceFabUpperPlateLogic, "SpaceFabUpperPlatePhysical", fCollimationEnvelopeLogic, false, 0);       
    	
    	//Lower Plate
    	G4Box* fSpaceFabLowerPlate1Solid = new G4Box("SpaceFabUpperPlate1Solid", fSpaceFabLowerPlateWidth * 0.5, fSpaceFabLowerPlateHeight * 0.5, fSpaceFabLowerPlateLength * 0.5);
    	
		G4Box* fSpaceFabLowerPlate2Solid = new G4Box("SpaceFabUpperPlate2Solid", 
													(fSpaceFabLowerPlateWidth - 2. * fSpaceFabLowerPlateTickness) * 0.5, 
													fSpaceFabLowerPlateTickness * 0.51, 
													(fSpaceFabLowerPlateLength - 2. * fSpaceFabLowerPlateTickness) * 0.5);
    	
    	G4SubtractionSolid* fSpaceFabLowerPlateSolid  = new G4SubtractionSolid("SpaceFabUpperPlateSolid", fSpaceFabLowerPlate1Solid, fSpaceFabLowerPlate2Solid, 0, G4ThreeVector(0., -fSpaceFabLowerPlateTickness * 0.5, 0.));
    	
    	fSpaceFabLowerPlateLogic = new G4LogicalVolume(fSpaceFabLowerPlateSolid, fSpaceFabPlatesMaterial, "SpaceFabLowerPlateLogic");
    	
    	fSpaceFabLowerPlateLogic->SetVisAttributes(fSpaceFabPlatesVisAttribute);
    	
    	G4ThreeVector fSpaceFabLowerPlatePositionVector = G4ThreeVector(0., 
    															-(fCollimationEnvelopeHeight * 0.5 - fSpaceFabLowerPlateHeight * 0.5),
    															(-fCollimChamberLength*0.5+fCollimChamberFoot1Distance+2.*fCollimChamberFootStep+2.5*fCollimChamberFootLength));
    	
    	fSpaceFabLowerPlatePhysical = new G4PVPlacement(0, fSpaceFabLowerPlatePositionVector, fSpaceFabLowerPlateLogic, "SpaceFabLowerPlatePhysical", fCollimationEnvelopeLogic, false, 0); 	
    }
    
   	//Base Plate Envelope
	G4VisAttributes* fBasePlateEnvelopeVisAttribute = new G4VisAttributes();
    fBasePlateEnvelopeVisAttribute->SetForceSolid(false);
    
    fBasePlateEnvelopeRadius = fCollimChamberInDiameter * 0.5;
	fBasePlateEnvelopeSemiLength = (fCollimChamberLength + 2 * fFlangeLength) * 0.5;
	
	G4Tubs* fBasePlateEnvelopeSolid = new G4Tubs("BasePlateEnvelopeSolid",
                                            0.,
                                            fBasePlateEnvelopeRadius,
                                            fBasePlateEnvelopeSemiLength,
                                            0 * CLHEP::deg,
                                            360 * CLHEP::deg);
                                            
    fBasePlateEnvelopeLogic = new G4LogicalVolume(fBasePlateEnvelopeSolid, fBasePlateEnvelopeMaterial, "fBasePlateEnvelopeLogic");
    
    fBasePlateEnvelopeLogic->SetVisAttributes(fBasePlateEnvelopeVisAttribute);
    fBasePlateEnvelopeLogic->SetVisAttributes(G4VisAttributes::GetInvisible());
    
    G4ThreeVector fBasePlateEnvelopePositionVector = G4ThreeVector(0., -fCollimationEnvelopeShift, 0.);
    
    fBasePlateEnvelopePhysical = new G4PVPlacement(0, fBasePlateEnvelopePositionVector, fBasePlateEnvelopeLogic, "fBasePlateEnvelopePhysical", fCollimationEnvelopeLogic, false, 0);  
                                              
    //Base Plate    
    if(bBasePlate) {
    	G4VisAttributes* fBasePlateVisAttribute = new G4VisAttributes();
    	fBasePlateVisAttribute->SetForceSolid(true);

    	G4Box* fBasePlateOutSolid = new G4Box("BasePlateOut", fBasePlateOutWidth * 0.5, fBasePlateOutHeight * 0.5, fBasePlateOutLength * 0.5);

        G4Box* fBasePlateInSolid = new G4Box("BasePlateIn", (fBasePlateOutWidth - 2 * fBasePlateThickness) * 0.5, (fBasePlateOutHeight - fBasePlateThickness) * 0.5, fBasePlateOutLength * 0.51);

        G4Box* fBasePlateBigHoleSolid = new G4Box("BasePlateBigHole", fBasePlateBigHoleWidth * 0.5, fBasePlateThickness * 0.51, fBasePlateBigHoleLength * 0.5);

        G4Box* fBasePlateSmallHoleSolid = new G4Box("BasePlateSmallHole", fBasePlateSmallHoleWidth * 0.5, fBasePlateThickness * 0.51, fBasePlateSmallHoleLength * 0.5);

        G4SubtractionSolid* fBasePlateSolid1 = new G4SubtractionSolid("BasePlate1", fBasePlateOutSolid, fBasePlateInSolid, 0, G4ThreeVector(0., fBasePlateThickness * 0.5, 0.));
    
        G4SubtractionSolid* fBasePlateSolid2 = new G4SubtractionSolid("BasePlate2", fBasePlateSolid1, fBasePlateBigHoleSolid, 0, G4ThreeVector(0., -(fBasePlateOutHeight * 0.5 - fBasePlateThickness * 0.5), (fBasePlateBigHoleDistance - fBasePlateOutLength * 0.5) + fBasePlateBigHoleLength * 0.5));

        G4SubtractionSolid* fBasePlateSolid3 = new G4SubtractionSolid("BasePlate3", fBasePlateSolid2, fBasePlateSmallHoleSolid, 0, G4ThreeVector(0., -(fBasePlateOutHeight * 0.5 - fBasePlateThickness * 0.5), -10.5 * fBasePlateSmallHoleLength));

        G4SubtractionSolid* fBasePlateSolid4 = new G4SubtractionSolid("BasePlate4", fBasePlateSolid3, fBasePlateSmallHoleSolid, 0, G4ThreeVector(0., -(fBasePlateOutHeight * 0.5 - fBasePlateThickness * 0.5), -8.5 * fBasePlateSmallHoleLength));
   
        G4SubtractionSolid* fBasePlateSolid5 = new G4SubtractionSolid("BasePlate5", fBasePlateSolid4, fBasePlateSmallHoleSolid, 0, G4ThreeVector(0., -(fBasePlateOutHeight * 0.5 - fBasePlateThickness * 0.5), -6.5 * fBasePlateSmallHoleLength));

        G4SubtractionSolid* fBasePlateSolid6 = new G4SubtractionSolid("BasePlate6", fBasePlateSolid5, fBasePlateSmallHoleSolid, 0, G4ThreeVector(0., -(fBasePlateOutHeight * 0.5 - fBasePlateThickness * 0.5), -4.5 * fBasePlateSmallHoleLength));

        G4SubtractionSolid* fBasePlateSolid7 = new G4SubtractionSolid("BasePlate7", fBasePlateSolid6, fBasePlateSmallHoleSolid, 0, G4ThreeVector(0., -(fBasePlateOutHeight * 0.5 - fBasePlateThickness * 0.5), -2.5 * fBasePlateSmallHoleLength));
 
        G4SubtractionSolid* fBasePlateSolid8 = new G4SubtractionSolid("BasePlate8", fBasePlateSolid7, fBasePlateSmallHoleSolid, 0, G4ThreeVector(0., -(fBasePlateOutHeight * 0.5 - fBasePlateThickness * 0.5), -0.5 * fBasePlateSmallHoleLength));
               
        G4Box* fBasePlateEdgeSolid = new G4Box("BasePlateEdge", (fBasePlateThickness + 5.0 * CLHEP::mm) * 1.42 * 0.5, (fBasePlateThickness + 5.0 * CLHEP::mm) * 1.42 * 0.5, fBasePlateOutLength * 0.51);
                
        G4RotationMatrix* fBasePlateEdgeRotationMatrix = new G4RotationMatrix(45 * CLHEP::deg, 0., 0.);       
                
		G4SubtractionSolid* fBasePlateSolid9 = new G4SubtractionSolid("BasePlate9", fBasePlateSolid8, fBasePlateEdgeSolid, fBasePlateEdgeRotationMatrix, G4ThreeVector(-fBasePlateOutWidth * 0.5, - fBasePlateOutHeight * 0.5, 0.));
                
		G4SubtractionSolid* fBasePlateSolid = new G4SubtractionSolid("BasePlateSolid", fBasePlateSolid9, fBasePlateEdgeSolid, fBasePlateEdgeRotationMatrix, G4ThreeVector(fBasePlateOutWidth * 0.5, - fBasePlateOutHeight * 0.5, 0.));

        fBasePlateLogic = new G4LogicalVolume(fBasePlateSolid, fBasePlateMaterial,"BasePlateLogic");
        
        fBasePlateLogic->SetVisAttributes(fBasePlateVisAttribute);

        G4ThreeVector fBasePlatePositionVector = G4ThreeVector(-fCollimatorEnvelopeDisplacement, -(fBasePlateCenterHeight + fBasePlateThickness - fBasePlateOutHeight * 0.5), (fBasePlateWindow1Length - fBasePlateWindow2Length) * 0.5);

        fBasePlatePhysical = new G4PVPlacement(0, fBasePlatePositionVector, fBasePlateLogic, "BasePlatePhysical", fBasePlateEnvelopeLogic, false, 0);
    }
      
    //Base Plate Windows     
   	if(bBasePlateWindow) {
    	G4VisAttributes* fBasePlateWindowVisAttribute = new G4VisAttributes();
    	fBasePlateWindowVisAttribute->SetForceSolid(true);
    	
    	//Window 1
    	G4Tubs* fBasePlateWindow1Solid = new G4Tubs("BasePlateWindow1Solid",
                                            fBasePlateWindowInDiameter * 0.5,
                                            fBasePlateWindowOutDiameter * 0.5,
                                            fBasePlateWindow1Length * 0.5,
                                            0 * CLHEP::deg,
                                            360 * CLHEP::deg);
                                            
        fBasePlateWindow1Logic = new G4LogicalVolume(fBasePlateWindow1Solid, fBasePlateWindowMaterial, "BasePlateWindow1Logic");
        
        fBasePlateWindow1Logic->SetVisAttributes(fBasePlateWindowVisAttribute);
    
    	G4ThreeVector fBasePlateWindow1PositionVector = G4ThreeVector(0., 0., -(fBasePlateOutLength + fBasePlateWindow2Length) * 0.5);
    
   		fBasePlateWindow1Physical = new G4PVPlacement(0, fBasePlateWindow1PositionVector, fBasePlateWindow1Logic, "BasePlateWindow1Physical", fBasePlateEnvelopeLogic, false, 0);   
        
        //Window 2
   	    G4Tubs* fBasePlateWindow2Solid = new G4Tubs("BasePlateWindow2Solid",
                                            fBasePlateWindowInDiameter * 0.5,
                                            fBasePlateWindowOutDiameter * 0.5,
                                            fBasePlateWindow2Length * 0.5,
                                            0 * CLHEP::deg,
                                            360 * CLHEP::deg);                                       
    	
    	fBasePlateWindow2Logic = new G4LogicalVolume(fBasePlateWindow2Solid, fBasePlateWindowMaterial, "BasePlateWindow2Logic");
        fBasePlateWindow2Logic->SetVisAttributes(fBasePlateWindowVisAttribute);
    
    	G4ThreeVector fBasePlateWindow2PositionVector = G4ThreeVector(0., 0., (fBasePlateOutLength + fBasePlateWindow1Length) * 0.5);
    
   		fBasePlateWindow2Physical = new G4PVPlacement(0, fBasePlateWindow2PositionVector, fBasePlateWindow2Logic, "BasePlateWindow2Physical", fBasePlateEnvelopeLogic, false, 0);      	
    }                                        
                                            
    //Transversal Beam    
    if(bTransvBeam) {
    	G4VisAttributes* fTransvBeamVisAttribute = new G4VisAttributes();
    	fTransvBeamVisAttribute->SetForceSolid(true);

    	G4Box* fTransvBeamSolid = new G4Box("TransvBeamSolid", (fBasePlateOutWidth - 2*fBasePlateThickness) * 0.5, fTransvBeamHeight * 0.5, fTransvBeamLength * 0.5);
    	
    	fTransvBeamLogic = new G4LogicalVolume(fTransvBeamSolid, fTransvBeamMaterial, "TransvBeamLogic");
        
        fTransvBeamLogic->SetVisAttributes(fTransvBeamVisAttribute);
        
        G4ThreeVector fTransvBeamPositionVector = G4ThreeVector(-fCollimatorEnvelopeDisplacement, -(fBasePlateCenterHeight - fTransvBeamHeight * 0.5), (fTransvBeamDistance - fBasePlateOutLength * 0.5 + fTransvBeamLength * 0.5));
        
        fTransvBeamPhysical = new G4PVPlacement(0, fTransvBeamPositionVector, fTransvBeamLogic, "fTransvBeamPhysical", fBasePlateEnvelopeLogic, false, 0);  
    }
    
	//definition and positioning of the collimators
	if(bCollimator){
		
		//-------------positioning of 14 rotated collimators inside the collimation chamber----------------
		G4double alpha = atan(fCollimatorEnvelopeDisplacement/(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorPeakHeight - fCollimatorBlockSupThickness - fCollimatorHeight * 0.5)); 
	
		fBeamSlitShift = pow((pow(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorPeakHeight - fCollimatorBlockSupThickness - fCollimatorHeight * 0.5,2) + pow(fCollimatorEnvelopeDisplacement,2)),0.5);
	
		G4double RotCollimAngle = 0.;
		G4double RotCollimInitialAngle = 45.;
    	G4double RotCollimStepAngle = -45.;
    
    	G4int i2 = 0;

		for(int i1=1;i1<15;i1++) {
			
			//-------collimator aperture variation due to slit displacement (manual and/or random)---------
			G4double vDisplacement = fCollDisplMean[i1];
			if(bCollimatorRelativeRandomDisplacement==1){
				//UNIFORM DISTRIBUTION
				vDisplacement = fCollDisplMean[i1] + 2. * (G4UniformRand() - 0.5) * fCollDisplSigma[i1];
			}
			if(bCollimatorRelativeRandomDisplacement==2){
				//GAUSSIAN DISTRIBUTION
				vDisplacement = G4RandGauss::shoot(fCollDisplMean[i1], fCollDisplSigma[i1]);
			}
			fCollimatorDistanceAperture += 2. * vDisplacement;
			
			if (fCollimatorDistanceAperture < 0. * CLHEP::mm) {fCollimatorDistanceAperture = 0. * CLHEP::mm;}
			if (fCollimatorDistanceAperture > 25. * CLHEP::mm) {fCollimatorDistanceAperture = 25. * CLHEP::mm;}
						
			//-----------------------------collimator definition (NICOLA)----------------------------------			
			//Collimator Support and Envelope
			G4Box* fCollimatorSupportEnvelopeSolid = new G4Box("CollimatorSupportEnvelopeSolid", fCollimatorSupportWidth * 0.5, fCollimatorSupportEnvelopeHeight * 0.5, fCollimatorSupportLength * 0.5);
			
			std::stringstream collNumStream;
   			collNumStream << i1;
	
			fCollimatorSupportEnvelopeLogic[i1] = new G4LogicalVolume(fCollimatorSupportEnvelopeSolid, fBeamPipeVacuum, "CollimatorSupportEnvelope" + collNumStream.str());
	
			G4VisAttributes* fCollimatorSupportEnvelopeVisAttribute;
			fCollimatorSupportEnvelopeVisAttribute = new G4VisAttributes(G4Colour(0., 1., 1.));
    		fCollimatorSupportEnvelopeLogic[i1]->SetVisAttributes(fCollimatorSupportEnvelopeVisAttribute);
    		fCollimatorSupportEnvelopeLogic[i1]->SetVisAttributes(G4VisAttributes::GetInvisible());

			G4VisAttributes* fCollimatorSupportVisAttribute;
			fCollimatorSupportVisAttribute = new G4VisAttributes(G4Colour(1.,0.,0.));
			fCollimatorSupportVisAttribute->SetForceSolid(true);

			G4Box* fCollimatorSupportA0Solid = new G4Box("CollimatorSupportA0", fCollimatorSupportWidth * 0.5, fCollimatorSupportHeight * 0.5, fCollimatorSupportLength * 0.5);

			G4Box* fCollimatorSupportA1Solid = new G4Box("CollimatorSupportA1", (fCollimatorSupportWidth - 2. * fCollimatorSupportThickness) * 0.5,  (fCollimatorSupportHeight - fCollimatorSupportThickness) * 0.5, fCollimatorSupportLength * 0.5 + 0.5 * CLHEP::mm);

			G4SubtractionSolid* fCollimatorSupportSolid = new G4SubtractionSolid("CollimatorSupport", fCollimatorSupportA0Solid, fCollimatorSupportA1Solid, 0, G4ThreeVector(0, fCollimatorSupportThickness * 0.5, 0));

			fCollimatorSupportLogic = new G4LogicalVolume(fCollimatorSupportSolid, fCollimatorSupportMaterial, "CollimatorSupport");
	
			fCollimatorSupportLogic->SetVisAttributes(fCollimatorSupportVisAttribute);

			G4ThreeVector fCollimatorSupportPositionVector = G4ThreeVector(0., -(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorSupportHeight * 0.5), 0.);
			fCollimatorSupportPhysical = new G4PVPlacement(0, fCollimatorSupportPositionVector, fCollimatorSupportLogic, "CollimatorSupport", fCollimatorSupportEnvelopeLogic[i1], false, 0); 

	    	//Collimator Apparatus
			//BlockBase
			G4VisAttributes* fCollimatorBlockBaseVisAttribute;
			fCollimatorBlockBaseVisAttribute = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
			fCollimatorBlockBaseVisAttribute->SetForceSolid(true);

			G4Box* fCollimatorBlockBaseSolid1 = new G4Box("CollimatorBlockBase1", fCollimatorBlockBaseWidth * 0.5, fCollimatorBlockBaseHeight * 0.5, fCollimatorBlockBaseLength * 0.5);

			G4Tubs* fCollimatorBlockBaseHoleSolid = new G4Tubs("CollimatorBlockBaseHole", 0., fCollimatorScrewRadius, fCollimatorBlockBaseWidth * 0.5 + 0.5 * CLHEP::mm, 0 * CLHEP::deg, 360 * CLHEP::deg);

			G4RotationMatrix* fCollimatorScrewRotationMatrix = new G4RotationMatrix();
			fCollimatorScrewRotationMatrix->rotateX(0. * CLHEP::deg);
			fCollimatorScrewRotationMatrix->rotateY(90. * CLHEP::deg);
			fCollimatorScrewRotationMatrix->rotateZ(0. * CLHEP::deg);

			G4SubtractionSolid* fCollimatorBlockBaseSolid2 = new G4SubtractionSolid("CollimatorBlockBase2", fCollimatorBlockBaseSolid1, fCollimatorBlockBaseHoleSolid, fCollimatorScrewRotationMatrix, G4ThreeVector(0., fCollimatorScrewUpHeight - fCollimatorBlockBaseHeight * 0.5, 0.));
	
			G4SubtractionSolid* fCollimatorBlockBaseSolid = new G4SubtractionSolid("CollimatorBlockBase", fCollimatorBlockBaseSolid2, fCollimatorBlockBaseHoleSolid, fCollimatorScrewRotationMatrix, G4ThreeVector(0., -(fCollimatorBlockBaseHeight * 0.5 - fCollimatorScrewDownHeight), 0.));

			fCollimatorBlockBaseLogic = new G4LogicalVolume(fCollimatorBlockBaseSolid, fCollimatorBlockBaseMaterial, "CollimatorBlockBase");

			fCollimatorBlockBaseLogic->SetVisAttributes(fCollimatorBlockBaseVisAttribute);

			G4ThreeVector fCollimatorBlockBaseLeftPositionVector = G4ThreeVector((fCollimatorEnvelopeDisplacement + fCollimatorWidth * 0.5 + fCollimatorDistanceAperture * 0.5 + fCollimatorDisplacement), -(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorSupportThickness - fCollimatorBlockBaseHeight * 0.5), 0.);
			fCollimatorBlockBaseLeftPhysical = new G4PVPlacement(0, fCollimatorBlockBaseLeftPositionVector, fCollimatorBlockBaseLogic, "CollimatorBlockBaseLeft", fCollimatorSupportEnvelopeLogic[i1], false, 0);

			G4ThreeVector fCollimatorBlockBaseRightPositionVector = G4ThreeVector(fCollimatorEnvelopeDisplacement - (fCollimatorWidth * 0.5 + fCollimatorDistanceAperture * 0.5 + fCollimatorDisplacement), -(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorSupportThickness - fCollimatorBlockBaseHeight * 0.5), 0.);
			fCollimatorBlockBaseRightPhysical = new G4PVPlacement(0, fCollimatorBlockBaseRightPositionVector, fCollimatorBlockBaseLogic, "CollimatorBlockBaseRight", fCollimatorSupportEnvelopeLogic[i1], false, 0);

			//Collimator
			G4VisAttributes* fCollimatorVisAttribute;
			fCollimatorVisAttribute = new G4VisAttributes(G4Colour(0.5,0.5,0.5));
			fCollimatorVisAttribute->SetForceSolid(true);

			G4Box* fCollimatorSolid = new G4Box("Collimator", fCollimatorWidth * 0.5, fCollimatorHeight * 0.5, fCollimatorLength * 0.5);

			fCollimatorLogic[i1] = new G4LogicalVolume(fCollimatorSolid, fCollimatorMaterial, "Collimator" + collNumStream.str());

			fCollimatorLogic[i1]->SetVisAttributes(fCollimatorVisAttribute);

			G4ThreeVector fCollimatorLeftPositionVector = G4ThreeVector((fCollimatorEnvelopeDisplacement + fCollimatorWidth * 0.5 + fCollimatorDistanceAperture * 0.5), -(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorSupportThickness - fCollimatorHeight * 0.5 - fCollimatorBlockBaseHeight), 0.);
			fCollimatorLeftPhysical = new G4PVPlacement(0, fCollimatorLeftPositionVector, fCollimatorLogic[i1], "CollimatorLeft", fCollimatorSupportEnvelopeLogic[i1], false, 0);

			G4ThreeVector fCollimatorRightPositionVector = G4ThreeVector(fCollimatorEnvelopeDisplacement - (fCollimatorWidth * 0.5 + fCollimatorDistanceAperture * 0.5), -(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorSupportThickness - fCollimatorHeight * 0.5 - fCollimatorBlockBaseHeight), 0.);
			fCollimatorRightPhysical = new G4PVPlacement(0, fCollimatorRightPositionVector, fCollimatorLogic[i1], "CollimatorRight", fCollimatorSupportEnvelopeLogic[i1], false, 0);
	
			//Gears
			G4VisAttributes* fCollimatorGearVisAttribute;
			fCollimatorGearVisAttribute = new G4VisAttributes(G4Colour(1.0,0.72,0.06));
			fCollimatorGearVisAttribute->SetForceSolid(true);

			G4Tubs* fCollimatorGearSolid = new G4Tubs("CollimatorGear", 0., fCollimatorGearRadius, fCollimatorGearThickness * 0.5, 0 * CLHEP::deg, 360 * CLHEP::deg);
	
			fCollimatorGearLogic = new G4LogicalVolume(fCollimatorGearSolid, fCollimatorGearMaterial, "CollimatorGear");

			fCollimatorGearLogic->SetVisAttributes(fCollimatorGearVisAttribute);
	
			G4RotationMatrix* fCollimatorGearRotationMatrix = new G4RotationMatrix();
			fCollimatorGearRotationMatrix->rotateX(0. * CLHEP::deg);
			fCollimatorGearRotationMatrix->rotateY(90. * CLHEP::deg);
			fCollimatorGearRotationMatrix->rotateZ(0. * CLHEP::deg);

			G4ThreeVector fCollimatorGearUpPositionVector = G4ThreeVector(-(fCollimatorSupportWidth * 0.5 - fCollimatorSupportThickness - fCollimatorGearThickness * 0.5), -(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorSupportThickness - fCollimatorBlockBaseHeight + fCollimatorGearRadius), 0.);
			fCollimatorGearUpPhysical = new G4PVPlacement(fCollimatorGearRotationMatrix, fCollimatorGearUpPositionVector, fCollimatorGearLogic, "CollimatorGearUp", fCollimatorSupportEnvelopeLogic[i1], false, 0);

			G4ThreeVector fCollimatorGearDownPositionVector = G4ThreeVector(-(fCollimatorSupportWidth * 0.5 - fCollimatorSupportThickness - fCollimatorGearThickness * 0.5), -(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorSupportThickness - fCollimatorBlockBaseHeight + fCollimatorGearRadius + 2 * fCollimatorGearRadius), 0.);
			fCollimatorGearDownPhysical = new G4PVPlacement(fCollimatorGearRotationMatrix, fCollimatorGearDownPositionVector, fCollimatorGearLogic, "CollimatorGearDown", fCollimatorSupportEnvelopeLogic[i1], false, 0);
	
			//Screws	
			G4VisAttributes* fCollimatorScrewVisAttribute;
			fCollimatorScrewVisAttribute = new G4VisAttributes(G4Colour(1.0,0.72,0.06));
			fCollimatorScrewVisAttribute->SetForceSolid(true);

			G4Tubs* fCollimatorScrewSolid = new G4Tubs("CollimatorScrew", 0., fCollimatorScrewRadius, (fCollimatorSupportWidth - fCollimatorSupportThickness * 2 - fCollimatorGearThickness) * 0.5, 0. * CLHEP::deg, 360. * CLHEP::deg);

			fCollimatorScrewLogic = new G4LogicalVolume(fCollimatorScrewSolid, fCollimatorScrewMaterial, "CollimatorScrew");

			fCollimatorScrewLogic->SetVisAttributes(fCollimatorScrewVisAttribute);

			G4ThreeVector fCollimatorScrewUpPositionVector = G4ThreeVector((fCollimatorSupportWidth * 0.5 - fCollimatorSupportThickness - (fCollimatorSupportWidth - fCollimatorSupportThickness * 2 - fCollimatorGearThickness) * 0.5 ), -(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorSupportThickness - fCollimatorScrewUpHeight), 0.);
		fCollimatorScrewUpPhysical = new G4PVPlacement(fCollimatorScrewRotationMatrix, fCollimatorScrewUpPositionVector, fCollimatorScrewLogic, "CollimatorScrewUp", fCollimatorSupportEnvelopeLogic[i1], false, 0);

			G4ThreeVector fCollimatorScrewDownPositionVector = G4ThreeVector((fCollimatorSupportWidth * 0.5 - fCollimatorSupportThickness - (fCollimatorSupportWidth - fCollimatorSupportThickness * 2 - fCollimatorGearThickness) * 0.5 ), -(fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorSupportThickness - fCollimatorScrewDownHeight), 0.);
			fCollimatorScrewDownPhysical = new G4PVPlacement(fCollimatorScrewRotationMatrix, fCollimatorScrewDownPositionVector, fCollimatorScrewLogic, "CollimatorScrewDown", fCollimatorSupportEnvelopeLogic[i1], false, 0);	
	
			//CollimatorBlock
			G4VisAttributes* fCollimatorBlockVisAttribute;
			fCollimatorBlockVisAttribute = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
			fCollimatorBlockVisAttribute->SetForceSolid(true);

			G4Box* fCollimatorBlockSolidA = new G4Box("CollimatorBlockSolidA", fCollimatorBlockWidth * 0.5, fCollimatorBlockHeight * 0.5, (fCollimatorBlockLateralThickness * 2 + fCollimatorLength) * 0.5);
			G4Box* fCollimatorBlockSolidB = new G4Box("CollimatorBlockSolidB", fCollimatorBlockWidth * 0.5 + 0.5 * CLHEP::mm, (fCollimatorBlockHeight - fCollimatorBlockSupThickness) * 0.5 + 0.5 * CLHEP::mm, fCollimatorLength * 0.5);
			G4Box* fCollimatorBlockGridHoleSolid = new G4Box("CollimatorBlockGridHoleSolid", fCollimatorBlockGridHoleWidth * 0.5, fCollimatorBlockGridHoleHeight * 0.5, (fCollimatorLength + fCollimatorBlockLateralThickness * 2) * 0.5 + 0.5 * CLHEP::mm);

			G4SubtractionSolid* fCollimatorBlockSolid1 = new G4SubtractionSolid("CollimatorBlockSolid1", fCollimatorBlockSolidA, fCollimatorBlockSolidB, 0, G4ThreeVector(0., -fCollimatorBlockSupThickness * 0.5, 0.));
			G4SubtractionSolid* fCollimatorBlockSolid2 = new G4SubtractionSolid("CollimatorBlockSolid2", fCollimatorBlockSolid1, fCollimatorBlockGridHoleSolid, 0, G4ThreeVector(+(fCollimatorBlockWidth * 0.5 - fCollimatorBlockGridHoleHorizDistance - fCollimatorBlockGridHoleWidth * 0.5), fCollimatorBlockGridHoleInfDistance -(fCollimatorBlockHeight * 0.5 - fCollimatorBlockGridHoleHeight * 0.5), 0.));
			G4SubtractionSolid* fCollimatorBlockSolid3 = new G4SubtractionSolid("CollimatorBlockSolid3", fCollimatorBlockSolid2, fCollimatorBlockGridHoleSolid, 0, G4ThreeVector(0., fCollimatorBlockGridHoleInfDistance -(fCollimatorBlockHeight * 0.5 - fCollimatorBlockGridHoleHeight * 0.5), 0.));
			G4SubtractionSolid* fCollimatorBlockSolid = new G4SubtractionSolid("CollimatorBlockSolid", fCollimatorBlockSolid3, fCollimatorBlockGridHoleSolid, 0, G4ThreeVector(-(fCollimatorBlockWidth * 0.5 - fCollimatorBlockGridHoleHorizDistance - fCollimatorBlockGridHoleWidth * 0.5), fCollimatorBlockGridHoleInfDistance -(fCollimatorBlockHeight * 0.5 - fCollimatorBlockGridHoleHeight * 0.5), 0.));

			fCollimatorBlockLogic = new G4LogicalVolume(fCollimatorBlockSolid, fCollimatorBlockMaterial, "CollimatorBlock");
	
			fCollimatorBlockLogic->SetVisAttributes(fCollimatorBlockVisAttribute);

			G4ThreeVector fCollimatorBlockLeftPositionVector = G4ThreeVector((fCollimatorEnvelopeDisplacement + fCollimatorWidth * 0.5 + fCollimatorDistanceAperture * 0.5 + fCollimatorDisplacement + fCollimatorBlockDisplacement - (fCollimatorBlockBaseWidth - fCollimatorBlockWidth) * 0.5), fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorBlockHeight * 0.5 - fCollimatorPeakHeight, 0.);
			fCollimatorBlockLeftPhysical = new G4PVPlacement(0, fCollimatorBlockLeftPositionVector, fCollimatorBlockLogic, "CollimatorBlockLeft", fCollimatorSupportEnvelopeLogic[i1], false, 0);

			G4ThreeVector fCollimatorBlockRightPositionVector = G4ThreeVector(fCollimatorEnvelopeDisplacement - (fCollimatorWidth * 0.5 + fCollimatorDistanceAperture * 0.5 + fCollimatorDisplacement + fCollimatorBlockDisplacement - (fCollimatorBlockBaseWidth - fCollimatorBlockWidth) * 0.5), fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorBlockHeight * 0.5 - fCollimatorPeakHeight, 0.);	
			fCollimatorBlockRightPhysical = new G4PVPlacement(0, fCollimatorBlockRightPositionVector, fCollimatorBlockLogic, "CollimatorBlockRight", fCollimatorSupportEnvelopeLogic[i1], false, 0);

			//Peak
			G4VisAttributes* fCollimatorPeakVisAttribute;
			fCollimatorPeakVisAttribute = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
			fCollimatorPeakVisAttribute->SetForceSolid(true);

			G4Box* fCollimatorPeakSolid = new G4Box("CollimatorPeakSolid", fCollimatorBlockWidth * 0.5, fCollimatorPeakHeight * 0.5, fCollimatorPeakLength * 0.5);

			fCollimatorPeakLogic = new G4LogicalVolume(fCollimatorPeakSolid, fCollimatorPeakMaterial, "CollimatorPeak");
	
			fCollimatorPeakLogic->SetVisAttributes(fCollimatorPeakVisAttribute);

			G4ThreeVector fCollimatorPeakLeftPositionVector = G4ThreeVector((fCollimatorEnvelopeDisplacement + fCollimatorWidth * 0.5 + fCollimatorDistanceAperture * 0.5 + fCollimatorDisplacement + fCollimatorBlockDisplacement - (fCollimatorBlockBaseWidth - fCollimatorBlockWidth) * 0.5), fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorPeakHeight * 0.5, 0.);
		fCollimatorPeakLeftPhysical = new G4PVPlacement(0, fCollimatorPeakLeftPositionVector, fCollimatorPeakLogic, "CollimatorPeakLeft", fCollimatorSupportEnvelopeLogic[i1], false, 0);

			G4ThreeVector fCollimatorPeakRightPositionVector = G4ThreeVector(fCollimatorEnvelopeDisplacement - (fCollimatorWidth * 0.5 + fCollimatorDistanceAperture * 0.5 + fCollimatorDisplacement + fCollimatorBlockDisplacement - (fCollimatorBlockBaseWidth - fCollimatorBlockWidth) * 0.5), fCollimatorSupportEnvelopeHeight * 0.5 - fCollimatorPeakHeight * 0.5, 0.);
			fCollimatorPeakRightPhysical = new G4PVPlacement(0, fCollimatorPeakRightPositionVector, fCollimatorPeakLogic, "CollimatorPeakRight", fCollimatorSupportEnvelopeLogic[i1], false, 0);		
			//---------------------------------------------------------------------------------------------			
	
			//----------------positioning of collimators (Gian) -------------------------------------------
			i2=i2+1;  
    		RotCollimAngle = RotCollimInitialAngle + (i2-1) * RotCollimStepAngle;  	
    		if (i1>1 && fmod(i1-1,4)==0.){ 
    			RotCollimAngle = RotCollimInitialAngle;
    			i2=1;    		
    		}   	    	
    		if (i1 == 13) {RotCollimAngle = 0.;}
			if (i1 == 14) {RotCollimAngle = -90.;}

			G4RotationMatrix* fCollimatorSupportEnvelopeRotationMatrix = new G4RotationMatrix(RotCollimAngle * CLHEP::deg, 0., 0.); 
			
			G4ThreeVector fCorrectionVector = G4ThreeVector(fBeamSlitShift*(sin(alpha)+sin(RotCollimAngle*CLHEP::twopi/360-alpha)), 
															fBeamSlitShift*(cos(alpha)-cos(RotCollimAngle*CLHEP::twopi/360-alpha)), 
															0.);	
			
			G4ThreeVector fCollimatorSupportEnvelopePositionVector = G4ThreeVector(-fCollimatorEnvelopeDisplacement + fCorrectionVector.x(),-(fBasePlateCenterHeight - fCollimatorSupportEnvelopeHeight * 0.5) + fCorrectionVector.y(),(fBasePlateWindow1Length - fBasePlateWindow2Length) * 0.5 - (11.5 - i1) * fBasePlateSmallHoleLength);
		
			if (i1 == 13 || i1 == 14) {
				fCollimatorSupportEnvelopePositionVector +=	G4ThreeVector(0., 0., fBasePlate1213Distance);			
			}	

			fCollimatorSupportEnvelopePhysical = new G4PVPlacement(fCollimatorSupportEnvelopeRotationMatrix, fCollimatorSupportEnvelopePositionVector, fCollimatorSupportEnvelopeLogic[i1], "CollimatorSupportEnvelopePhysical" + collNumStream.str(), fBasePlateEnvelopeLogic, false, 0);
						
		} //end for cycle
	
	} //end definition and positioning of the collimators 	
	//-----------------------------------------------------------------------------------------------------	
	
	//--------------------------------PedestalCollimationChamber and SpaceFab (Nicola)---------------------
	if(bPedestalCollimationChamber){
		
		//PedestalCollimationChamberEnvelope
		G4Box* fPedestalCollimationChamberEnvelopeSolid = new G4Box("PedestalCollimationChamberEnvelopeSolid", fPedestalCollimationChamberEnvelopeWidth * 0.5, fPedestalCollimationChamberEnvelopeHeight * 0.5, fPedestalCollimationChamberEnvelopeLength * 0.5);
	
		G4VisAttributes* fPedestalCollimationChamberEnvelopeVisAttribute;
		fPedestalCollimationChamberEnvelopeVisAttribute = new G4VisAttributes(G4Colour(1., 1., 1.)); // Visualize PedestalCollimationChamberEnvelope
		fPedestalCollimationChamberEnvelopeVisAttribute->SetForceSolid(false);

		fPedestalCollimationChamberEnvelopeLogic = new G4LogicalVolume(fPedestalCollimationChamberEnvelopeSolid, fPedestalCollimationChamberEnvelopeMaterial, "PedestalCollimationChamberEnvelope");

   		fPedestalCollimationChamberEnvelopeLogic->SetVisAttributes(fPedestalCollimationChamberEnvelopeVisAttribute);
   		fPedestalCollimationChamberEnvelopeLogic->SetVisAttributes(G4VisAttributes::GetInvisible());

		G4ThreeVector fPedestalCollimationChamberEnvelopePositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5) - fPedestalCollimationChamberDisplacement, -(fBeamHeight - fPedestalCollimationChamberEnvelopeHeight * 0.5), fCollimatorCenterDistance); //same position along z axis of the chamber, shifted on the right
		fPedestalCollimationChamberEnvelopePhysical = new G4PVPlacement(0, fPedestalCollimationChamberEnvelopePositionVector, fPedestalCollimationChamberEnvelopeLogic, "PedestalCollimationChamberEnvelopePhysical", fWorldLogic, false, 0);

		//Plate1
		G4Box* fPedestalCollimationChamberPlate1Solid = new G4Box("PedestalCollimationChamberPlate1Solid", fPedestalCollimationChamberPlate1Width * 0.5, fPedestalCollimationChamberPlate1Height * 0.5, fPedestalCollimationChamberPlate1Length * 0.5);
	
		G4VisAttributes* fPedestalCollimationChamberPlate1VisAttribute;
		fPedestalCollimationChamberPlate1VisAttribute = new G4VisAttributes();
		fPedestalCollimationChamberPlate1VisAttribute->SetForceSolid(true);

		fPedestalCollimationChamberPlate1Logic = new G4LogicalVolume(fPedestalCollimationChamberPlate1Solid, fPedestalCollimationChamberPlate1Material, "PedestalCollimationChamberPlate1");

		fPedestalCollimationChamberPlate1Logic->SetVisAttributes(fPedestalCollimationChamberPlate1VisAttribute);

		G4ThreeVector fPedestalCollimationChamberPlate1LeftPositionVector = G4ThreeVector((fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), -(fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberPlate1Height * 0.5), 0);
		fPedestalCollimationChamberPlate1LeftPhysical = new G4PVPlacement(0, fPedestalCollimationChamberPlate1LeftPositionVector, fPedestalCollimationChamberPlate1Logic, "fPedestalCollimationChamberPlate1LeftPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

		G4ThreeVector fPedestalCollimationChamberPlate1RightPositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), -(fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberPlate1Height * 0.5), 0);
		fPedestalCollimationChamberPlate1RightPhysical = new G4PVPlacement(0, fPedestalCollimationChamberPlate1RightPositionVector, fPedestalCollimationChamberPlate1Logic, "fPedestalCollimationChamberPlate1RightPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);
	
		//Plate2
		G4Box* fPedestalCollimationChamberPlate2Solid = new G4Box("PedestalCollimationChamberPlate2Solid", fPedestalCollimationChamberPlate2Width * 0.5, fPedestalCollimationChamberPlate2Height * 0.5, fPedestalCollimationChamberPlate2Length * 0.5);
	
		G4VisAttributes* fPedestalCollimationChamberPlate2VisAttribute;
		fPedestalCollimationChamberPlate2VisAttribute = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
		fPedestalCollimationChamberPlate2VisAttribute->SetForceSolid(true);

		fPedestalCollimationChamberPlate2Logic = new G4LogicalVolume(fPedestalCollimationChamberPlate2Solid, fPedestalCollimationChamberPlate2Material, "PedestalCollimationChamberPlate2");

		fPedestalCollimationChamberPlate2Logic->SetVisAttributes(fPedestalCollimationChamberPlate2VisAttribute);

		G4ThreeVector fPedestalCollimationChamberPlate2LeftPositionVector = G4ThreeVector(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5, -(fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberPlate1Height - fPedestalCollimationChamberPlate2Height * 0.5), 0);
		fPedestalCollimationChamberPlate2LeftPhysical = new G4PVPlacement(0, fPedestalCollimationChamberPlate2LeftPositionVector, fPedestalCollimationChamberPlate2Logic, "fPedestalCollimationChamberPlate2LeftPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

		G4ThreeVector fPedestalCollimationChamberPlate2RightPositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), -(fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberPlate1Height - fPedestalCollimationChamberPlate2Height * 0.5), 0);
		fPedestalCollimationChamberPlate2RightPhysical = new G4PVPlacement(0, fPedestalCollimationChamberPlate2RightPositionVector, fPedestalCollimationChamberPlate2Logic, "fPedestalCollimationChamberPlate2RightPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

		//Block
    	G4Box* fPedestalCollimationChamberBlockOutSolid = new G4Box("PedestalCollimationChamberBlockOutSolid", fPedestalCollimationChamberBlockWidth * 0.5, fPedestalCollimationChamberBlockHeight * 0.5, fPedestalCollimationChamberBlockLength * 0.5);
    	
    	G4Box* fPedestalCollimationChamberBlockInSolid = new G4Box("PedestalCollimationChamberBlockInSolid", (fPedestalCollimationChamberBlockWidth - fPedestalBlockThickness) * 0.5, (fPedestalCollimationChamberBlockHeight - fPedestalBlockThickness) * 0.5, (fPedestalCollimationChamberBlockLength - fPedestalBlockThickness) * 0.5);

		G4SubtractionSolid* fPedestalCollimationChamberBlockSolid = new G4SubtractionSolid("PedestalCollimationChamberBlockSolid", fPedestalCollimationChamberBlockOutSolid, fPedestalCollimationChamberBlockInSolid);
		
		fPedestalCollimationChamberBlockInsideLogic = new G4LogicalVolume(fPedestalCollimationChamberBlockInSolid, fPedestalCollimationChamberBlockInsideMaterial, "fPedestalCollimationChamberBlockInside");
		
        G4VisAttributes* fPedestalCollimationChamberBlockVisAttribute;
        fPedestalCollimationChamberBlockVisAttribute = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
        fPedestalCollimationChamberBlockVisAttribute->SetForceSolid(true);

        fPedestalCollimationChamberBlockLogic = new G4LogicalVolume(fPedestalCollimationChamberBlockSolid, fPedestalCollimationChamberBlockMaterial, "PedestalCollimationChamberBlock");

        fPedestalCollimationChamberBlockLogic->SetVisAttributes(fPedestalCollimationChamberBlockVisAttribute);

        G4ThreeVector fPedestalCollimationChamberBlockLeftPositionVector = G4ThreeVector(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5, -(fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberBlockHeight * 0.5 - fPedestalCollimationChamberPlate1Height - fPedestalCollimationChamberPlate2Height), 0);
        fPedestalCollimationChamberBlockLeftPhysical = new G4PVPlacement(0, fPedestalCollimationChamberBlockLeftPositionVector, fPedestalCollimationChamberBlockLogic, "fPedestalCollimationChamberBlockLeftPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberBlockRightPositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), -(fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberBlockHeight * 0.5 - fPedestalCollimationChamberPlate1Height - fPedestalCollimationChamberPlate2Height), 0);
        fPedestalCollimationChamberBlockRightPhysical = new G4PVPlacement(0, fPedestalCollimationChamberBlockRightPositionVector, fPedestalCollimationChamberBlockLogic, "fPedestalCollimationChamberBlockRightPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);
        
        fPedestalCollimationChamberBlockLeftInsidePhysical = new G4PVPlacement(0, fPedestalCollimationChamberBlockLeftPositionVector, fPedestalCollimationChamberBlockInsideLogic, "fPedestalCollimationChamberBlockLeftInsidePhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);        
        fPedestalCollimationChamberBlockRightInsidePhysical = new G4PVPlacement(0, fPedestalCollimationChamberBlockRightPositionVector, fPedestalCollimationChamberBlockInsideLogic, "fPedestalCollimationChamberBlockRightInsidePhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        //Plate3
		G4Box* fPedestalCollimationChamberPlate3Solid = new G4Box("PedestalCollimationChamberPlate3Solid", fPedestalCollimationChamberPlate3Width * 0.5, fPedestalCollimationChamberPlate3Height * 0.5, fPedestalCollimationChamberPlate3Length * 0.5);
	
		G4VisAttributes* fPedestalCollimationChamberPlate3VisAttribute;
		fPedestalCollimationChamberPlate3VisAttribute = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
		fPedestalCollimationChamberPlate3VisAttribute->SetForceSolid(true);

		fPedestalCollimationChamberPlate3Logic = new G4LogicalVolume(fPedestalCollimationChamberPlate3Solid, fPedestalCollimationChamberPlate3Material, "PedestalCollimationChamberPlate3");

		fPedestalCollimationChamberPlate3Logic->SetVisAttributes(fPedestalCollimationChamberPlate3VisAttribute);

        G4ThreeVector fPedestalCollimationChamberPlate3LeftPositionVector = G4ThreeVector(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberPlate3Height * 0.5 - fPedestalCollimationChamberStrutHeight - fPedestalCollimationChamberExternalScrewHeight -(fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), 0);
        fPedestalCollimationChamberPlate3LeftPhysical = new G4PVPlacement(0, fPedestalCollimationChamberPlate3LeftPositionVector, fPedestalCollimationChamberPlate3Logic, "PedestalCollimationChamberPlate3LeftPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

		G4ThreeVector fPedestalCollimationChamberPlate3RightPositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberPlate3Height * 0.5 - fPedestalCollimationChamberStrutHeight - fPedestalCollimationChamberExternalScrewHeight -(fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), 0);
        fPedestalCollimationChamberPlate3RightPhysical = new G4PVPlacement(0, fPedestalCollimationChamberPlate3RightPositionVector, fPedestalCollimationChamberPlate3Logic, "PedestalCollimationChamberPlate3RightPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        //4 ExternalScrew
        G4VisAttributes* fPedestalCollimationChamberExternalScrewVisAttribute;
        fPedestalCollimationChamberExternalScrewVisAttribute = new G4VisAttributes(G4Colour(1.0,0.72,0.06));
        fPedestalCollimationChamberExternalScrewVisAttribute->SetForceSolid(true);

        G4Tubs* fPedestalCollimationChamberExternalScrewSolid = new G4Tubs("PedestalCollimationChamberExternalScrew", 0., fPedestalCollimationChamberExternalScrewRadius, fPedestalCollimationChamberExternalScrewHeight * 0.5, 0 * CLHEP::deg, 360 * CLHEP::deg);

        fPedestalCollimationChamberExternalScrewLogic = new G4LogicalVolume(fPedestalCollimationChamberExternalScrewSolid, fPedestalCollimationChamberExternalScrewMaterial, "PedestalCollimationChamberExternalScrew");

        fPedestalCollimationChamberExternalScrewLogic->SetVisAttributes(fPedestalCollimationChamberExternalScrewVisAttribute);

        G4RotationMatrix* fPedestalCollimationChamberExternalScrewRotationMatrix = new G4RotationMatrix();
        fPedestalCollimationChamberExternalScrewRotationMatrix->rotateX(90. * CLHEP::deg);
        fPedestalCollimationChamberExternalScrewRotationMatrix->rotateY(0. * CLHEP::deg);
        fPedestalCollimationChamberExternalScrewRotationMatrix->rotateZ(0. * CLHEP::deg);

        G4ThreeVector fPedestalCollimationChamberExternalScrewLeftFrontPositionVector = G4ThreeVector(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberExternalScrewHeight * 0.5 - fPedestalCollimationChamberStrutHeight -(fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), -(fPedestalCollimationChamberEnvelopeLength * 0.5 - fPedestalCollimationChamberStrutShiftZ - fPedestalCollimationChamberStrutLength * 0.5));
        fPedestalCollimationChamberExternalScrewLeftFrontPhysical = new G4PVPlacement(fPedestalCollimationChamberExternalScrewRotationMatrix, fPedestalCollimationChamberExternalScrewLeftFrontPositionVector, fPedestalCollimationChamberExternalScrewLogic, "PedestalCollimationChamberExternalScrewLeftFront", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberExternalScrewLeftRearPositionVector = G4ThreeVector(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5, -(fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight) + fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberExternalScrewHeight * 0.5 - fPedestalCollimationChamberStrutHeight, fPedestalCollimationChamberEnvelopeLength * 0.5 - fPedestalCollimationChamberStrutShiftZ - fPedestalCollimationChamberStrutLength * 0.5);
        fPedestalCollimationChamberExternalScrewLeftRearPhysical = new G4PVPlacement(fPedestalCollimationChamberExternalScrewRotationMatrix, fPedestalCollimationChamberExternalScrewLeftRearPositionVector, fPedestalCollimationChamberExternalScrewLogic, "PedestalCollimationChamberExternalScrewLeftRear", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberExternalScrewRightFrontPositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberExternalScrewHeight * 0.5 - fPedestalCollimationChamberStrutHeight - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), -(fPedestalCollimationChamberEnvelopeLength * 0.5 - fPedestalCollimationChamberStrutShiftZ - fPedestalCollimationChamberStrutLength * 0.5));
        fPedestalCollimationChamberExternalScrewRightFrontPhysical = new G4PVPlacement(fPedestalCollimationChamberExternalScrewRotationMatrix, fPedestalCollimationChamberExternalScrewRightFrontPositionVector, fPedestalCollimationChamberExternalScrewLogic, "PedestalCollimationChamberExternalScrewRightFront", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberExternalScrewRightRearPositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberExternalScrewHeight * 0.5 - fPedestalCollimationChamberStrutHeight - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), fPedestalCollimationChamberEnvelopeLength * 0.5 - fPedestalCollimationChamberStrutShiftZ - fPedestalCollimationChamberStrutLength * 0.5);
        fPedestalCollimationChamberExternalScrewRightRearPhysical = new G4PVPlacement(fPedestalCollimationChamberExternalScrewRotationMatrix, fPedestalCollimationChamberExternalScrewRightRearPositionVector, fPedestalCollimationChamberExternalScrewLogic, "PedestalCollimationChamberExternalScrewRightRear", fPedestalCollimationChamberEnvelopeLogic, false, 0);

   		//2 CentralScrew
        G4VisAttributes* fPedestalCollimationChamberCentralScrewVisAttribute;
        fPedestalCollimationChamberCentralScrewVisAttribute = new G4VisAttributes(G4Colour(1.0,0.72,0.06));
        fPedestalCollimationChamberCentralScrewVisAttribute->SetForceSolid(true);

        G4Tubs* fPedestalCollimationChamberCentralScrewSolid = new G4Tubs("PedestalCollimationChamberCentralScrew", 0., fPedestalCollimationChamberCentralScrewRadius, fPedestalCollimationChamberCentralScrewHeight * 0.5, 0 * CLHEP::deg, 360 * CLHEP::deg);

        fPedestalCollimationChamberCentralScrewLogic = new G4LogicalVolume(fPedestalCollimationChamberCentralScrewSolid, fPedestalCollimationChamberCentralScrewMaterial, "PedestalCollimationChamberCentralScrew");

        fPedestalCollimationChamberCentralScrewLogic->SetVisAttributes(fPedestalCollimationChamberCentralScrewVisAttribute);

        G4RotationMatrix* fPedestalCollimationChamberCentralScrewRotationMatrix = new G4RotationMatrix();
        fPedestalCollimationChamberCentralScrewRotationMatrix->rotateX(90. * CLHEP::deg);
        fPedestalCollimationChamberCentralScrewRotationMatrix->rotateY(0. * CLHEP::deg);
        fPedestalCollimationChamberCentralScrewRotationMatrix->rotateZ(0. * CLHEP::deg);

        G4ThreeVector fPedestalCollimationChamberCentralScrewLeftPositionVector = G4ThreeVector(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberCentralScrewHeight * 0.5 - fPedestalCollimationChamberLongitudinalGirder1Height - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), 0.);
        fPedestalCollimationChamberCentralScrewLeftPhysical = new G4PVPlacement(fPedestalCollimationChamberCentralScrewRotationMatrix, fPedestalCollimationChamberCentralScrewLeftPositionVector, fPedestalCollimationChamberCentralScrewLogic, "PedestalCollimationChamberCentralScrewLeft", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberCentralScrewRightPositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberCentralScrewHeight * 0.5 - fPedestalCollimationChamberLongitudinalGirder1Height - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), 0.);
        fPedestalCollimationChamberCentralScrewRightPhysical = new G4PVPlacement(fPedestalCollimationChamberCentralScrewRotationMatrix, fPedestalCollimationChamberCentralScrewRightPositionVector, fPedestalCollimationChamberCentralScrewLogic, "PedestalCollimationChamberCentralScrewRight", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        //4 Struts
        G4VisAttributes* fPedestalCollimationChamberStrutVisAttribute = new G4VisAttributes();
        fPedestalCollimationChamberStrutVisAttribute->SetForceSolid(true);

        G4Box* fPedestalCollimationChamberStrutOutSolid = new G4Box("PedestalCollimationChamberStrut", fPedestalCollimationChamberStrutWidth * 0.5, fPedestalCollimationChamberStrutHeight * 0.5, fPedestalCollimationChamberStrutLength * 0.5);

        G4Box* fPedestalCollimationChamberStrutExternalHoleSolid = new G4Box("PedestalCollimationChamberStrutExternalHoleSolid", fPedestalCollimationChamberStrutExternalHoleWidth * 0.5, fPedestalCollimationChamberStrutExternalHoleHeight * 0.5, fPedestalCollimationChamberStrutExternalHoleLength * 0.5);

        G4Box* fPedestalCollimationChamberStrutInternalHoleSolid = new G4Box("PedestalCollimationChamberStrutInternalHoleSolid", fPedestalCollimationChamberStrutInternalHoleWidth * 0.5, fPedestalCollimationChamberStrutInternalHoleHeight * 0.5, fPedestalCollimationChamberStrutInternalHoleLength * 0.5);

        G4SubtractionSolid* fPedestalCollimationChamberStrutSolid1 = new G4SubtractionSolid("PedestalCollimationChamberStrutSolid1", fPedestalCollimationChamberStrutOutSolid, fPedestalCollimationChamberStrutExternalHoleSolid, 0, G4ThreeVector(0., (fPedestalCollimationChamberStrutHeight * 0.5 - fPedestalCollimationChamberStrutExternalHoleHeight * 0.5), -(fPedestalCollimationChamberStrutLength * 0.5 - fPedestalCollimationChamberStrutExternalHoleLength * 0.5)));

        G4SubtractionSolid* fPedestalCollimationChamberStrutSolid = new G4SubtractionSolid("PedestalCollimationChamberStrutSolid", fPedestalCollimationChamberStrutSolid1, fPedestalCollimationChamberStrutInternalHoleSolid, 0, G4ThreeVector(0., -(fPedestalCollimationChamberStrutHeight * 0.5 - fPedestalCollimationChamberStrutInternalHoleHeight * 0.5 - fPedestalCollimationChamberStrutInternalHoleShiftY), fPedestalCollimationChamberStrutLength * 0.5 - fPedestalCollimationChamberStrutInternalHoleLength * 0.5 + 10. * CLHEP::mm));

        fPedestalCollimationChamberStrutLogic = new G4LogicalVolume(fPedestalCollimationChamberStrutSolid, fPedestalCollimationChamberStrutMaterial, "PedestalCollimationChamberStrutLogic");
        fPedestalCollimationChamberStrutLogic->SetVisAttributes(fPedestalCollimationChamberStrutVisAttribute);

        G4ThreeVector fPedestalCollimationChamberStrutLeftFrontPositionVector = G4ThreeVector(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberStrutHeight * 0.5 - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), -(fPedestalCollimationChamberEnvelopeLength * 0.5 - fPedestalCollimationChamberStrutShiftZ - fPedestalCollimationChamberStrutLength * 0.5));
        fPedestalCollimationChamberStrutLeftFrontPhysical = new G4PVPlacement(0, fPedestalCollimationChamberStrutLeftFrontPositionVector, fPedestalCollimationChamberStrutLogic, "fPedestalCollimationChamberStrutLeftFrontPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberStrutRightFrontPositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberStrutHeight * 0.5 - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), -(fPedestalCollimationChamberEnvelopeLength * 0.5 - fPedestalCollimationChamberStrutShiftZ - fPedestalCollimationChamberStrutLength * 0.5));
        fPedestalCollimationChamberStrutRightFrontPhysical = new G4PVPlacement(0, fPedestalCollimationChamberStrutRightFrontPositionVector, fPedestalCollimationChamberStrutLogic, "fPedestalCollimationChamberStrutRightFrontPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4RotationMatrix* fPedestalCollimationChamberStrutRotationMatrix = new G4RotationMatrix();
        fPedestalCollimationChamberStrutRotationMatrix->rotateX(0. * CLHEP::deg);
        fPedestalCollimationChamberStrutRotationMatrix->rotateY(180. * CLHEP::deg);
        fPedestalCollimationChamberStrutRotationMatrix->rotateZ(0. * CLHEP::deg);

        G4ThreeVector fPedestalCollimationChamberStrutLeftRearPositionVector = G4ThreeVector(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberStrutHeight * 0.5 - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), (fPedestalCollimationChamberEnvelopeLength * 0.5 - fPedestalCollimationChamberStrutShiftZ - fPedestalCollimationChamberStrutLength * 0.5));
        fPedestalCollimationChamberStrutLeftRearPhysical = new G4PVPlacement(fPedestalCollimationChamberStrutRotationMatrix, fPedestalCollimationChamberStrutLeftRearPositionVector, fPedestalCollimationChamberStrutLogic, "fPedestalCollimationChamberStrutLeftRearPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberStrutRightRearPositionVector = G4ThreeVector(-(fPedestalCollimationChamberEnvelopeWidth * 0.5 - fPedestalCollimationChamberPlate1Width * 0.5), fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberStrutHeight * 0.5 - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), (fPedestalCollimationChamberEnvelopeLength * 0.5 - fPedestalCollimationChamberStrutShiftZ - fPedestalCollimationChamberStrutLength * 0.5));
        fPedestalCollimationChamberStrutRightRearPhysical = new G4PVPlacement(fPedestalCollimationChamberStrutRotationMatrix, fPedestalCollimationChamberStrutRightRearPositionVector, fPedestalCollimationChamberStrutLogic, "fPedestalCollimationChamberStrutRightRearPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        //2 Transversal Girders
        G4VisAttributes* fPedestalCollimationChamberTransversalGirderVisAttribute = new G4VisAttributes(G4Colour(0., 0., 1.));
        fPedestalCollimationChamberTransversalGirderVisAttribute->SetForceSolid(true);

        G4Box* fPedestalCollimationChamberTransversalGirder1Solid = new G4Box("PedestalCollimationChamberTransversalGirder1Solid", fPedestalCollimationChamberTransversalGirder1Width * 0.5, fPedestalCollimationChamberTransversalGirder1Height * 0.5, fPedestalCollimationChamberTransversalGirder1Length * 0.5);

        G4Box* fPedestalCollimationChamberTransversalGirder2Solid = new G4Box("PedestalCollimationChamberTransversalGirder2Solid", fPedestalCollimationChamberTransversalGirder2Width * 0.5 + 1. * CLHEP::mm, fPedestalCollimationChamberTransversalGirder2Height * 0.5, fPedestalCollimationChamberTransversalGirder2Length * 0.5);

        G4Box* fPedestalCollimationChamberTransversalGirder3Solid = new G4Box("PedestalCollimationChamberTransversalGirder3Solid", fPedestalCollimationChamberTransversalGirder3Width * 0.5, fPedestalCollimationChamberTransversalGirder3Height * 0.5, fPedestalCollimationChamberTransversalGirder3Length * 0.5);

        G4Box* fPedestalCollimationChamberTransversalGirder4Solid = new G4Box("PedestalCollimationChamberTransversalGirder4Solid", fPedestalCollimationChamberTransversalGirder4Width * 0.5, fPedestalCollimationChamberTransversalGirder4Height * 0.5, fPedestalCollimationChamberTransversalGirder4Length * 0.5);

        G4SubtractionSolid* fPedestalCollimationChamberTransversalGirderOutSolid = new G4SubtractionSolid("PedestalCollimationChamberTransversalGirderOutSolid", fPedestalCollimationChamberTransversalGirder1Solid, fPedestalCollimationChamberTransversalGirder2Solid, 0, G4ThreeVector(0., 0., 0.));

        G4SubtractionSolid* fPedestalCollimationChamberTransversalGirderInSolid = new G4SubtractionSolid("PedestalCollimationChamberTransversalGirderInSolid", fPedestalCollimationChamberTransversalGirder3Solid, fPedestalCollimationChamberTransversalGirder4Solid, 0, G4ThreeVector(0., 0., 0.));

        G4RotationMatrix* fPedestalCollimationChamberTransversalGirderRotationMatrix = new G4RotationMatrix();
        fPedestalCollimationChamberTransversalGirderRotationMatrix->rotateX(45. * CLHEP::deg);
        fPedestalCollimationChamberTransversalGirderRotationMatrix->rotateY(0. * CLHEP::deg);
        fPedestalCollimationChamberTransversalGirderRotationMatrix->rotateZ(0. * CLHEP::deg);

        G4UnionSolid* fPedestalCollimationChamberTransversalGirderSolid = new G4UnionSolid("fPedestalCollimationChamberTransversalGirderSolid", fPedestalCollimationChamberTransversalGirderOutSolid, fPedestalCollimationChamberTransversalGirderInSolid, fPedestalCollimationChamberTransversalGirderRotationMatrix, G4ThreeVector(0., 0., 0.));

        fPedestalCollimationChamberTransversalGirderLogic = new G4LogicalVolume(fPedestalCollimationChamberTransversalGirderSolid, fPedestalCollimationChamberTransversalGirderMaterial, "PedestalCollimationChamberTransversalGirder");
        fPedestalCollimationChamberTransversalGirderLogic->SetVisAttributes(fPedestalCollimationChamberTransversalGirderVisAttribute);

        G4ThreeVector fPedestalCollimationChamberTransversalGirderFrontPositionVector = G4ThreeVector(fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberTransversalGirder1Height * 0.5 - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), -(fPedestalCollimationChamberLongitudinalGirder1Length * 0.5 + fPedestalCollimationChamberTransversalGirder1Length * 0.5));
        fPedestalCollimationChamberTransversalGirderFrontPhysical = new G4PVPlacement(0, fPedestalCollimationChamberTransversalGirderFrontPositionVector, fPedestalCollimationChamberTransversalGirderLogic, "fPedestalCollimationChamberTransversalGirderFrontPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberTransversalGirderRearPositionVector = G4ThreeVector(fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberTransversalGirder1Height * 0.5 - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), fPedestalCollimationChamberLongitudinalGirder1Length * 0.5 + fPedestalCollimationChamberTransversalGirder1Length * 0.5);
        fPedestalCollimationChamberTransversalGirderRearPhysical = new G4PVPlacement(0, fPedestalCollimationChamberTransversalGirderRearPositionVector, fPedestalCollimationChamberTransversalGirderLogic, "fPedestalCollimationChamberTransversalGirderRearPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        //3 Longitudinal Girders
        G4VisAttributes* fPedestalCollimationChamberLongitudinalGirderVisAttribute = new G4VisAttributes(G4Colour(0., 0., 1.));
        fPedestalCollimationChamberLongitudinalGirderVisAttribute->SetForceSolid(true);

        G4Box* fPedestalCollimationChamberLongitudinalGirder1Solid = new G4Box("PedestalCollimationChamberTransversalGirder1Solid", fPedestalCollimationChamberLongitudinalGirder1Width * 0.5, fPedestalCollimationChamberLongitudinalGirder1Height * 0.5, fPedestalCollimationChamberLongitudinalGirder1Length * 0.5);

        G4Box* fPedestalCollimationChamberLongitudinalGirder2Solid = new G4Box("PedestalCollimationChamberTransversalGirder2Solid", fPedestalCollimationChamberLongitudinalGirder2Width * 0.5, fPedestalCollimationChamberLongitudinalGirder2Height * 0.5, fPedestalCollimationChamberLongitudinalGirder2Length * 0.5 + 1. * CLHEP::mm);

        G4Box* fPedestalCollimationChamberLongitudinalGirder3Solid = new G4Box("PedestalCollimationChamberTransversalGirder3Solid", fPedestalCollimationChamberLongitudinalGirder3Width * 0.5, fPedestalCollimationChamberLongitudinalGirder3Height * 0.5, fPedestalCollimationChamberLongitudinalGirder3Length * 0.5);

        G4Box* fPedestalCollimationChamberLongitudinalGirder4Solid = new G4Box("PedestalCollimationChamberTransversalGirder4Solid", fPedestalCollimationChamberLongitudinalGirder4Width * 0.5, fPedestalCollimationChamberLongitudinalGirder4Height * 0.5, fPedestalCollimationChamberLongitudinalGirder4Length * 0.5);

        G4SubtractionSolid* fPedestalCollimationChamberLongitudinalGirderOutSolid = new G4SubtractionSolid("PedestalCollimationChamberTransversalGirderOutSolid", fPedestalCollimationChamberLongitudinalGirder1Solid, fPedestalCollimationChamberLongitudinalGirder2Solid, 0, G4ThreeVector(0., 0., 0.));

        G4SubtractionSolid* fPedestalCollimationChamberLongitudinalGirderInSolid = new G4SubtractionSolid("PedestalCollimationChamberTransversalGirderInSolid", fPedestalCollimationChamberLongitudinalGirder3Solid, fPedestalCollimationChamberLongitudinalGirder4Solid, 0, G4ThreeVector(0., 0., 0.));

        G4RotationMatrix* fPedestalCollimationChamberLongitudinalGirderRotationMatrix = new G4RotationMatrix();
        fPedestalCollimationChamberLongitudinalGirderRotationMatrix->rotateX(0. * CLHEP::deg);
        fPedestalCollimationChamberLongitudinalGirderRotationMatrix->rotateY(0. * CLHEP::deg);
        fPedestalCollimationChamberLongitudinalGirderRotationMatrix->rotateZ(45. * CLHEP::deg);

        G4UnionSolid* fPedestalCollimationChamberLongitudinalGirderSolid = new G4UnionSolid("fPedestalCollimationChamberLongitudinalGirderSolid", fPedestalCollimationChamberLongitudinalGirderOutSolid, fPedestalCollimationChamberLongitudinalGirderInSolid, fPedestalCollimationChamberLongitudinalGirderRotationMatrix, G4ThreeVector(0., 0., 0.));

        fPedestalCollimationChamberLongitudinalGirderLogic = new G4LogicalVolume(fPedestalCollimationChamberLongitudinalGirderSolid, fPedestalCollimationChamberLongitudinalGirderMaterial, "PedestalCollimationChamberTransversalGirder");
        fPedestalCollimationChamberLongitudinalGirderLogic->SetVisAttributes(fPedestalCollimationChamberLongitudinalGirderVisAttribute);

        G4ThreeVector fPedestalCollimationChamberLongitudinalGirderLeftPositionVector = G4ThreeVector(fPedestalCollimationChamberTransversalGirder1Width * 0.5 - fPedestalCollimationChamberLongitudinalGirder1Width * 0.5 + fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberLongitudinalGirder1Height * 0.5 - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), 0.);
        fPedestalCollimationChamberLongitudinalGirderLeftPhysical = new G4PVPlacement(0, fPedestalCollimationChamberLongitudinalGirderLeftPositionVector, fPedestalCollimationChamberLongitudinalGirderLogic, "fPedestalCollimationChamberLongitudinalGirderLeftPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberLongitudinalGirderCentralPositionVector = G4ThreeVector(fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberLongitudinalGirder1Height * 0.5 - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), 0.);
        fPedestalCollimationChamberLongitudinalGirderCentralPhysical = new G4PVPlacement(0, fPedestalCollimationChamberLongitudinalGirderCentralPositionVector, fPedestalCollimationChamberLongitudinalGirderLogic, "fPedestalCollimationChamberLongitudinalGirderCentralPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

        G4ThreeVector fPedestalCollimationChamberLongitudinalGirderRightPositionVector = G4ThreeVector(-(fPedestalCollimationChamberTransversalGirder1Width * 0.5 - fPedestalCollimationChamberLongitudinalGirder1Width * 0.5) + fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fPedestalCollimationChamberLongitudinalGirder1Height * 0.5 - (fSpaceFabPlateHeight + fSpaceFabBinaryHeight + fSpaceFabCartDownHeight + fSpaceFabCartUpHeight), 0.);
        fPedestalCollimationChamberLongitudinalGirderRightPhysical = new G4PVPlacement(0, fPedestalCollimationChamberLongitudinalGirderRightPositionVector, fPedestalCollimationChamberLongitudinalGirderLogic, "fPedestalCollimationChamberLongitudinalGirderRightPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

	 	//SpaceFabPlate	
		G4VisAttributes* fSpaceFabPlateVisAttribute = new G4VisAttributes();
		fSpaceFabPlateVisAttribute->SetForceSolid(true);	
	
		G4Box* fSpaceFabPlateOutSolid = new G4Box("SpaceFabPlateOutSolid", fSpaceFabPlateWidth * 0.5, fSpaceFabPlateHeight * 0.5, fSpaceFabPlateLength * 0.5);
	
		G4Box* fSpaceFabPlateHoleSolid = new G4Box("SpaceFabPlateHoleSolid", fSpaceFabPlateHoleWidth * 0.5, fSpaceFabPlateHoleHeight * 0.5, fSpaceFabPlateHoleLength * 0.5);

		G4SubtractionSolid* fSpaceFabPlate1Solid = new G4SubtractionSolid("SpaceFabPlate1Solid", fSpaceFabPlateOutSolid, fSpaceFabPlateHoleSolid, 0, G4ThreeVector(-(fSpaceFabPlateWidth * 0.5 - fSpaceFabPlateHoleWidth * 0.5), 0., -(fSpaceFabPlateLength * 0.5 - fSpaceFabPlateHoleLength * 0.5)));

		G4SubtractionSolid* fSpaceFabPlateSolid = new G4SubtractionSolid("SpaceFabPlateSolid", fSpaceFabPlate1Solid, fSpaceFabPlateHoleSolid, 0, G4ThreeVector(-(fSpaceFabPlateWidth * 0.5 - fSpaceFabPlateHoleWidth * 0.5), 0., fSpaceFabPlateLength * 0.5 - fSpaceFabPlateHoleLength * 0.5));
	
		fSpaceFabPlateLogic = new G4LogicalVolume(fSpaceFabPlateSolid, fSpaceFabPlateMaterial, "SpaceFabPlate");
		fSpaceFabPlateLogic->SetVisAttributes(fSpaceFabPlateVisAttribute);
	
		G4ThreeVector fSpaceFabPlatePositionVector = G4ThreeVector(fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabPlateHeight * 0.5 - fSpaceFabBinaryHeight - fSpaceFabCartDownHeight - fSpaceFabCartUpHeight, 0.);	
		fSpaceFabPlatePhysical = new G4PVPlacement(0, fSpaceFabPlatePositionVector, fSpaceFabPlateLogic, "fSpaceFabPlatePhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);	
	
	   	//SpaceFabBinary
		G4VisAttributes* fSpaceFabBinaryVisAttribute = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
		fSpaceFabBinaryVisAttribute->SetForceSolid(true);
	
		G4Box* fSpaceFabBinarySolid = new G4Box("SpaceFabBinarySolid", fSpaceFabBinaryWidth * 0.5, fSpaceFabBinaryHeight * 0.5, fSpaceFabBinaryLength * 0.5);

		fSpaceFabBinaryLogic = new G4LogicalVolume(fSpaceFabBinarySolid, fSpaceFabBinaryMaterial, "SpaceFabBinary");
		fSpaceFabBinaryLogic->SetVisAttributes(fSpaceFabBinaryVisAttribute);

		G4ThreeVector fSpaceFabBinaryFrontPositionVector = G4ThreeVector(fPedestalCollimationChamberTransversalGirderShiftX + fSpaceFabPlateWidth * 0.5 - fSpaceFabBinaryWidth * 0.5 - fSpaceFabBinaryShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabBinaryHeight * 0.5 - fSpaceFabCartDownHeight - fSpaceFabCartUpHeight, -(fSpaceFabPlateLength * 0.5 - fSpaceFabBinaryLength * 0.5 - fSpaceFabBinaryShiftZ));
		fSpaceFabBinaryFrontPhysical = new G4PVPlacement(0, fSpaceFabBinaryFrontPositionVector, fSpaceFabBinaryLogic, "fSpaceFabBinaryFrontPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);
	
		G4ThreeVector fSpaceFabBinaryCentralPositionVector = G4ThreeVector(-(fSpaceFabPlateWidth * 0.5 - fSpaceFabBinaryWidth * 0.5) + fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabBinaryHeight * 0.5 - fSpaceFabCartDownHeight - fSpaceFabCartUpHeight, 0.);
		fSpaceFabBinaryCentralPhysical = new G4PVPlacement(0, fSpaceFabBinaryCentralPositionVector, fSpaceFabBinaryLogic, "fSpaceFabBinaryCentralPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);	

		G4ThreeVector fSpaceFabBinaryRearPositionVector = G4ThreeVector(fPedestalCollimationChamberTransversalGirderShiftX + fSpaceFabPlateWidth * 0.5 - fSpaceFabBinaryWidth * 0.5 - fSpaceFabBinaryShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabBinaryHeight * 0.5 - fSpaceFabCartDownHeight - fSpaceFabCartUpHeight, fSpaceFabPlateLength * 0.5 - fSpaceFabBinaryLength * 0.5 - fSpaceFabBinaryShiftZ);
		fSpaceFabBinaryRearPhysical = new G4PVPlacement(0, fSpaceFabBinaryRearPositionVector, fSpaceFabBinaryLogic, "fSpaceFabBinaryRearPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);
	
	   	//SpaceFabCartDown
		G4VisAttributes* fSpaceFabCartDownVisAttribute = new G4VisAttributes(G4Colour(0.6, 0.4, 0.8));
		fSpaceFabCartDownVisAttribute->SetForceSolid(true);

		G4Box* fSpaceFabCartDownSolid = new G4Box("SpaceFabCartDownSolid", fSpaceFabCartDownWidth * 0.5, fSpaceFabCartDownHeight * 0.5, fSpaceFabCartDownLength * 0.5);
	
		fSpaceFabCartDownLogic = new G4LogicalVolume(fSpaceFabCartDownSolid, fSpaceFabCartDownMaterial, "SpaceFabCartDown");
		fSpaceFabCartDownLogic->SetVisAttributes(fSpaceFabCartDownVisAttribute);

		G4ThreeVector fSpaceFabCartDownFrontPositionVector = G4ThreeVector(fSpaceFabPlateWidth * 0.5 - fSpaceFabCartDownWidth * 0.5 - fSpaceFabCartFrontRearShiftX + fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabCartDownHeight * 0.5 - fSpaceFabCartUpHeight, -(fSpaceFabPlateLength * 0.5 - fSpaceFabBinaryLength * 0.5 - fSpaceFabBinaryShiftZ));
		fSpaceFabCartDownFrontPhysical = new G4PVPlacement(0, fSpaceFabCartDownFrontPositionVector, fSpaceFabCartDownLogic, "fSpaceFabCartDownFrontPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);
	
		G4ThreeVector fSpaceFabCartDownCentralPositionVector = G4ThreeVector(fPedestalCollimationChamberTransversalGirderShiftX + fSpaceFabPlateWidth * 0.5 - fSpaceFabCartDownWidth * 0.5 - fSpaceFabCartCentralShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabCartDownHeight * 0.5 - fSpaceFabCartUpHeight, 0.);
		fSpaceFabCartDownCentralPhysical = new G4PVPlacement(0, fSpaceFabCartDownCentralPositionVector, fSpaceFabCartDownLogic, "fSpaceFabCartDownCentralPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);	

		G4ThreeVector fSpaceFabCartDownRearPositionVector = G4ThreeVector(fSpaceFabPlateWidth * 0.5 - fSpaceFabCartDownWidth * 0.5 - fSpaceFabCartFrontRearShiftX + fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabCartDownHeight * 0.5 - fSpaceFabCartUpHeight, fSpaceFabPlateLength * 0.5 - fSpaceFabBinaryLength * 0.5 - fSpaceFabBinaryShiftZ);
		fSpaceFabCartDownRearPhysical = new G4PVPlacement(0, fSpaceFabCartDownRearPositionVector, fSpaceFabCartDownLogic, "fSpaceFabCartDownRearPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);

	   	//SpaceFabCartUp
		G4VisAttributes* fSpaceFabCartUpVisAttribute = new G4VisAttributes(G4Colour(0.8, 0.6, 0.4));
		fSpaceFabCartUpVisAttribute->SetForceSolid(true);

		G4Box* fSpaceFabCartUpSolid = new G4Box("SpaceFabCartUpSolid", fSpaceFabCartUpWidth * 0.5, fSpaceFabCartUpHeight * 0.5, fSpaceFabCartUpLength * 0.5);
	
		fSpaceFabCartUpLogic = new G4LogicalVolume(fSpaceFabCartUpSolid, fSpaceFabCartUpMaterial, "SpaceFabCartUp");
		fSpaceFabCartUpLogic->SetVisAttributes(fSpaceFabCartUpVisAttribute);

		G4ThreeVector fSpaceFabCartUpFrontPositionVector = G4ThreeVector(fSpaceFabPlateWidth * 0.5 - fSpaceFabCartUpWidth * 0.5 - fSpaceFabCartFrontRearShiftX + fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabCartUpHeight * 0.5, -(fSpaceFabPlateLength * 0.5 - fSpaceFabBinaryLength * 0.5 - fSpaceFabBinaryShiftZ));
		fSpaceFabCartUpFrontPhysical = new G4PVPlacement(0, fSpaceFabCartUpFrontPositionVector, fSpaceFabCartUpLogic, "fSpaceFabCartUpFrontPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);
	
		G4ThreeVector fSpaceFabCartUpCentralPositionVector = G4ThreeVector(fPedestalCollimationChamberTransversalGirderShiftX + fSpaceFabPlateWidth * 0.5 - fSpaceFabCartUpWidth * 0.5 - fSpaceFabCartCentralShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabCartUpHeight * 0.5, 0.);
		fSpaceFabCartUpCentralPhysical = new G4PVPlacement(0, fSpaceFabCartUpCentralPositionVector, fSpaceFabCartUpLogic, "fSpaceFabCartUpCentralPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);	

		G4ThreeVector fSpaceFabCartUpRearPositionVector = G4ThreeVector(fSpaceFabPlateWidth * 0.5 - fSpaceFabCartUpWidth * 0.5 - fSpaceFabCartFrontRearShiftX + fPedestalCollimationChamberTransversalGirderShiftX, fPedestalCollimationChamberEnvelopeHeight * 0.5 - fSpaceFabCartUpHeight * 0.5, fSpaceFabPlateLength * 0.5 - fSpaceFabBinaryLength * 0.5 - fSpaceFabBinaryShiftZ);
		fSpaceFabCartUpRearPhysical = new G4PVPlacement(0, fSpaceFabCartUpRearPositionVector, fSpaceFabCartUpLogic, "fSpaceFabCartUpRearPhysical", fPedestalCollimationChamberEnvelopeLogic, false, 0);
	}	
	//-----------------------------------------------------------------------------------------------------
	
	//-----------------------------Pedestals M30 M31 (Nicola)----------------------------------------------
	if (bModulesFeOn==true){
		//Plate1
		G4VisAttributes* fPedestalPlate1VisAttribute = new G4VisAttributes();
		fPedestalPlate1VisAttribute->SetForceSolid(true);
	
		G4Box* fPedestalPlate1Solid = new G4Box("PedestalPlate1Solid", fPedestalPlate1Width * 0.5, fPedestalPlate1Height * 0.5, fPedestalPlate1Length * 0.5);
	
		fPedestalPlate1Logic = new G4LogicalVolume(fPedestalPlate1Solid, fPedestalPlate1Material, "PedestalPlate1");
		fPedestalPlate1Logic->SetVisAttributes(fPedestalPlate1VisAttribute);

		G4ThreeVector fPedestalPlate1M30FrontPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate1Height * 0.5), fPedestalM30FrontPositionZ);
		fPedestalPlate1M30FrontPhysical = new G4PVPlacement(0, fPedestalPlate1M30FrontPositionVector,fPedestalPlate1Logic, "fPedestalPlate1M30FrontPhysical", fWorldLogic, false, 0);
	
		G4ThreeVector fPedestalPlate1M30RearPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate1Height * 0.5), fPedestalM30RearPositionZ);
		fPedestalPlate1M30RearPhysical = new G4PVPlacement(0, fPedestalPlate1M30RearPositionVector,fPedestalPlate1Logic, "fPedestalPlate1M30RearPhysical", fWorldLogic, false, 0);

		G4ThreeVector fPedestalPlate1M31FrontPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate1Height * 0.5), fPedestalM31FrontPositionZ);
		fPedestalPlate1M31FrontPhysical = new G4PVPlacement(0, fPedestalPlate1M31FrontPositionVector,fPedestalPlate1Logic, "fPedestalPlate1M31FrontPhysical", fWorldLogic, false, 0);

		G4ThreeVector fPedestalPlate1M31RearPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate1Height * 0.5), fPedestalM31RearPositionZ);
		fPedestalPlate1M31RearPhysical = new G4PVPlacement(0, fPedestalPlate1M31RearPositionVector,fPedestalPlate1Logic, "fPedestalPlate1M31RearPhysical", fWorldLogic, false, 0);
	   
		//Plate2
		G4VisAttributes* fPedestalPlate2VisAttribute = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
		fPedestalPlate2VisAttribute->SetForceSolid(true);
	
		G4Box* fPedestalPlate2Solid = new G4Box("PedestalPlate2Solid", fPedestalPlate2Width * 0.5, fPedestalPlate2Height * 0.5, fPedestalPlate2Length * 0.5);
	
		fPedestalPlate2Logic = new G4LogicalVolume(fPedestalPlate2Solid, fPedestalPlate2Material, "PedestalPlate2");
		fPedestalPlate2Logic->SetVisAttributes(fPedestalPlate2VisAttribute);

		G4ThreeVector fPedestalPlate2M30FrontPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate2Height * 0.5 - fPedestalPlate1Height), fPedestalM30FrontPositionZ);
		fPedestalPlate2M30FrontPhysical = new G4PVPlacement(0, fPedestalPlate2M30FrontPositionVector,fPedestalPlate2Logic, "fPedestalPlate2M30FrontPhysical", fWorldLogic, false, 0);
	
		G4ThreeVector fPedestalPlate2M30RearPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate2Height * 0.5 - fPedestalPlate1Height), fPedestalM30RearPositionZ);
		fPedestalPlate2M30RearPhysical = new G4PVPlacement(0, fPedestalPlate2M30RearPositionVector,fPedestalPlate2Logic, "fPedestalPlate2M30RearPhysical", fWorldLogic, false, 0);

		G4ThreeVector fPedestalPlate2M31FrontPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate2Height * 0.5 - fPedestalPlate1Height), fPedestalM31FrontPositionZ);
		fPedestalPlate2M31FrontPhysical = new G4PVPlacement(0, fPedestalPlate2M31FrontPositionVector,fPedestalPlate2Logic, "fPedestalPlate2M31FrontPhysical", fWorldLogic, false, 0);

		G4ThreeVector fPedestalPlate2M31RearPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate2Height * 0.5 - fPedestalPlate1Height), fPedestalM31RearPositionZ);
		fPedestalPlate2M31RearPhysical = new G4PVPlacement(0, fPedestalPlate2M31RearPositionVector,fPedestalPlate2Logic, "fPedestalPlate2M31RearPhysical", fWorldLogic, false, 0);
	
		//Block
		G4VisAttributes* fPedestalBlockVisAttribute = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
		fPedestalBlockVisAttribute->SetForceSolid(true);
	
		G4Box* fPedestalBlockOutSolid = new G4Box("PedestalBlockOutSolid", fPedestalBlockWidth * 0.5, fPedestalBlockHeight * 0.5, fPedestalBlockLength * 0.5);
	
		G4Box* fPedestalBlockInSolid = new G4Box("PedestalBlockInSolid", (fPedestalBlockWidth - fPedestalBlockThickness) * 0.5, (fPedestalBlockHeight - fPedestalBlockThickness)* 0.5, (fPedestalBlockLength - fPedestalBlockThickness) * 0.5);
	
		G4SubtractionSolid* fPedestalBlockSolid = new G4SubtractionSolid("PedestalBlockSolid", fPedestalBlockOutSolid, fPedestalBlockInSolid);
		
		fPedestalBlockInsideLogic = new G4LogicalVolume(fPedestalBlockInSolid, fPedestalBlockInsideMaterial, "PedestalBlockInside");
		        	
		fPedestalBlockLogic = new G4LogicalVolume(fPedestalBlockSolid, fPedestalBlockMaterial, "PedestalBlock");
		fPedestalBlockLogic->SetVisAttributes(fPedestalBlockVisAttribute);

		G4ThreeVector fPedestalBlockM30FrontPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalBlockHeight * 0.5 - fPedestalPlate1Height - fPedestalPlate2Height), fPedestalM30FrontPositionZ);
		fPedestalBlockM30FrontPhysical = new G4PVPlacement(0, fPedestalBlockM30FrontPositionVector,fPedestalBlockLogic, "fPedestalBlockM30FrontPhysical", fWorldLogic, false, 0);
	
		G4ThreeVector fPedestalBlockM30RearPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalBlockHeight * 0.5 - fPedestalPlate1Height - fPedestalPlate2Height), fPedestalM30RearPositionZ);
		fPedestalBlockM30RearPhysical = new G4PVPlacement(0, fPedestalBlockM30RearPositionVector,fPedestalBlockLogic, "fPedestalBlockM30RearPhysical", fWorldLogic, false, 0);

		G4ThreeVector fPedestalBlockM31FrontPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalBlockHeight * 0.5 - fPedestalPlate1Height - fPedestalPlate2Height), fPedestalM31FrontPositionZ);
		fPedestalBlockM31FrontPhysical = new G4PVPlacement(0, fPedestalBlockM31FrontPositionVector,fPedestalBlockLogic, "fPedestalBlockM31FrontPhysical", fWorldLogic, false, 0);

		G4ThreeVector fPedestalBlockM31RearPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalBlockHeight * 0.5 - fPedestalPlate1Height - fPedestalPlate2Height), fPedestalM31RearPositionZ);
		fPedestalBlockM31RearPhysical = new G4PVPlacement(0, fPedestalBlockM31RearPositionVector,fPedestalBlockLogic, "fPedestalBlockM31RearPhysical", fWorldLogic, false, 0);
		
		fPedestalBlockM30FrontInsidePhysical = new G4PVPlacement(0, fPedestalBlockM30FrontPositionVector, fPedestalBlockInsideLogic, "fPedestalBlockM30FrontInsidePhysical", fWorldLogic, false, 0);
		fPedestalBlockM30RearInsidePhysical = new G4PVPlacement(0, fPedestalBlockM30RearPositionVector, fPedestalBlockInsideLogic, "fPedestalBlockM30RearInsidePhysical", fWorldLogic, false, 0);
		fPedestalBlockM31FrontInsidePhysical = new G4PVPlacement(0, fPedestalBlockM31FrontPositionVector, fPedestalBlockInsideLogic, "fPedestalBlockM31FrontInsidePhysical", fWorldLogic, false, 0);
		fPedestalBlockM31RearInsidePhysical = new G4PVPlacement(0, fPedestalBlockM31RearPositionVector, fPedestalBlockInsideLogic, "fPedestalBlockM31RearInsidePhysical", fWorldLogic, false, 0);

		//Plate3
		G4VisAttributes* fPedestalPlate3VisAttribute = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
		fPedestalPlate3VisAttribute->SetForceSolid(true);
	
		G4Box* fPedestalPlate3Solid = new G4Box("PedestalPlate3Solid", fPedestalPlate3Width * 0.5, fPedestalPlate3Height * 0.5, fPedestalPlate3Length * 0.5);
	
		fPedestalPlate3Logic = new G4LogicalVolume(fPedestalPlate3Solid, fPedestalPlate3Material, "PedestalPlate3");
		fPedestalPlate3Logic->SetVisAttributes(fPedestalPlate3VisAttribute);

		G4ThreeVector fPedestalPlate3M30FrontPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate3Height * 0.5 - fPedestalPlate1Height - fPedestalPlate2Height - fPedestalBlockHeight), fPedestalM30FrontPositionZ);
		fPedestalPlate3M30FrontPhysical = new G4PVPlacement(0, fPedestalPlate3M30FrontPositionVector,fPedestalPlate3Logic, "fPedestalPlate3M30FrontPhysical", fWorldLogic, false, 0);
	
		G4ThreeVector fPedestalPlate3M30RearPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate3Height * 0.5 - fPedestalPlate1Height - fPedestalPlate2Height - fPedestalBlockHeight), fPedestalM30RearPositionZ);
		fPedestalPlate3M30RearPhysical = new G4PVPlacement(0, fPedestalPlate3M30RearPositionVector,fPedestalPlate3Logic, "fPedestalPlate3M30RearPhysical", fWorldLogic, false, 0);

		G4ThreeVector fPedestalPlate3M31FrontPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate3Height * 0.5 - fPedestalPlate1Height - fPedestalPlate2Height - fPedestalBlockHeight), fPedestalM31FrontPositionZ);
		fPedestalPlate3M31FrontPhysical = new G4PVPlacement(0, fPedestalPlate3M31FrontPositionVector,fPedestalPlate3Logic, "fPedestalPlate3M31FrontPhysical", fWorldLogic, false, 0);

		G4ThreeVector fPedestalPlate3M31RearPositionVector = G4ThreeVector(0., - (fBeamHeight - fPedestalPlate3Height * 0.5 - fPedestalPlate1Height - fPedestalPlate2Height - fPedestalBlockHeight), fPedestalM31RearPositionZ);
		fPedestalPlate3M31RearPhysical = new G4PVPlacement(0, fPedestalPlate3M31RearPositionVector,fPedestalPlate3Logic, "fPedestalPlate3M31RearPhysical", fWorldLogic, false, 0);
	}
    //------------------------------------------------------------------------------------------------------
	      
    //Beam pipe
    if(bBeamPipe){
    	G4VisAttributes* fBeamPipeVisAttribute = new G4VisAttributes();  
        fBeamPipeVisAttribute->SetForceSolid(true);
        
        //pipe A0
        fBeamPipeA0Solid = new G4Tubs("BeamPipeA0",
                                      fBeamPipeA0InnerRadius,
                                      fBeamPipeA0OuterRadius,
                                      fBeamPipeA0Length * 0.5,
                                      0*CLHEP::deg,
                                      360*CLHEP::deg);
        
        fBeamPipeA0Logic = new G4LogicalVolume(fBeamPipeA0Solid, fBeamPipeMaterial, "BeamPipeA0");
        
        fBeamPipeA0Logic->SetVisAttributes(fBeamPipeVisAttribute);
                       
        G4ThreeVector fBeamPipeA0PositionVector = G4ThreeVector(0., 0., fBeamPipeA0Length * 0.5);
        
        fBeamPipeA0Physical = new G4PVPlacement(0, fBeamPipeA0PositionVector, fBeamPipeA0Logic, "BeamPipeA0", fWorldLogic, false, 0);
        
        //inside pipe A0
        G4Tubs* fBeamPipeA0InsideSolid = new G4Tubs("BeamPipeA0Inside",
                                              		0.,
                                              		fBeamPipeA0InnerRadius,
                                              		fBeamPipeA0Length * 0.5,
                                              		0*CLHEP::deg,
                                              		360*CLHEP::deg);
        
        G4LogicalVolume* fBeamPipeA0InsideLogic = new G4LogicalVolume(fBeamPipeA0InsideSolid, fBeamPipeVacuum, "BeamPipeA0Inside");
        
        fBeamPipeA0InsideLogic->SetVisAttributes(fBeamPipeVisAttribute);
        
		fBeamPipeA0InsidePhysical = new G4PVPlacement(0, fBeamPipeA0PositionVector, fBeamPipeA0InsideLogic, "BeamPipeInsideA0", fWorldLogic, false, 0);
        
        //pipe A1 
        fBeamPipeA1Solid = new G4Tubs("BeamPipeA1",
                                      fBeamPipeA0InnerRadius,
                                      fBeamPipeA0OuterRadius,
                                      fBeamPipeA1Length * 0.5,
                                      0*CLHEP::deg,
                                      360*CLHEP::deg);
        
        fBeamPipeA1Logic = new G4LogicalVolume(fBeamPipeA1Solid, fBeamPipeMaterial, "BeamPipeA1");
        
        fBeamPipeA1Logic->SetVisAttributes(fBeamPipeVisAttribute);
        
        G4ThreeVector fBeamPipeA1PositionVector = G4ThreeVector(0.,0.,fBeamPipeA0Length + 2.0 * fBeamPipeLengthVariation + (fCollimChamberLength + 4. * fFlangeLength) + fBeamPipeA1Length * 0.5);
        
        fBeamPipeA1Physical = new G4PVPlacement(0,
                                                fBeamPipeA1PositionVector,
                                                fBeamPipeA1Logic,
                                                "BeamPipeA1",
                                                fWorldLogic,
                                                false,
                                                0);
		//inside pipe A1                                               
       	G4Tubs* fBeamPipeA1InsideSolid = new G4Tubs("BeamPipeA1Inside",
                                              		0.,
                                              		fBeamPipeA0InnerRadius,
                                              		fBeamPipeA1Length * 0.5,
                                              		0*CLHEP::deg,
                                              		360*CLHEP::deg);
        
        G4LogicalVolume* fBeamPipeA1InsideLogic = new G4LogicalVolume(fBeamPipeA1InsideSolid, fBeamPipeVacuum, "BeamPipeA1Inside");
        
        fBeamPipeA1InsideLogic->SetVisAttributes(fBeamPipeVisAttribute);
        		
		fBeamPipeA1InsidePhysical = new G4PVPlacement(0, fBeamPipeA1PositionVector, fBeamPipeA1InsideLogic, "BeamPipeA1Inside", fWorldLogic, false, 0);
        
    }
    
    //Concrete
    if(bConcrete){
    
    	G4VisAttributes* fConcreteVisAttribute;
        fConcreteVisAttribute = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
        fConcreteVisAttribute->SetForceSolid(false);
       
       	G4VisAttributes* fPbCapVisAttribute = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
      	fPbCapVisAttribute->SetForceSolid(false);
   
		//-------------------------------------------A0 WALL with hole for pipe----------------------------------------
		//A0B0 first wall after IP no hole
        G4Box* fConcreteA0B0Solid = new G4Box("ConcreteA0B0",
                                              (fRoomWidth +fWallThickness + fWallThickness2) * 0.5,
                                              fWallHeight * 0.5,
                                              fWallThickness * 0.5);
		//A0B0 first wall hole
        G4Tubs* fConcreteA0B1Solid = new G4Tubs("ConcreteA0B1",
                                                0.,
                                                fConcreteA0RadiusPipe,
                                                fWallThickness * 0.6,
                                                0*CLHEP::deg,
                                                360*CLHEP::deg);
		
		//A0Solid first wall with hole for pipe        
        G4SubtractionSolid* fConcreteA0Solid = new G4SubtractionSolid("ConcreteA0", fConcreteA0B0Solid, fConcreteA0B1Solid,0,
        														      G4ThreeVector(-(fRoomShiftX -(fWallThickness2 - fWallThickness) * 0.5 ),- fRoomShiftY,0.));

        fConcreteA0Logic = new G4LogicalVolume(fConcreteA0Solid,
                                               fConcreteMaterial,
                                               "ConcreteA0");
        
        fConcreteA0Logic->SetVisAttributes(fConcreteVisAttribute);
        
        G4ThreeVector fConcreteA0PositionVector = G4ThreeVector(fRoomShiftX -(fWallThickness2 - fWallThickness) * 0.5, //asimmetrical right wall thickness requires this additional shift
        														fRoomShiftY,
        														fConcreteA0Distance);
        
        fConcreteA0Physical = new G4PVPlacement(0,
                                                fConcreteA0PositionVector,
                                                fConcreteA0Logic,
                                                "fConcreteA0",
                                                fWorldLogic,
                                                false,
                                                0);


		//---------------------------------------A1 Concrete SHIELDING BLOCK with hole for pipe----------------------------
   		if(bA1ConcreteDetail==true) //RELASTIC CONCRETE HOLE + Pb RING
		{		
			//A1B0 concrete block for shield -no hole
        	G4Box* fConcreteA1B0Solid = new G4Box("ConcreteA1B0",
                                              fConcreteA1Width * 0.5,
                                              fConcreteA1Height * 0.5,
                                              fConcreteA1Length * 0.5); 

			//A1B1 tube for pipe  hole
        	G4Tubs* fConcreteA1B1Solid = new G4Tubs("ConcreteA1B1",
                                                0.,
                                                fConcreteA1RadiusPipe * 2.0,
                                                fConcreteA1Length * 0.6,
                                                0*CLHEP::deg,
                                                360*CLHEP::deg);	                                                
   
   			//A1B2 Hole for Pb pipe cap
        	G4Box* fConcreteA1B2Solid = new G4Box("ConcreteA1B2",
                                              (fA1LeadPipeCapWidth + 25.*CLHEP::mm) * 0.5,
                                              (fA1LeadPipeCapHeight + 25.*CLHEP::mm) * 0.5,
                                              (fA1LeadPipeCapLength + 10.*CLHEP::mm) * 0.5 + + 10.*CLHEP::mm); // 10 cm added to avoid error in subtraction
         
     		//A1B0B1Solid concrete block for shield with hole for the pipe
        	G4SubtractionSolid* fConcreteA1B0B1Solid = new G4SubtractionSolid("ConcreteA1B0B1", fConcreteA1B0Solid, fConcreteA1B1Solid, 0, G4ThreeVector(0.,25.*CLHEP::cm,0.));
        
     		//A1Solid concrete block for shield with hole for the pipe and for Lead cap
        	G4SubtractionSolid* fConcreteA1B0B1B2Solid = new G4SubtractionSolid("ConcreteA1B0B1B2", fConcreteA1B0B1Solid, fConcreteA1B2Solid, 0, G4ThreeVector(0., 25.*CLHEP::cm, -(fConcreteA1Length-(fA1LeadPipeCapLength + 10.*CLHEP::mm)) * 0.5 - 10.*CLHEP::mm));     
        
        	fConcreteA1Logic = new G4LogicalVolume(fConcreteA1B0B1B2Solid,
                                               fConcreteMaterial,
                                               "ConcreteA1");	
						
			fConcreteA1Logic->SetVisAttributes(fConcreteVisAttribute);
		
			//Pb pipe cap
			G4Box* fA1LeadPipeCapB0Solid = new G4Box("PipeCapA1B0",
                                              fA1LeadPipeCapWidth * 0.5,
                                              fA1LeadPipeCapHeight * 0.5,
                                              fA1LeadPipeCapLength * 0.5);

			//A1B1 tube for shield tube hole
        	G4Tubs* fA1LeadPipeCapB1Solid = new G4Tubs("PipeCapA1B1",
                                                0.,
                                                fConcreteA1RadiusPipe,
                                                fA1LeadPipeCapLength * 0.6,
                                                0*CLHEP::deg,
                                                360*CLHEP::deg);
	
			//Lead pipe Cap solid with hole
        	G4SubtractionSolid* fA1LeadPipeCapSolid = new G4SubtractionSolid("A1LeadPipeCap", fA1LeadPipeCapB0Solid, fA1LeadPipeCapB1Solid,0, G4ThreeVector(0.,0.,0.));      
        
        	fA1LeadPipeCapLogic = new G4LogicalVolume(fA1LeadPipeCapSolid, fLeadPipeCapMaterial, "A1LeadPipeCap");
                
        	fA1LeadPipeCapLogic->SetVisAttributes(fPbCapVisAttribute);
        
        	G4ThreeVector fA1LeadPipeCapPositionVector = G4ThreeVector(0.,0.,fConcreteA1Distance -(fConcreteA1Length-fA1LeadPipeCapLength ) * 0.5);
        
       		fA1LeadPipeCapPhysical = new G4PVPlacement(0,
                                                fA1LeadPipeCapPositionVector,
                                                fA1LeadPipeCapLogic,
                                                "A1LeadPipeCap",
                                                fWorldLogic,
                                                false,
                                                0);
		}
		else //IDEAL CONCRETE HOLE
		{		
			//A1B0 concrete block for shield - no hole
        	G4Box* fConcreteA1B0Solid = new G4Box("ConcreteA1B0",
                                              	fConcreteA1Width * 0.5,
                                              	fConcreteA1Height * 0.5,
                                              	fConcreteA1Length * 0.5); 

			//A1B1 tube for shield tube hole
        	G4Tubs* fConcreteA1B1Solid = new G4Tubs("ConcreteA1B1",
                                                0.,
                                                fConcreteA1RadiusPipe,
                                                fConcreteA1Length * 0.6,
                                                0*CLHEP::deg,
                                                360*CLHEP::deg);
	
			//A1Solid concrete block for shield with hole
        	G4SubtractionSolid* fConcreteA1Solid = new G4SubtractionSolid("ConcreteA1", fConcreteA1B0Solid, fConcreteA1B1Solid,0, G4ThreeVector(0.,25.*CLHEP::cm,0.));
        	
        	fConcreteA1Logic = new G4LogicalVolume(fConcreteA1Solid,
                                               	fConcreteMaterial,
                                               	"ConcreteA1");
                
       		fConcreteA1Logic->SetVisAttributes(fConcreteVisAttribute);
        }
              
        G4ThreeVector fConcreteA1PositionVector = G4ThreeVector(0.,-25.*CLHEP::cm,fConcreteA1Distance);
        
        fConcreteA1Physical = new G4PVPlacement(0,
                                                fConcreteA1PositionVector,
                                                fConcreteA1Logic,
                                                "fConcreteA1",
                                                fWorldLogic,
                                                false,
                                                0);
    		
		//-------------------------------------------FLOOR--------------------------------------------------
        G4Box* fConcreteFloorSolid = new G4Box("ConcreteFloor",
                                              fRoomWidth* 0.5,
                                              fFloorThickness * 0.5,
                                              fRoomLength * 0.5);

        fConcreteFloorLogic = new G4LogicalVolume(fConcreteFloorSolid,
                                               fConcreteMaterial,
                                               "ConcreteFloor");
        
        fConcreteFloorLogic->SetVisAttributes(fConcreteVisAttribute);
        
        G4ThreeVector fConcreteFloorPositionVector = G4ThreeVector(fRoomShiftX,
        														   -(fBeamHeight + fFloorThickness * 0.5), 
																   fConcreteA0Distance + fWallThickness* 0.5 +fRoomLength * 0.5);
        
        fConcreteFloorPhysical = new G4PVPlacement(0,
                                                fConcreteFloorPositionVector,
                                                fConcreteFloorLogic,
                                                "fConcreteFloor",
                                                fWorldLogic,
                                                false,
                                                0);
    
    
		//-------------------------------------------CEILING------------------------------------------------
        G4Box* fConcreteTopSolid = new G4Box("ConcreteTop",
                                              fRoomWidth* 0.5,
                                              fTopThickness * 0.5,
                                              fRoomLength * 0.5);

        fConcreteTopLogic = new G4LogicalVolume(fConcreteTopSolid,
                                               fConcreteMaterial,
                                               "ConcreteTop");
        
        fConcreteTopLogic->SetVisAttributes(fConcreteVisAttribute);
        
        G4ThreeVector fConcreteTopPositionVector = G4ThreeVector(fRoomShiftX,
        														 fRoomHeight - fBeamHeight + fTopThickness * 0.5, 
																 fConcreteA0Distance + fWallThickness* 0.5 +fRoomLength * 0.5);
        
        fConcreteTopPhysical = new G4PVPlacement(0,
                                                fConcreteTopPositionVector,
                                                fConcreteTopLogic,
                                                "fConcreteTop",
                                                fWorldLogic,
                                                false,
                                                0);
  
                                                
		//-----------RIGHT WALL (considering beam propagation direction (opposite to e- beam dumps))---------
        G4Box* fConcreteRightWallSolid = new G4Box("ConcreteRightWall",
                                              fWallThickness2* 0.5,
                                              fWallHeight * 0.5,
                                              fRoomLength * 0.5);

        fConcreteRightWallLogic = new G4LogicalVolume(fConcreteRightWallSolid,
                                               fConcreteMaterial,
                                               "ConcreteRightWall");
        
        fConcreteRightWallLogic->SetVisAttributes(fConcreteVisAttribute);
        
        G4ThreeVector fConcreteRightWallPositionVector = G4ThreeVector(fRoomShiftX - fRoomWidth * 0.5 -fWallThickness2 * 0.5,
        														 	   fRoomShiftY, 
																	   fConcreteA0Distance + fWallThickness* 0.5 +fRoomLength * 0.5);
        
        fConcreteRightWallPhysical = new G4PVPlacement(0,
                                                fConcreteRightWallPositionVector,
                                                fConcreteRightWallLogic,
                                                "fConcreteRightWall",
                                                fWorldLogic,
                                                false,
                                                0);
                                            
		//---------LEFT WALL (considering beam propagation direction (e- beam dumps side))------------------
        G4Box* fConcreteLeftWallSolid = new G4Box("ConcreteLeftWall",
                                              fWallThickness * 0.5,
                                              fWallHeight * 0.5,
                                              fRoomLength * 0.5);

        fConcreteLeftWallLogic = new G4LogicalVolume(fConcreteLeftWallSolid,
                                               fConcreteMaterial,
                                               "ConcreteLeftWall");
        
        fConcreteLeftWallLogic->SetVisAttributes(fConcreteVisAttribute);
        
        G4ThreeVector fConcreteLeftWallPositionVector = G4ThreeVector(fRoomWidth * 0.5 + fRoomShiftX + fWallThickness *0.5,
        														 	  fRoomShiftY, 
																	  fConcreteA0Distance + fWallThickness* 0.5 +fRoomLength * 0.5);
        
        fConcreteLeftWallPhysical = new G4PVPlacement(0,
                                                fConcreteLeftWallPositionVector,
                                                fConcreteLeftWallLogic,
                                                "fConcreteLeftWall",
                                                fWorldLogic,
                                                false,
                                                0);

		//-------A2 WALL (last wall considering z-axis beam propagation (to experimental rooms))-----------

        /*G4Box* fConcreteA2Solid = new G4Box("ConcreteA2",
                                              (fRoomWidth +fWallThickness + fWallThickness2) * 0.5,
                                              fWallHeight * 0.5,
                                              fWallThickness * 0.5);

        fConcreteA2Logic = new G4LogicalVolume(fConcreteA2Solid,
                                               fConcreteMaterial,
                                               "ConcreteA2");
        
		fConcreteA2Logic->SetVisAttributes(fConcreteVisAttribute);*/
        
        G4ThreeVector fConcreteA2PositionVector = G4ThreeVector(fRoomShiftX -(fWallThickness2 - fWallThickness) * 0.5, //asimmetrical right wall thickness requires this additional shift
        														fRoomShiftY,
        														fConcreteA0Distance + fWallThickness * 0.5 + fRoomLength + fWallThickness * 0.5);
        
        fConcreteA2Physical = new G4PVPlacement(0,
                                                fConcreteA2PositionVector,
                                                fConcreteA0Logic, //EDIT: A2 replaced with A0 to make hole in the last wall
                                                "fConcreteA0",    
                                                fWorldLogic,
                                                false,
                                                0);
    }  
     
    //Girders and Pedestals
    G4VisAttributes* fGirderAttribute; 
    fGirderAttribute = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
    fGirderAttribute->SetForceSolid(true);
    
    //Magnet
    G4VisAttributes* fMagnetAttribute; 
    fMagnetAttribute = new G4VisAttributes(G4Colour(0.15,0.1,0.78));
    fMagnetAttribute->SetForceSolid(true);

    G4VisAttributes* fMagnetCoilAttribute; 
    fMagnetCoilAttribute = new G4VisAttributes(G4Colour(0.78,0.1,0.03));
    fMagnetCoilAttribute->SetForceSolid(true);   
       
    //Girders joint Logic Definition    
    G4Box* fGirderJointA0Solid = new G4Box("GirderJointA0",
                                              fGirderJointLength * 0.5,
                                              fGirderHeight * 0.5,
                                              fGirderWidth * 0.5);
    
    G4Box* fGirderJointA1Solid = new G4Box("GirderJointA1",
                                              fGirderJointLength * 0.6,
                                              fGirderHeight * 0.5 - fGirderThickness,
                                              fGirderWidth * 0.5 - fGirderThickness);
    
    G4SubtractionSolid* fGirderJointSolid = new G4SubtractionSolid("GirderJoint", fGirderJointA0Solid, fGirderJointA1Solid);
    
    fGirderJointLogic = new G4LogicalVolume(fGirderJointSolid, fGirderMaterial, "GirderJoint");
        
    fGirderJointLogic->SetVisAttributes(fGirderAttribute);
        
    G4ThreeVector fGirderJointPositionVector = G4ThreeVector(0., 0., fConcreteA1Distance);
    
    //Pedestals Logic Definition 
    G4Box* fPedestalA0Solid = new G4Box("PedestalA0",
                                              fPedestalWidth * 0.5,
                                              fPedestalHeight * 0.5,
                                              fPedestalLength * 0.5);
    
    G4Box* fPedestalA1Solid = new G4Box("PedestalA1",
                                               fPedestalWidth * 0.5 - fPedestalThickness,
                                               fPedestalHeight * 0.6,
                                               fPedestalLength * 0.5 - fPedestalThickness);
    
    G4SubtractionSolid* fPedestalSolid = new G4SubtractionSolid("Pedestal", fPedestalA0Solid, fPedestalA1Solid);
    
    fPedestalLogic = new G4LogicalVolume(fPedestalSolid, fPedestalMaterial, "Pedestal");
        
    fPedestalLogic->SetVisAttributes(fGirderAttribute);
      
    G4Box* fPedestalInsideSolid = new G4Box("PedestalInside",
                                               fPedestalWidth * 0.5 - fPedestalThickness,
                                               fPedestalHeight * 0.5,
                                               fPedestalLength * 0.5 - fPedestalThickness);
    
    fPedestalInsideLogic = new G4LogicalVolume(fPedestalInsideSolid, fPedestalInsideMaterial, "PedestalInside");
        
    fPedestalInsideLogic->SetVisAttributes(fGirderAttribute);                                                                            
     
	//M30 CSPEC chamber - Compton Spectrometer vacuum chamber     
	G4VisAttributes* fCSPECVisAttribute = new G4VisAttributes();
    fCSPECVisAttribute->SetForceSolid(true);
	
	//internal layer
    G4Tubs* fCSPECSolid = new G4Tubs("CSPEC",
                                            fCSPECInnerRadius,
                                            fCSPECOuterRadius,
                                            fCSPECLength * 0.5,
                                            0*CLHEP::deg,
                                            360*CLHEP::deg);
        
	fCSPECLogic = new G4LogicalVolume(fCSPECSolid, fBeamPipeMaterial, "CSPEC");
                                             
    fCSPECLogic->SetVisAttributes(fCSPECVisAttribute);  
    
    //Pb layer
    G4Tubs* fCSPECPbSolid = new G4Tubs("CSPECPb",
                                            fCSPECPbInnerRadius,
                                            fCSPECPbOuterRadius,
                                            fCSPECLength * 0.5,
                                            0*CLHEP::deg,
                                            360*CLHEP::deg);
        
	fCSPECPbLogic = new G4LogicalVolume(fCSPECPbSolid, fLeadPipeCapMaterial, "CSPECPb");	
	
	G4VisAttributes* fCSPECPbVisAttribute = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
	fCSPECPbVisAttribute->SetForceSolid(true);                                            
    fCSPECPbLogic->SetVisAttributes(fCSPECPbVisAttribute);
        
    //external layer
    G4Tubs* fCSPECSheetSolid = new G4Tubs("CSPECSheet",
                                            fCSPECSheetInnerRadius,
                                            fCSPECSheetOuterRadius,
                                            fCSPECLength * 0.5,
                                            0*CLHEP::deg,
                                            360*CLHEP::deg);
        
	fCSPECSheetLogic = new G4LogicalVolume(fCSPECSheetSolid, fBeamPipeMaterial, "CSPECSheet");
                                             
    fCSPECSheetLogic->SetVisAttributes(fCSPECVisAttribute);
                              
    //TransparentCSPEC
    G4VisAttributes* fTransparentCSPECVisAttribute; 								
    fTransparentCSPECVisAttribute = new G4VisAttributes(G4Colour(1.0,0.0,1.0));  			
    fTransparentCSPECVisAttribute->SetForceSolid(false);
    
    G4double signCSPEC = 1.;
    if (bScoringCSPEC==2) signCSPEC = -1.;	//change the transparent box size
    
    G4double fTransparentCSPECLength = (fM31Distance - signCSPEC * 0.5 * fM31GirderLength - (fConcreteA1Distance + 0.5 * fConcreteA1Length)) * 0.5;
    
    G4Box* fTransparentCSPECSolid = new G4Box("TransparentCSPEC", 43. * CLHEP::cm, 50. * CLHEP::cm, fTransparentCSPECLength);   

	fTransparentCSPECLogic = new G4LogicalVolume(fTransparentCSPECSolid, fBeamPipeVacuum, "TransparentCSPEC");
	
	fTransparentCSPECLogic->SetVisAttributes(fTransparentCSPECVisAttribute);
	
	//CSPEC position vector
    G4ThreeVector fCSPECPositionVector = G4ThreeVector(0., 0., fTransparentCSPECLength + fConcreteA1Distance + 0.5 * fConcreteA1Length);
    
    //CSPEC Windows
    G4Tubs* fCSPECWindowSolid = new G4Tubs("fCSPECWindow",
                                                  fBeamPipeA0OuterRadius,
                                                  fCSPECSheetOuterRadius,
                                                  fCSPECWindowLength* 0.5,
                                                  0*CLHEP::deg,
                                                  360*CLHEP::deg);
        
    fCSPECWindowLogic = new G4LogicalVolume(fCSPECWindowSolid, fBeamPipeMaterial, "fCSPECWindow");
        
    fCSPECWindowLogic->SetVisAttributes(fCSPECVisAttribute);
        
    G4ThreeVector fCSPECWindow0PositionVector = G4ThreeVector(0., 0., fCSPECPositionVector.z() - fCSPECLength * 0.5 - fCSPECWindowLength * 0.5);
    G4ThreeVector fCSPECWindow1PositionVector = G4ThreeVector(0., 0., fCSPECPositionVector.z() + fCSPECLength * 0.5 + fCSPECWindowLength * 0.5);
    
    //positioning of CSPEC transparent detector
    if(bScoringCSPEC==1 || bScoringCSPEC==2){                                   								
    	fTransparentCSPECPhysical = new G4PVPlacement(0,					
                                              fCSPECPositionVector,
                                              fTransparentCSPECLogic,
                                              "TransparentCSPEC",
                                              fWorldLogic,
                                              false,
                                              0);     
    }                                 
	
	//positioning of CSPEC (used with the large screen scoring) 
    if (bModulesFeOn==true && bScoringCSPEC==0){                              								
    	fCSPECPhysical = new G4PVPlacement(0,								
                                              fCSPECPositionVector,
                                              fCSPECLogic,
                                              "CSPEC",
                                              fWorldLogic,
                                              false,
                                              0);
                                              
        fCSPECPbPhysical = new G4PVPlacement(0,								
                                              fCSPECPositionVector,
                                              fCSPECPbLogic,
                                              "CSPECPb",
                                              fWorldLogic,
                                              false,
                                              0);
                                              
		fCSPECSheetPhysical = new G4PVPlacement(0,								
                                              fCSPECPositionVector,
                                              fCSPECSheetLogic,
                                              "CSPECSheet",
                                              fWorldLogic,
                                              false,
                                              0);
                                              
		fCSPECWindow0Physical = new G4PVPlacement(0,
                                              fCSPECWindow0PositionVector,
                                              fCSPECWindowLogic,
                                              "fCSPECWindow0",
                                              fWorldLogic,
                                              false,
                                              0);
        
		fCSPECWindow1Physical = new G4PVPlacement(0,
                                              fCSPECWindow1PositionVector,
                                              fCSPECWindowLogic,
                                              "fCSPECWindow1",
                                              fWorldLogic,
                                              false,
                                              0);     
                                             
		//positioning of EndLine pipe and window (Gian)                                           
        //pipe A3        
		fBeamPipeA3Length = fM31Distance - fCSPECPositionVector.z() - fCSPECLength * 0.5 - fM31GirderLength * 0.5 + fM31GirderLength - 50.*CLHEP::cm;
                
        fBeamPipeA3Solid = new G4Tubs("BeamPipeA3",
                                      fBeamPipeA0InnerRadius,
                                      fBeamPipeA0OuterRadius,
                                      fBeamPipeA3Length * 0.5,
                                      0*CLHEP::deg,
                                      360*CLHEP::deg);
        
        fBeamPipeA3Logic = new G4LogicalVolume(fBeamPipeA3Solid, fBeamPipeMaterial, "BeamPipeA3");
        
        G4VisAttributes* fBeamPipeA3VisAttribute = new G4VisAttributes();  
        fBeamPipeA3VisAttribute->SetForceSolid(true);
        
        fBeamPipeA3Logic->SetVisAttributes(fBeamPipeA3VisAttribute);       
        
        G4ThreeVector fBeamPipeA3PositionVector = G4ThreeVector(0., fCSPECPositionVector.y(), fCSPECPositionVector.z() + fCSPECLength * 0.5 + fBeamPipeA3Length * 0.5);
        
        fBeamPipeA3Physical = new G4PVPlacement(0, fBeamPipeA3PositionVector, fBeamPipeA3Logic, "BeamPipeA3", fWorldLogic, false, 0);
        
		//inside pipe A3                                               
        G4Tubs* fBeamPipeA3InsideSolid = new G4Tubs("BeamPipeA3Inside",
                                              		0.,
                                              		fBeamPipeA0InnerRadius,
                                              		fBeamPipeA3Length * 0.5,
                                              		0*CLHEP::deg,
                                              		360*CLHEP::deg);
        
        G4LogicalVolume* fBeamPipeA3InsideLogic = new G4LogicalVolume(fBeamPipeA3InsideSolid, fBeamPipeVacuum, "BeamPipeA3Inside");
        
        fBeamPipeA3InsideLogic->SetVisAttributes(fBeamPipeA3VisAttribute);
        
		fBeamPipeA3InsidePhysical = new G4PVPlacement(0, fBeamPipeA3PositionVector, fBeamPipeA3InsideLogic, "BeamPipeA3Inside", fWorldLogic, false, 0); 
     
    	//Window A3 
    	if(bA3Window) {
			G4VisAttributes* fA3WindowVisAttribute = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
			fA3WindowVisAttribute->SetForceSolid(true);

			G4Tubs* fA3WindowSolid = new G4Tubs("A3WindowSolid",
												0.,
												fBeamPipeA0InnerRadius,
												fA3WindowLength * 0.5,
												0 * CLHEP::deg,
												360 * CLHEP::deg);
	
			fA3WindowLogic = new G4LogicalVolume(fA3WindowSolid, fA3WindowMaterial, "A3WindowLogic");

			fA3WindowLogic->SetVisAttributes(fA3WindowVisAttribute);

			G4ThreeVector fA3WindowPositionVector = G4ThreeVector(0., 0., fBeamPipeA3Length * 0.5 - fA3WindowLength * 0.5);

			fA3WindowPhysical = new G4PVPlacement(0, fA3WindowPositionVector, fA3WindowLogic, "A3WindowPhysical", fBeamPipeA3InsideLogic, false, 0);
    	}		                                          
                                                                           
	} 
	         
    //M31
	G4Box* fM31GirderA0Solid = new G4Box("M31GirderA0",
                                              fGirderWidth * 0.5,
                                              fGirderHeight * 0.5,
                                              fM31GirderLength * 0.5);
    
    G4Box* fM31GirderA1Solid = new G4Box("M31GirderA1",
                                              fGirderWidth * 0.5 - fGirderThickness,
                                              fGirderHeight * 0.5 - fGirderThickness,
                                              fM31GirderLength * 0.6);
    
    G4SubtractionSolid* fM31GirderSolid = new G4SubtractionSolid("M31Girder", fM31GirderA0Solid, fM31GirderA1Solid);
    
    fM31GirderLogic = new G4LogicalVolume(fM31GirderSolid, fGirderMaterial, "M31Girder");
        
    fM31GirderLogic->SetVisAttributes(fCSPECVisAttribute);
       		 
    G4ThreeVector fM31LGirderPositionVector = G4ThreeVector(-fGirderJointLength * 0.5 - fGirderWidth * 0.5, fGirderM31Y, fM31Distance); //Left girder
    G4ThreeVector fM31RGirderPositionVector = G4ThreeVector(+fGirderJointLength * 0.5 + fGirderWidth * 0.5, fGirderM31Y, fM31Distance); //right girder
    
    if (bModulesFeOn==true && bScoringCSPEC<2)
    {
    	fM31LGirderPhysical = new G4PVPlacement(0, fM31LGirderPositionVector, fM31GirderLogic, "M31LGirder", fWorldLogic, false, 0);        
    	fM31RGirderPhysical = new G4PVPlacement(0, fM31RGirderPositionVector, fM31GirderLogic, "M31RGirder", fWorldLogic, false, 0);	
	}
    
    G4ThreeVector fM31Joint1PositionVector = G4ThreeVector(0., fGirderM31Y,fM31Distance - fM31GirderLength * 0.5 + fGirderWidth * 0.5); 
    G4ThreeVector fM31Joint2PositionVector = G4ThreeVector(0., fGirderM31Y,fM31Distance + fM31GirderLength * 0.5 - fGirderWidth * 0.5);
    
    if (bModulesFeOn==true && bScoringCSPEC<2)
    {    
        G4LogicalVolume* fGirder31JointLogic = new G4LogicalVolume(fGirderJointSolid, fGirderMaterial, "Girder31Joint");
        fGirder31JointLogic->SetVisAttributes(fCSPECVisAttribute);
        
    	fM31Joint1Physical = new G4PVPlacement(0, fM31Joint1PositionVector, fGirder31JointLogic, "M31Joint1", fWorldLogic, false, 0);       
     	fM31Joint2Physical = new G4PVPlacement(0, fM31Joint2PositionVector, fGirder31JointLogic, "M31Joint2", fWorldLogic, false, 0);
    }
                                                 
	if(bLine==1)
	{   
    	//M27A
		G4Box* fM27AGirderA0Solid = new G4Box("M27AGirderA0",
                                              fGirderWidth * 0.5,
                                              fGirderHeight * 0.5,
                                              fM27AGirderLength * 0.5);
    
    	G4Box* fM27AGirderA1Solid = new G4Box("M27AGirderA1",
                                              fGirderWidth * 0.5 - fGirderThickness,
                                              fGirderHeight * 0.5 - fGirderThickness,
                                              fM27AGirderLength * 0.6);
    
    	G4SubtractionSolid* fM27AGirderSolid = new G4SubtractionSolid("M27AGirder", fM27AGirderA0Solid, fM27AGirderA1Solid);
    
   		fM27AGirderLogic = new G4LogicalVolume(fM27AGirderSolid, fGirderMaterial, "M27AGirder");
        
    	fM27AGirderLogic->SetVisAttributes(fGirderAttribute);
       		 
    	G4ThreeVector fM27ALGirderPositionVector = G4ThreeVector(-fGirderJointLength * 0.5 - fGirderWidth * 0.5, fGirderY,fM27ADistance); //Left girder
    	G4ThreeVector fM27ARGirderPositionVector = G4ThreeVector(+fGirderJointLength * 0.5 + fGirderWidth * 0.5, fGirderY,fM27ADistance); //right girder
    
    	if (bModulesOn==true)
    	{
    		fM27ALGirderPhysical = new G4PVPlacement(0, fM27ALGirderPositionVector, fM27AGirderLogic, "M27ALGirder", fWorldLogic, false, 0);     
    		fM27ARGirderPhysical = new G4PVPlacement(0, fM27ARGirderPositionVector, fM27AGirderLogic, "M27ARGirder", fWorldLogic, false, 0);
    	}
    
    	G4ThreeVector fM27AJoint1PositionVector = G4ThreeVector(0., fGirderY,fM27ADistance - fM27AGirderLength * 0.5 + fGirderWidth * 0.5); 
    	G4ThreeVector fM27AJoint2PositionVector = G4ThreeVector(0., fGirderY,fM27ADistance + fM27AGirderLength * 0.5 - fGirderWidth * 0.5);
    
    	if (bModulesOn==true)
    	{
    		fM27AJoint1Physical = new G4PVPlacement(0, fM27AJoint1PositionVector, fGirderJointLogic, "M27AJoint1", fWorldLogic, false, 0);      
     		fM27AJoint2Physical = new G4PVPlacement(0, fM27AJoint2PositionVector, fGirderJointLogic, "M27AJoint2", fWorldLogic, false, 0);
     	}
     	
    	//M27A Pedestals
    	G4ThreeVector fM27APedestal1PositionVector = G4ThreeVector(0., fPedestalY,fM27ADistance - fM27AGirderLength * 0.5 + fPedestalLength * 0.5); 
    	G4ThreeVector fM27APedestal2PositionVector = G4ThreeVector(0., fPedestalY,fM27ADistance + fM27AGirderLength * 0.5 - fPedestalLength * 0.5);
    
    	if (bModulesOn==true)
    	{
    		fM27APedestal1Physical = new G4PVPlacement(0,
                                            fM27APedestal1PositionVector,
                                            fPedestalLogic,
                                            "M27APedestal1",
                                            fWorldLogic,
                                            false,
                                            0); 
     
     		fM27APedestal1InsidePhysical = new G4PVPlacement(0,
                                            fM27APedestal1PositionVector,
                                            fPedestalInsideLogic,
                                            "M27APedestal1Inside",
                                            fWorldLogic,
                                            false,
                                            0);                                       
                                                  
     		fM27APedestal2Physical = new G4PVPlacement(0,
                                            fM27APedestal2PositionVector,
                                            fPedestalLogic,
                                            "M27APedestal2",
                                            fWorldLogic,
                                            false,
                                            0);
                                            
     		fM27APedestal2InsidePhysical = new G4PVPlacement(0,
                                            fM27APedestal2PositionVector,
                                            fPedestalInsideLogic,
                                            "M27APedestal2Inside",
                                            fWorldLogic,
                                            false,
                                            0); 
		}
                                            
    	//M27 Magnet    
    	G4Box* fM27AMagnetA0Solid = new G4Box("M27AMagnetA0",				//external box
                                  		 fM27AMagnetWidth * 0.5,
                                    	 fM27AMagnetHeight * 0.5,
                                		 fM27AMagnetLength * 0.5);
      
    	G4Box* fM27AMagnetA1Solid = new G4Box("M27AMagnetA1",				//hole for pipe between poles 
                                  		 fM27AMagnetPoleWidth * 0.6,
                                    	 fBeamPipeA0OuterRadius,  			//check when fixed the final value!
                                		 fM27AMagnetLength * 0.6);
                                		 
   		G4Box* fM27AMagnetA2Solid = new G4Box("M27AMagnetA2",				//holes for Cu coils 
                                  		 (110. * CLHEP::mm) * 0.5,
                                    	 (136. * CLHEP::mm) * 0.5,
                                		 fM27AMagnetLength * 0.6);
    
    	G4Box* fM27AMagnetBaseSolid = new G4Box("M27AMagnetBase",			//Base
                                  		 (532.6 * CLHEP::mm) * 0.5,
                                    	 (30. * CLHEP::mm) * 0.5,
                                		 fM27AMagnetLength * 0.5);
    
    	G4SubtractionSolid* fM27AMagnetA0A1Solid = new G4SubtractionSolid("M27AMagnetA0A1", 
        																	  fM27AMagnetA0Solid, 
        																	  fM27AMagnetA1Solid,
        																	  0,
        																	  G4ThreeVector(0.,0.,0.));
   
    	G4SubtractionSolid* fM27AMagnetA0A1A2Solid = new G4SubtractionSolid("M27AMagnetA0A1A2", 
        																	  fM27AMagnetA0A1Solid, 
        																	  fM27AMagnetA2Solid,
        																	  0,
        																	  G4ThreeVector(fM27AMagnetPoleWidth * 0.5 + (110. * CLHEP::mm) *0.5 ,0.,0.));
        																	     						
    
    	G4SubtractionSolid* fM27AMagnetSolid = new G4SubtractionSolid("M27AMagnetSolid", 
        																	  fM27AMagnetA0A1A2Solid, 
        																	  fM27AMagnetA2Solid,
        																	  0,
        																	  G4ThreeVector(-fM27AMagnetPoleWidth * 0.5 - (110. * CLHEP::mm) * 0.5,0.,0.));
    
    	fM27AMagnetLogic = new G4LogicalVolume(fM27AMagnetSolid,
                                               fMagnetMaterial,
                                               "M27AMagnet");
                                               
    	fM27AMagnetBaseLogic = new G4LogicalVolume(fM27AMagnetBaseSolid,
                                               fMagnetMaterial,
                                               "M27AMagnetBase");
        
    	fM27AMagnetLogic->SetVisAttributes(fMagnetAttribute);
    	fM27AMagnetBaseLogic->SetVisAttributes(fMagnetAttribute);
    
    	G4ThreeVector fM27AMagnetPositionVector = G4ThreeVector(0., 0.,fM27ADistance);
    	G4ThreeVector fM27AMagnetBasePositionVector = G4ThreeVector(0., - fM27AMagnetHeight * 0.5 -(30. * CLHEP::mm) * 0.5,fM27ADistance);
    
    	if (bModulesOn==true)
    	{
    		fM27AMagnetPhysical = new G4PVPlacement(0,
                                            fM27AMagnetPositionVector,
                                            fM27AMagnetLogic,
                                            "M27AMagnet",
                                            fWorldLogic,
                                            false,
                                            0);  
    
    		fM27AMagnetBasePhysical = new G4PVPlacement(0,
                                            fM27AMagnetBasePositionVector,
                                            fM27AMagnetBaseLogic,
                                            "M27AMagnetBase",
                                            fWorldLogic,
                                            false,
                                            0);  
		}
   
   		//M27A Magnet Coils
   		G4Box* fM27AMagnetCoilA0Solid = new G4Box("M27AMagnetCoilA0",		//external box
                                  		 fM27AMagnetCoilWidth * 0.5,
                                    	 fM27AMagnetCoilHeight * 0.5,
                                		 fM27AMagnetCoilLength * 0.5);
      
    	G4Box* fM27AMagnetCoilA1Solid = new G4Box("M27AMagnetCoilA1",		//Internal hole
                                  		 fM27AMagnetCoilWidth * 0.5 - fM27AMagnetCoilThickness,
                                    	 fM27AMagnetCoilHeight * 0.5,  		//check when fixed the final value!
                                		 fM27AMagnetCoilLength * 0.5 - fM27AMagnetCoilThickness);
    
    	G4SubtractionSolid* fM27AMagnetCoilSolid = new G4SubtractionSolid("M27AMagnetCoilSolid", 
        															  fM27AMagnetCoilA0Solid, 
        															  fM27AMagnetCoilA1Solid,
        															  0,
        															  G4ThreeVector(0.,0.,0.));    
    
    	fM27AMagnetCoilLogic = new G4LogicalVolume(fM27AMagnetCoilSolid,
                                               fCoilMaterial,
                                               "M27AMagnetCoil");
        
    	fM27AMagnetCoilLogic->SetVisAttributes(fMagnetCoilAttribute);
    
    	G4ThreeVector fM27AMagnetCoilTopPositionVector = G4ThreeVector(0.,fBeamPipeA0OuterRadius + (fM27AMagnetHeight * 0.5 -fBeamPipeA0OuterRadius - 30. * CLHEP::mm)*0.5,fM27ADistance);
    	G4ThreeVector fM27AMagnetCoilBottomPositionVector = G4ThreeVector(0.,-fBeamPipeA0OuterRadius - (fM27AMagnetHeight * 0.5 -fBeamPipeA0OuterRadius - 30. * CLHEP::mm)*0.5,fM27ADistance);
    
    	if (bModulesOn==true)
    	{    
    		fM27AMagnetCoilTopPhysical = new G4PVPlacement(0,
                                            fM27AMagnetCoilTopPositionVector,
                                            fM27AMagnetCoilLogic,
                                            "M27AMagnetCoilTop",
                                            fWorldLogic,
                                            false,
                                            0);  
    		fM27AMagnetCoilBottomPhysical = new G4PVPlacement(0,
                                            fM27AMagnetCoilBottomPositionVector,
                                            fM27AMagnetCoilLogic,
                                            "M27AMagnetCoilBottom",
                                            fWorldLogic,
                                            false,
                                            0);      
    	}
    
    	//M34	 	
    	G4Box* fM34Solid = new G4Box("M34",
                                  fGirderWidth * 0.5 +fGirderJointLength,
                                  60. * CLHEP::cm,
                                  fM34GirderLength * 0.5);
    
    	fM34Logic = new G4LogicalVolume(fM34Solid,						//M34Logic envelope for the rotation of the whole module (girder+joints+pedestals)
                                    fWorldMaterial,
                                    "M34");
                                    
        fM34Logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    
	
		G4Box* fM34GirderA0Solid = new G4Box("M34GirderA0",
                                              fGirderWidth * 0.5,
                                              fGirderHeight * 0.5,
                                              fM34GirderLength * 0.5);
    
    	G4Box* fM34GirderA1Solid = new G4Box("M34GirderA1",
                                              fGirderWidth * 0.5 - fGirderThickness,
                                              fGirderHeight * 0.5 - fGirderThickness,
                                              fM34GirderLength * 0.6);
    
    	G4SubtractionSolid* fM34GirderSolid = new G4SubtractionSolid("M34Girder", fM34GirderA0Solid, fM34GirderA1Solid);
    
    	fM34GirderLogic = new G4LogicalVolume(fM34GirderSolid,
                                               fGirderMaterial,
                                               "M34Girder");
        
    	fM34GirderLogic->SetVisAttributes(fGirderAttribute);
    
    		 
    	G4ThreeVector fM34LGirderPositionVector = G4ThreeVector(-fGirderJointLength * 0.5 - fGirderWidth * 0.5, fGirderY + 900. *CLHEP::mm,0.); //Left girder
    	G4ThreeVector fM34RGirderPositionVector = G4ThreeVector(+fGirderJointLength * 0.5 + fGirderWidth * 0.5, fGirderY + 900. *CLHEP::mm,0.); //right girder
    
    	if (bModulesOn==true)
    	{
    		fM34LGirderPhysical = new G4PVPlacement(0,
                                            fM34LGirderPositionVector,
                                            fM34GirderLogic,
                                            "M34LGirder",
                                            fM34Logic,
                                            false,
                                            0);    
    
    
    		fM34RGirderPhysical = new G4PVPlacement(0,
                                            fM34RGirderPositionVector,
                                            fM34GirderLogic,
                                            "M34RGirder",
                                            fM34Logic,
                                            false,
                                            0);
    	}
    
    	G4ThreeVector fM34Joint1PositionVector = G4ThreeVector(0., fGirderY + 900. *CLHEP::mm, - fM34GirderLength * 0.5 + fGirderWidth * 0.5); 
    	G4ThreeVector fM34Joint2PositionVector = G4ThreeVector(0., fGirderY + 900. *CLHEP::mm, + fM34GirderLength * 0.5 - fGirderWidth * 0.5);
    
    	if (bModulesOn==true)
    	{
    		fM34Joint1Physical = new G4PVPlacement(0,
                                           fM34Joint1PositionVector,
                                           fGirderJointLogic,
                                           "M34Joint1",
                                           fM34Logic,
                                           false,
                                           0);  
     
     		fM34Joint2Physical = new G4PVPlacement(0,
                                            fM34Joint2PositionVector,
                                            fGirderJointLogic,
                                            "M34Joint2",
                                            fM34Logic,
                                            false,
                                            0);
  	
    	}
   
    	//M34 Pedestals
   	 	G4ThreeVector fM34Pedestal1PositionVector = G4ThreeVector(0., fPedestalY + 900. *CLHEP::mm,- fM34GirderLength * 0.5 + fPedestalLength * 0.5); 
    	G4ThreeVector fM34Pedestal2PositionVector = G4ThreeVector(0., fPedestalY + 900. *CLHEP::mm,+ fM34GirderLength * 0.5 - fPedestalLength * 0.5);
    
   	 	if (bModulesOn==true)
    	{
   			fM34Pedestal1Physical = new G4PVPlacement(0,
                                            fM34Pedestal1PositionVector,
                                            fPedestalLogic,
                                            "M34Pedestal1",
                                            fM34Logic,
                                            false,
                                            0);  
    
    		fM34Pedestal1InsidePhysical = new G4PVPlacement(0,
                                            fM34Pedestal1PositionVector,
                                            fPedestalInsideLogic,
                                            "M34Pedestal1Inside",
                                            fM34Logic,
                                            false,
                                            0);                                            
     
     		fM34Pedestal2Physical = new G4PVPlacement(0,
                                            fM34Pedestal2PositionVector,
                                            fPedestalLogic,
                                            "M34Pedestal2",
                                            fM34Logic,
                                            false,
                                            0); 
                                            
    		fM34Pedestal2InsidePhysical = new G4PVPlacement(0,
                                            fM34Pedestal2PositionVector,
                                            fPedestalInsideLogic,
                                            "M34Pedestal2Inside",
                                            fM34Logic,
                                            false,
                                            0);  
     	}
     
     	//M34 Complete (girder+joints+pedestals)
     	G4ThreeVector fM34PositionVector = G4ThreeVector(fM34X,- 900. *CLHEP::mm,fM34Distance); 
     
     	G4RotationMatrix *fM34RotationMatrix = new G4RotationMatrix(0.,0.,0.);	
     	fM34RotationMatrix->rotateY(-fThetaM34);

     	if (bModulesOn==true)
     	{
     		fM34Physical = new G4PVPlacement(fM34RotationMatrix,
                                      fM34PositionVector,
                                      fM34Logic,
                                      "M34",
                                      fWorldLogic,
                                      false,
                                      0); 
     	}
     
    	//M34 Dump Magnet 
    	G4Tubs* fM34MagnetA0Solid = new G4Tubs("M34MagnetA0",		//external box
                                  		 fM34MagnetInnerRadius,
                                    	 fM34MagnetOuterRadius,
                                		 fM34MagnetHeight * 0.5,
                                		 fM34MagnetThetaStart,
                                		 fM34MagnetThetaEnd);

		G4Tubs* fM34MagnetA1Solid = new G4Tubs("M34MagnetA1",		//poles hole
                                  		 fM34MagnetInnerRadius + 80. * CLHEP::mm,
                                    	 fM34MagnetOuterRadius - 80. * CLHEP::mm,
                                		 25. * CLHEP::mm,
                                		 fM34MagnetThetaStart*1.02,
                                		 fM34MagnetThetaEnd*1.05);
                                		 
		G4Tubs* fM34MagnetA2Solid = new G4Tubs("M34MagnetA2",		//coils hole
                                  		 fM34MagnetRadius - 240. * CLHEP::mm,
                                    	 fM34MagnetRadius - 60. * CLHEP::mm,
                                		 (254. * 0.5) * CLHEP::mm,
                                		 fM34MagnetThetaStart*1.02,
                                		 fM34MagnetThetaEnd*1.05);                          		 
    
   		G4Tubs* fM34MagnetA3Solid = new G4Tubs("M34MagnetA2",		//coils hole
                                  		 fM34MagnetRadius + 60. * CLHEP::mm,
                                    	 fM34MagnetRadius + 240. * CLHEP::mm,
                                		 (254. * 0.5) * CLHEP::mm,
                                		 fM34MagnetThetaStart*1.02,
                                		 fM34MagnetThetaEnd*1.05);       
        
    	G4SubtractionSolid* fM34MagnetA0A1Solid = new G4SubtractionSolid("M34MagnetA0A1Solid", 
        															  fM34MagnetA0Solid, 
        															  fM34MagnetA1Solid,
        															  0,
        															  G4ThreeVector(0.,0.,0.));
                          		 
    	G4SubtractionSolid* fM34MagnetA0A1A2Solid = new G4SubtractionSolid("M34MagnetA0A1A2Solid", 
   		   															  fM34MagnetA0A1Solid, 
        															  fM34MagnetA2Solid,
        															  0,
        															  G4ThreeVector(0.,0.,0.));    
    
    	G4SubtractionSolid* fM34MagnetA0A1A2A3Solid = new G4SubtractionSolid("M34MagnetA0A1Solid", 
        															  fM34MagnetA0A1A2Solid, 
        															  fM34MagnetA3Solid,
        															  0,
        															  G4ThreeVector(0.,0.,0.));  
    	fM34MagnetLogic = new G4LogicalVolume(fM34MagnetA0A1A2A3Solid,
                                               fMagnetMaterial,
                                               "M34AMagnet");
                                               
        
    	fM34MagnetLogic->SetVisAttributes(fMagnetAttribute);
    
    	G4ThreeVector fM34MagnetPositionVector = G4ThreeVector(fM33X+fM34MagnetRadius*cos(fThetaElectron)+(fM33GirderLength*0.5 + 500*CLHEP::mm)*sin(fThetaElectron),0.,fM33Distance+(fM33GirderLength*0.5 + 500 *CLHEP::mm)*cos(fThetaElectron) - fM34MagnetRadius * sin(fThetaElectron)); 
         
		G4RotationMatrix *fM34MagnetRotationMatrix = new G4RotationMatrix(0.,0.,0.);
         
		fM34MagnetRotationMatrix->rotateX(1.57079);
     
    	if (bModulesOn==true)
    	{
    		fM34MagnetPhysical = new G4PVPlacement(fM34MagnetRotationMatrix,
                                      fM34MagnetPositionVector,
                                      fM34MagnetLogic,
                                      "M34Magnet",
                                      fWorldLogic,
                                      false,
                                      0); 
    	}
    
    	//M34 Dump Magnet Coils
    	G4Tubs* fM34MagnetCoilA0Solid = new G4Tubs("M34MagnetCoilA0",		//external box
                                  		 fM34MagnetRadius - 190. * CLHEP::mm,
                                    	 fM34MagnetRadius + 190. * CLHEP::mm,
                                		 fM34MagnetCoilHeight * 0.5,
                                		 fM34MagnetCoilThetaStart,
                                		 fM34MagnetCoilThetaEnd);

		G4Tubs* fM34MagnetCoilA1Solid = new G4Tubs("M34MagnetCoilA1",		//hole
                                  		 fM34MagnetRadius - 88. * CLHEP::mm,
                                    	 fM34MagnetRadius + 88. * CLHEP::mm,
                                		 fM34MagnetCoilHeight * 0.6,
                                		 fM34MagnetCoilHoleThetaStart,
                                		 fM34MagnetCoilHoleThetaEnd);   
                                		 
    	G4SubtractionSolid* fM34MagnetCoilSolid = new G4SubtractionSolid("M34MagnetACoilSolid", 
        															  fM34MagnetCoilA0Solid, 
        															  fM34MagnetCoilA1Solid,
        															  0,
        															  G4ThreeVector(0.,0.,0.));   
    
    	fM34MagnetCoilLogic = new G4LogicalVolume(fM34MagnetCoilSolid,
                                               fCoilMaterial,
                                               "M34AMagnetCoil");
                                               
        
    	fM34MagnetCoilLogic->SetVisAttributes(fMagnetCoilAttribute);     
                                                         
    	G4ThreeVector fM34MagnetCoilTopPositionVector = G4ThreeVector(
     													fM33X+fM34MagnetRadius*cos(fThetaElectron) + (fM33GirderLength * 0.5 + 500*CLHEP::mm) * sin(fThetaElectron),
     													126.5 * CLHEP::mm - fM34MagnetCoilHeight * 0.5,
     													fM33Distance+ (fM33GirderLength*0.5 + 500 *CLHEP::mm) * cos(fThetaElectron) - fM34MagnetRadius * sin(fThetaElectron)); 

    	G4ThreeVector fM34MagnetCoilBottomPositionVector = G4ThreeVector(
    													 fM33X + fM34MagnetRadius*cos(fThetaElectron) + (fM33GirderLength *0.5 + 500 *CLHEP::mm) * sin(fThetaElectron),
     													-126.5 * CLHEP::mm + fM34MagnetCoilHeight * 0.5,
     													fM33Distance + (fM33GirderLength *0.5 + 500 *CLHEP::mm) * cos(fThetaElectron) - fM34MagnetRadius * sin(fThetaElectron)
     													); 
         
    	//G4RotationMatrix *fM34MagnetRotationMatrix = new G4RotationMatrix(0.,0.,0.);      
    	//fM34MagnetRotationMatrix->rotateX(1.57079);
    
		if (bModulesOn==true)
    	{
    		fM34MagnetCoilTopPhysical = new G4PVPlacement(fM34MagnetRotationMatrix,
                                      fM34MagnetCoilTopPositionVector,
                                      fM34MagnetCoilLogic,
                                      "M34MagnetCoilTop",
                                      fWorldLogic,
                                      false,
                                      0); 
     
     		fM34MagnetCoilBottomPhysical = new G4PVPlacement(fM34MagnetRotationMatrix,
                                      fM34MagnetCoilBottomPositionVector,
                                      fM34MagnetCoilLogic,
                                      "M34MagnetCoilBottom",
                                      fWorldLogic,
                                      false,
                                      0); 
    	}
      
    	//M32
    	G4Box* fM32Solid = new G4Box("M32",
                                  fGirderWidth * 0.5 +fGirderJointLength,
                                  60. * CLHEP::cm,
                                  fM32GirderLength * 0.5);
    
    	fM32Logic = new G4LogicalVolume(fM32Solid,				//M32Logic envelope for the rotation of the whole module (girder+joints+pedestals)
                                    fWorldMaterial,
                                    "M32");
                                    
        fM32Logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    
	
		G4Box* fM32GirderA0Solid = new G4Box("M32GirderA0",
                                              fGirderWidth * 0.5,
                                              fGirderHeight * 0.5,
                                              fM32GirderLength * 0.5);
    
    	G4Box* fM32GirderA1Solid = new G4Box("M32GirderA1",
                                              fGirderWidth * 0.5 - fGirderThickness,
                                              fGirderHeight * 0.5 - fGirderThickness,
                                              fM32GirderLength * 0.6);
    
    	G4SubtractionSolid* fM32GirderSolid = new G4SubtractionSolid("M32Girder", fM32GirderA0Solid, fM32GirderA1Solid);
    
    	fM32GirderLogic = new G4LogicalVolume(fM32GirderSolid,
                                               fGirderMaterial,
                                               "M32Girder");
        
    	fM32GirderLogic->SetVisAttributes(fGirderAttribute);
       		 
    	G4ThreeVector fM32LGirderPositionVector = G4ThreeVector(-fGirderJointLength * 0.5 - fGirderWidth * 0.5, fGirderY + 900. *CLHEP::mm,0.); //Left girder
    	G4ThreeVector fM32RGirderPositionVector = G4ThreeVector(+fGirderJointLength * 0.5 + fGirderWidth * 0.5, fGirderY + 900. *CLHEP::mm,0.); //right girder
    
    	if (bModulesOn==true)
    	{
    		fM32LGirderPhysical = new G4PVPlacement(0,
                                            fM32LGirderPositionVector,
                                            fM32GirderLogic,
                                            "M32LGirder",
                                            fM32Logic,
                                            false,
                                            0);    
    
    
    		fM32RGirderPhysical = new G4PVPlacement(0,
                                            fM32RGirderPositionVector,
                                            fM32GirderLogic,
                                            "M32RGirder",
                                            fM32Logic,
                                            false,
                                            0);
    	}
    
    	G4ThreeVector fM32Joint1PositionVector = G4ThreeVector(0., fGirderY + 900. *CLHEP::mm,- fM32GirderLength * 0.5 + fGirderWidth * 0.5); 
    	G4ThreeVector fM32Joint2PositionVector = G4ThreeVector(0., fGirderY + 900. *CLHEP::mm, + fM32GirderLength * 0.5 - fGirderWidth * 0.5);
    
    	if (bModulesOn==true)
    	{
    		fM32Joint1Physical = new G4PVPlacement(0,
                                           fM32Joint1PositionVector,
                                           fGirderJointLogic,
                                           "M32Joint1",
                                           fM32Logic,
                                           false,
                                           0);  
     
     		fM32Joint2Physical = new G4PVPlacement(0,
                                            fM32Joint2PositionVector,
                                            fGirderJointLogic,
                                            "M32Joint2",
                                            fM32Logic,
                                            false,
                                            0);
  
     	}  
   
    	//M32 Pedestals
    	G4ThreeVector fM32Pedestal1PositionVector = G4ThreeVector(0., fPedestalY+ 900. *CLHEP::mm,- fM32GirderLength * 0.5 + fPedestalLength * 0.5); 
    	G4ThreeVector fM32Pedestal2PositionVector = G4ThreeVector(0., fPedestalY+ 900. *CLHEP::mm,+ fM32GirderLength * 0.5 - fPedestalLength * 0.5);
    
    	if (bModulesOn==true)
    	{
    		fM32Pedestal1Physical = new G4PVPlacement(0,
                                            fM32Pedestal1PositionVector,
                                            fPedestalLogic,
                                            "M32Pedestal1",
                                            fM32Logic,
                                            false,
                                            0);  
    
    		fM32Pedestal1InsidePhysical = new G4PVPlacement(0,
                                            fM32Pedestal1PositionVector,
                                            fPedestalInsideLogic,
                                            "M32Pedestal1Inside",
                                            fM32Logic,
                                            false,
                                            0);                                            
     
     		fM32Pedestal2Physical = new G4PVPlacement(0,
                                            fM32Pedestal2PositionVector,
                                            fPedestalLogic,
                                            "M32Pedestal2",
                                            fM32Logic,
                                            false,
                                            0); 
                                            
    		fM32Pedestal2InsidePhysical = new G4PVPlacement(0,
                                            fM32Pedestal2PositionVector,
                                            fPedestalInsideLogic,
                                            "M32Pedestal2Inside",
                                            fM32Logic,
                                            false,
                                            0);  
     	}
     
     	//M32 Complete (girders+joints+pedestals)                                      
     	G4ThreeVector fM32PositionVector = G4ThreeVector(fM32X + 20 * CLHEP::cm,- 900. * CLHEP::mm,fM32Distance); 
     
     	G4RotationMatrix *fElectronRotationMatrix = new G4RotationMatrix(0.,0.,0.);
     	fElectronRotationMatrix->rotateY(-fThetaElectron);

		if (bModulesOn==true)
		{     
     		fM32Physical = new G4PVPlacement(fElectronRotationMatrix,
                                      fM32PositionVector,
                                      fM32Logic,
                                      "M32",
                                      fWorldLogic,
                                      false,
                                      0);        
    	 }
     
   		//M33
    	G4Box* fM33Solid = new G4Box("M33",
                                  fGirderWidth * 0.5 +fGirderJointLength,
                                  150 * CLHEP::cm,
                                  fM33GirderLength * 0.5);
    	
    	fM33Logic = new G4LogicalVolume(fM33Solid,				//M33Logic envelope for the rotation of the whole module (girder+joints+pedestals)
                                    fWorldMaterial,
                                    "M33");
        
        fM33Logic->SetVisAttributes(G4VisAttributes::GetInvisible());
    
		G4Box* fM33GirderA0Solid = new G4Box("M33GirderA0",
                                              fGirderWidth * 0.5,
                                              fGirderHeight * 0.5,
                                              fM33GirderLength * 0.5);
    
    	G4Box* fM33GirderA1Solid = new G4Box("M33GirderA1",
                                              fGirderWidth * 0.5 - fGirderThickness,
                                              fGirderHeight * 0.5 - fGirderThickness,
                                              fM33GirderLength * 0.6);
    
    	G4SubtractionSolid* fM33GirderSolid = new G4SubtractionSolid("M33Girder", fM33GirderA0Solid, fM33GirderA1Solid);
    
    	fM33GirderLogic = new G4LogicalVolume(fM33GirderSolid,
                                               fGirderMaterial,
                                               "M33Girder");
        
    	fM33GirderLogic->SetVisAttributes(fGirderAttribute);
    		 
    	G4ThreeVector fM33LGirderPositionVector = G4ThreeVector(-fGirderJointLength * 0.5 - fGirderWidth * 0.5, fGirderY,0.); //Left girder
    	G4ThreeVector fM33RGirderPositionVector = G4ThreeVector(+fGirderJointLength * 0.5 + fGirderWidth * 0.5, fGirderY,0.); //right girder
    
    	if (bModulesOn==true)
    	{
    		fM33LGirderPhysical = new G4PVPlacement(0,
                                            fM33LGirderPositionVector,
                                            fM33GirderLogic,
                                            "M33LGirder",
                                            fM33Logic,
                                            false,
                                            0);    
    
    
    		fM33RGirderPhysical = new G4PVPlacement(0,
                                            fM33RGirderPositionVector,
                                            fM33GirderLogic,
                                            "M33RGirder",
                                            fM33Logic,
                                            false,
                                            0);
    	}
    
    	G4ThreeVector fM33Joint1PositionVector = G4ThreeVector(0., fGirderY,- fM33GirderLength * 0.5 + fGirderWidth * 0.5); 
    	G4ThreeVector fM33Joint2PositionVector = G4ThreeVector(0., fGirderY, + fM33GirderLength * 0.5 - fGirderWidth * 0.5);
    
    	if (bModulesOn==true)
    	{
    		fM33Joint1Physical = new G4PVPlacement(0,
                                           fM33Joint1PositionVector,
                                           fGirderJointLogic,
                                           "M33Joint1",
                                           fM33Logic,
                                           false,
                                           0);  
     
     		fM33Joint2Physical = new G4PVPlacement(0,
                                            fM33Joint2PositionVector,
                                            fGirderJointLogic,
                                            "M33Joint2",
                                            fM33Logic,
                                            false,
                                            0);
		}
   
    	//M33 Pedestals
    	G4ThreeVector fM33Pedestal1PositionVector = G4ThreeVector(0., fPedestalY,- fM33GirderLength * 0.5 + fPedestalLength * 0.5); 
    	G4ThreeVector fM33Pedestal2PositionVector = G4ThreeVector(0., fPedestalY,+ fM33GirderLength * 0.5 - fPedestalLength * 0.5);
    
    	if (bModulesOn==true)
    	{
    		fM33Pedestal1Physical = new G4PVPlacement(0,
                                            fM33Pedestal1PositionVector,
                                            fPedestalLogic,
                                            "M33Pedestal1",
                                            fM33Logic,
                                            false,
                                            0);  
    
    		fM33Pedestal1InsidePhysical = new G4PVPlacement(0,
                                            fM33Pedestal1PositionVector,
                                            fPedestalInsideLogic,
                                            "M33Pedestal1Inside",
                                            fM33Logic,
                                            false,
                                            0);                                            
     
     		fM33Pedestal2Physical = new G4PVPlacement(0,
                                            fM33Pedestal2PositionVector,
                                            fPedestalLogic,
                                            "M33Pedestal2",
                                            fM33Logic,
                                            false,
                                            0); 
                                            
    		fM33Pedestal2InsidePhysical = new G4PVPlacement(0,
                                            fM33Pedestal2PositionVector,
                                            fPedestalInsideLogic,
                                            "M33Pedestal2Inside",
                                            fM33Logic,
                                            false,
                                            0);  
		}
     
    	//M33 Complete (girders+joints+pedestals)                                      
    	G4ThreeVector fM33PositionVector = G4ThreeVector(fM33X,0.,fM33Distance); 
     
    	//G4RotationMatrix *fElectronRotationMatrix = new G4RotationMatrix(0.,0.,0.);
    	//fElectronRotationMatrix->rotateY(-fThetaElectron);

    	if (bModulesOn==true)
    	{
    		fM33Physical = new G4PVPlacement(fElectronRotationMatrix,
                                      fM33PositionVector,
                                      fM33Logic,
                                      "M33",
                                      fWorldLogic,
                                      false,
                                      0); 
		}
     
	}
    
    //Transparent detector
    if(bTransparentDetector){

		G4VisAttributes* fDetectorVisAttribute; 								
        fDetectorVisAttribute = new G4VisAttributes(G4Colour(1.0,0.0,1.0));  			//set detector vis attribute
        fDetectorVisAttribute->SetForceSolid(false);
        
        //Screen 
        G4Box* fTransparentDetectorSolidBox = new G4Box("TransparentDetector",                                  
                                                     fTransparentDetectorWidth * 0.5,
                                                     fTransparentDetectorHeight * 0.5,
                                                     fTransparentDetectorLength * 0.5);
        
        //Tube for hole subtraction in screen
        G4Tubs* fBeamPipeHoleSolid = new G4Tubs("BeamPipeHoleSolid",
                                              0.,
                                              fBeamPipeA0OuterRadius,
                                              (fBeamPipeA0Length + fBeamPipeLengthVariation) * 0.5,
                                              0*CLHEP::deg,
                                              360*CLHEP::deg);
        
        G4SubtractionSolid* fTransparentDetectorSolid = new G4SubtractionSolid("TransparentDetector", fTransparentDetectorSolidBox, fBeamPipeHoleSolid, 0, G4ThreeVector(-fRoomShiftX,-fRoomShiftY,0.)); //screen not centered on beam axys, floor-ceiling asimmetry 
     
        fTransparentDetectorLogic = new G4LogicalVolume(fTransparentDetectorSolid, fWorldMaterial, "TransparentDetector");
       
        fTransparentDetectorLogic->SetVisAttributes(fDetectorVisAttribute);
        
     	//position first detector
        G4ThreeVector fTransparentDetectorPositionVector = G4ThreeVector(fRoomShiftX,fRoomShiftY,fConcreteA0Distance - fWallThickness * 0.5 - fTransparentDetectorLength * 0.5); 

        fTransparentDetectorPhysical = new G4PVPlacement(0,
                                                         fTransparentDetectorPositionVector,
                                                         fTransparentDetectorLogic,
                                                         "TransparentDetector",
                                                         fWorldLogic,
                                                         false,
                                                         0);
        
        //postion second detector (100 cm shift on z-axis to avoid overlap with M32)
        fTransparentDetectorPositionVector = G4ThreeVector(fRoomShiftX,fRoomShiftY,(fBeamPipeA0Length + fBeamPipeLengthVariation) - fTransparentDetectorLength * 0.5 - 100.0 * CLHEP::cm); 
        
        fTransparentDetectorPhysical = new G4PVPlacement(0,
                                                         fTransparentDetectorPositionVector,
                                                         fTransparentDetectorLogic,
                                                         "TransparentDetector",
                                                         fWorldLogic,
                                                         false,
                                                         1);
		
		//position third detector (just before ConcreteA1)
        fTransparentDetectorPositionVector = G4ThreeVector(fRoomShiftX,fRoomShiftY,fConcreteA1Distance - fConcreteA1Length * 0.5 - fTransparentDetectorLength * 0.5); 

        fTransparentDetectorPhysical = new G4PVPlacement(0,
                                                         fTransparentDetectorPositionVector,
                                                         fTransparentDetectorLogic,
                                                         "TransparentDetector",
                                                         fWorldLogic,
                                                         false,
                                                         2);

        //SCORING AFTER COLLIMATION WHOLE ROOM CROSS-SECTION
        G4Box* fTransparentDetectorSolidBox1 = new G4Box("TransparentDetectorBox", //gian: a box without the hole, it is IMPORTANT for the primary beam!!!
                                                     fRoomWidth * 0.5,
                                                     fTransparentDetectorHeight * 0.5,
                                                     fTransparentDetectorLength * 0.5);
        
        fTransparentDetectorBoxLogic = new G4LogicalVolume(fTransparentDetectorSolidBox1, fWorldMaterial, "TransparentDetectorBox");
       
        G4ThreeVector fTransparentDetectorBoxPositionVector = G4ThreeVector(fRoomShiftX, fRoomShiftY, fConcreteA1Distance + fConcreteA1Length * 0.5 + fTransparentDetectorLength * 0.5); 

		if(bScoringCSPEC==0) //in case of CSPEC scoring to avoid overlap 
       	{   
       		fTransparentDetectorBoxPhysical = new G4PVPlacement(0,
                                                            fTransparentDetectorBoxPositionVector,
                                                            fTransparentDetectorBoxLogic,
                                                            "TransparentDetectorBox",
                                                            fWorldLogic,
                                                            false,
                                                            1);	
    	}
    	
    	fTransparentDetectorBoxLogic->SetVisAttributes(fDetectorVisAttribute);
    
    	//Transparent Detector for dose evaluation (in air) at the position of the Racks
        G4Box* fTransparentDetectorRack1Solid = new G4Box("TransparentDetectorRack1Solid",
                                                     	fRackWidth * 0.5,
                                                     	fRack1Height * 0.5,
                                                     	fRackLength * 0.5);
        G4Box* fTransparentDetectorRack2Solid = new G4Box("TransparentDetectorRack2Solid",
                                                     	fRackWidth * 0.5,
                                                     	fRack2Height * 0.5,
                                                     	fRackLength * 0.5);
        
        fTransparentDetectorRack1Logic = new G4LogicalVolume(fTransparentDetectorRack1Solid, fWorldMaterial, "TransparentDetectorRack1");
        fTransparentDetectorRack2Logic = new G4LogicalVolume(fTransparentDetectorRack2Solid, fWorldMaterial, "TransparentDetectorRack2");
       
        fTransparentDetectorRack1Logic->SetVisAttributes(fDetectorVisAttribute);
		fTransparentDetectorRack2Logic->SetVisAttributes(fDetectorVisAttribute);
        
       	G4ThreeVector fTransparentDetectorRack1PositionVector = G4ThreeVector(fRack1PositionX,fRack1PositionY,fRack1PositionZ);
        G4ThreeVector fTransparentDetectorRack2PositionVector = G4ThreeVector(fRack2PositionX,fRack2PositionY,fRack2PositionZ);
	
       	fTransparentDetectorRack1Physical = new G4PVPlacement(0,
                           								 fTransparentDetectorRack1PositionVector,
                           								 fTransparentDetectorRack1Logic,
                                                         "TransparentDetectorRack1",
                                                         fWorldLogic,
                                                         false,
                                                         0);   
                     
      	fTransparentDetectorRack2Physical = new G4PVPlacement(0,
                                                         fTransparentDetectorRack2PositionVector,
                                                         fTransparentDetectorRack2Logic,
                                                         "TransparentDetectorRack2",
                                                         fWorldLogic,
                                                         false,
                                                         0); 
      	
    	//Transparent Detector for dose evaluation (in air) at the position of the Hexapod 
        G4Box* fTransparentDetectorHexapodSolid = new G4Box("TransparentDetectorHexapodSolid",
                                                     	 fHexapodWidth * 0.5,
                                                     	 fHexapodHeight * 0.5,
                                                     	 fHexapodLength * 0.5);
       
        fTransparentDetectorHexapodLogic = new G4LogicalVolume(fTransparentDetectorHexapodSolid, fWorldMaterial, "TransparentDetectorHexapod");
       
        fTransparentDetectorHexapodLogic->SetVisAttributes(fDetectorVisAttribute);
        
        G4double fHexapodYDistance = 30. * CLHEP::mm;
        fHexapodPositionX = 0. * CLHEP::cm;
    	fHexapodPositionY = fCollimationEnvelopeShift - fCollimationEnvelopeHeight * 0.5 - fHexapodHeight * 0.5 - fHexapodYDistance;
    	fHexapodPositionZ = fBeamPipeA0Length + (fCollimChamberLength + 4. * fFlangeLength) * 0.5; 
        G4ThreeVector fTransparentDetectorHexapodPositionVector = G4ThreeVector(fHexapodPositionX,fHexapodPositionY,fHexapodPositionZ);
		
        fTransparentDetectorHexapodPhysical = new G4PVPlacement(0,
                                                         fTransparentDetectorHexapodPositionVector, 
                                                         fTransparentDetectorHexapodLogic,
                                                         "TransparentDetectorHexapod",
                                                         fWorldLogic,
                                                         false,
                                                         0);                                                                        
    }

#ifndef G4MULTITHREADED
	//Sensitive Volumes
	if(bTransparentDetector){
    	G4VSensitiveDetector* vDetector = new G4ELINP_SensitiveDetector("det");
        G4SDManager::GetSDMpointer()->AddNewDetector(vDetector);
        
        switch (fSensitiveIndex) {
        	case (1):     	
				if(bScoringCSPEC==1 || bScoringCSPEC==2){   
     				fSensitiveVolume = fTransparentCSPECLogic; 
     			}
     			else{  
        			fSensitiveVolume = fTransparentDetectorBoxLogic;
        		} 
        		fSensitiveVolume->SetSensitiveDetector(vDetector);
        		break;
        	case (2):    
        	    if(bScoringCSPEC==0){
        			fSensitiveVolume = fTransparentDetectorBoxLogic;
        		}
        		fSensitiveVolume->SetSensitiveDetector(vDetector);
        		fTransparentDetectorLogic->SetSensitiveDetector(vDetector); 
        		break; 
        	case (3):      
                fSensitiveVolume = fTransparentDetectorLogic;
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break; 
        	case (4):  
                fSensitiveVolume = fCollimatorLogic[1];
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break; 
		    case (5):  
                fSensitiveVolume = fTransparentDetectorHexapodLogic;
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break; 
            case (6):  
                fSensitiveVolume = fTransparentDetectorRack1Logic;
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break; 
            case (7):  
                fSensitiveVolume = fTransparentDetectorRack2Logic;
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break; 
    	}
    }
   
   	//Dose scoring 
   	switch (fScoringIndex) {	
   		case (1):									   
   			fScoringVolume = fCollimatorLogic[1];	
   			break;
   		case (2):
   		   	fScoringVolume = fTransparentDetectorHexapodLogic;
   			break;
   		case (3):
   			fScoringVolume = fTransparentDetectorRack1Logic;
   			break;
   		case (4):
			fScoringVolume = fTransparentDetectorRack2Logic;
   			break; 			
   	    case (5):
			fScoringVolume = fCollimatorLogic[2];
   			break;
   		case (6):
			fScoringVolume = fCollimatorLogic[3];
   			break;
   		case (7):
			fScoringVolume = fCollimatorLogic[4];
   			break;
   		case (8):
			fScoringVolume = fCollimatorLogic[5];
   			break;
   		case (9):
			fScoringVolume = fCollimatorLogic[6];
   			break;
   		case (10):
			fScoringVolume = fCollimatorLogic[7];
   			break;	
   		case (11):
			fScoringVolume = fCollimatorLogic[8];
   			break;	
   		case (12):
			fScoringVolume = fCollimatorLogic[9];
   			break;	
   		case (13):
			fScoringVolume = fCollimatorLogic[10];
   			break;	
   		case (14):
			fScoringVolume = fCollimatorLogic[11];
   			break;	
   		case (15):
			fScoringVolume = fCollimatorLogic[12];
   			break;	
   		case (16):
			fScoringVolume = fCollimatorLogic[13];
   			break;	
   		case (17):
			fScoringVolume = fCollimatorLogic[14];
   			break;				
   	} 
     
   	G4MultiFunctionalDetector* multisd = new G4MultiFunctionalDetector("multisd");
   	G4VPrimitiveScorer* dosedet = new G4PSDoseDeposit("dose");
   	multisd->RegisterPrimitive(dosedet);
   	SetSensitiveDetector(fScoringVolume->GetName(),multisd);
#endif
    
    return fWorldPhysical;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifdef G4MULTITHREADED
void G4ELINP_DetectorConstruction::ConstructSDandField()
{
    
    //Sensitive Volumes
	if(bTransparentDetector){
    	G4VSensitiveDetector* vDetector = new G4ELINP_SensitiveDetector("det");
        G4SDManager::GetSDMpointer()->AddNewDetector(vDetector);
          
        switch (fSensitiveIndex) {
        	case (1):     	
				if(bScoringCSPEC==1 || bScoringCSPEC==2){   
     				fSensitiveVolume = fTransparentCSPECLogic; 
     			}
     			else{  
        			fSensitiveVolume = fTransparentDetectorBoxLogic;
        		} 
        		fSensitiveVolume->SetSensitiveDetector(vDetector);
        		break;
        	case (2):    
        	    if(bScoringCSPEC==0){
        			fSensitiveVolume = fTransparentDetectorBoxLogic;
        		}
        		fSensitiveVolume->SetSensitiveDetector(vDetector);
        		fTransparentDetectorLogic->SetSensitiveDetector(vDetector); 
        		break; 
        	case (3):      
                fSensitiveVolume = fTransparentDetectorLogic;
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break; 
        	case (4):  
                fSensitiveVolume = fCollimatorLogic[1];
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break; 
		    case (5):  
                fSensitiveVolume = fTransparentDetectorHexapodLogic;
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break; 
            case (6):  
                fSensitiveVolume = fTransparentDetectorRack1Logic;
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break; 
            case (7):  
                fSensitiveVolume = fTransparentDetectorRack2Logic;
                fSensitiveVolume->SetSensitiveDetector(vDetector);
                break;
    	}     	
    }
   
   	//Dose scoring 
   	switch (fScoringIndex) {	
   		case (1):									   
   			fScoringVolume = fCollimatorLogic[1];	
   			break;
   		case (2):
   		   	fScoringVolume = fTransparentDetectorHexapodLogic;
   			break;
   		case (3):
   			fScoringVolume = fTransparentDetectorRack1Logic;
   			break;
   		case (4):
			fScoringVolume = fTransparentDetectorRack2Logic;
   			break; 			
   	    case (5):
			fScoringVolume = fCollimatorLogic[2];
   			break;
   		case (6):
			fScoringVolume = fCollimatorLogic[3];
   			break;
   		case (7):
			fScoringVolume = fCollimatorLogic[4];
   			break;
   		case (8):
			fScoringVolume = fCollimatorLogic[5];
   			break;
   		case (9):
			fScoringVolume = fCollimatorLogic[6];
   			break;
   		case (10):
			fScoringVolume = fCollimatorLogic[7];
   			break;	
   		case (11):
			fScoringVolume = fCollimatorLogic[8];
   			break;	
   		case (12):
			fScoringVolume = fCollimatorLogic[9];
   			break;	
   		case (13):
			fScoringVolume = fCollimatorLogic[10];
   			break;	
   		case (14):
			fScoringVolume = fCollimatorLogic[11];
   			break;	
   		case (15):
			fScoringVolume = fCollimatorLogic[12];
   			break;	
   		case (16):
			fScoringVolume = fCollimatorLogic[13];
   			break;	
   		case (17):
			fScoringVolume = fCollimatorLogic[14];
   			break;				
   	}    
     
   	G4MultiFunctionalDetector* multisd = new G4MultiFunctionalDetector("multisd");
   	G4VPrimitiveScorer* dosedet = new G4PSDoseDeposit("dose");
   	multisd->RegisterPrimitive(dosedet);
   	SetSensitiveDetector(fScoringVolume->GetName(),multisd);
}

#else
void G4ELINP_DetectorConstruction::ConstructSDandField(){}
#endif
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
