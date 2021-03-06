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
// $Id: DetectorConstruction.cc,v 1.1 2010-10-18 15:56:17 maire Exp $
// GEANT4 tag $Name: geant4-09-04-patch-02 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "G4RunManager.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4AssemblyVolume.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"
//#include "MagneticField.hh"
//#include "Field.hh"
#include "GlobalField.hh"
#include "G4TransportationManager.hh"
#include "nonUniformMagneticField.hh"

#include "DetectionSystemGammaTracking.hh"
#include "DetectionSystem8pi.hh"
#include "DetectionSystemGriffin.hh"
#include "DetectionSystemSceptar.hh"
#include "DetectionSystemSpice.hh"
#include "DetectionSystemS3.hh"
#include "DetectionSystemPaces.hh"
#include "DetectionSystemNew.hh"
#include "NewSquareDetector.hh"
#include "DetectionSystemSodiumIodide.hh"
#include "DetectionSystemLanthanumBromide.hh"
#include "ApparatusGenericTarget.hh"
#include "ApparatusSpiceTargetChamber.hh"
#include "Apparatus8piVacuumChamber.hh"
#include "Apparatus8piVacuumChamberAuxMatShell.hh"
#include "ApparatusFieldBox.hh"

#include "DetectionSystemBox.hh" // New file
#include "DetectionSystemGrid.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction() :
    // Fields
//    expHallMagField( 0 ), 
//    defaultMaterial( 0 ),
    solidWorld( 0 ), 
    logicWorld( 0 ), 
    physiWorld( 0 )
{

	WorldSizeX  = WorldSizeY = WorldSizeZ = 10.0*m;

  box_mat = "G4_WATER";
  box_thickness = 0.0*mm;
  box_inner_dimensions = G4ThreeVector(0.0*mm,0.0*mm,0.0*mm);
  box_colour = G4ThreeVector(0.0,0.0,1.0);

  grid_mat = "G4_WATER";
  grid_size = 0.0*mm;
  grid_dimensions = G4ThreeVector(0.0*mm,0.0*mm,0.0*mm);
  grid_colour = G4ThreeVector(1.0,0.0,0.0);

  // materials
  DefineMaterials();

//  this->builtDetectors = false;

  // ensure the global field is initialized
  (void)GlobalField::getObject();
  
  this->matWorldName = "G4_AIR";

  // Generic Target Apparatus
  this->setGenericTargetMaterial   = false;
  this->setGenericTargetDimensions = false;
  this->setGenericTargetPosition   = false;

  // Field Box
  this->setFieldBoxMaterial= false;
  this->setFieldBoxDimensions= false;
  this->setFieldBoxPosition= false;
  this->setFieldBoxMagneticField= false;

  // parameters to suppress:

  DefineSuppressedParameters();
  
  // Shield Selection Default

  this->useTigressPositions = false;

  this->detectorShieldSelect = 1 ; // Include suppressors by default. 
  this->extensionSuppressorLocation = 0 ; // Back by default (Detector Forward)
  this->hevimetSelector = 0 ; // Chooses whether or not to include a hevimet

  this->customDetectorNumber 		= 1 ; // det_num
  this->customDetectorPosition  = 1 ; // pos_num
  this->customDetectorVal				= 0 ; // Unused for now (Oct 2013)


  // create commands for interactive definition

  detectorMessenger = new DetectorMessenger(this);

  // ensure the global field is initialized
  //(void)GlobalField::getObject();

  //expHallMagField = new MagneticField(); // Global field is set to zero

  griffinDetectorsMapIndex = 0;
  for(G4int i = 0; i < 16; i++)
  {
      griffinDetectorsMap[i] = 0;
  }


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ 
  delete detectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{

	// Replaced by ConstructDetectionSystems
	
  // Experimental hall (world volume)
  // search the world material by its name
  
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  
  G4Material* matWorld = G4Material::GetMaterial(matWorldName);
  
	if( !matWorld ) {
    G4cout << " ----> Material " << matWorldName << " not found, cannot build world! " << G4endl;
    return 0;
  }
  
  solidWorld = new G4Box("World", WorldSizeX/2,WorldSizeY/2,WorldSizeZ/2);
  
  logicWorld = new G4LogicalVolume(solidWorld,		//its solid
                                   matWorld,	//its material
                                   "World");		//its name
  
  physiWorld = new G4PVPlacement(   0,                  //no rotation
                                    G4ThreeVector(),	//at (0,0,0)
                                    logicWorld,         //its logical volume
                                    "World",            //its name
                                    0,                  //its mother  volume
                                    false,              //no boolean operation
                                    0);                 //copy number  
  
  // Visualization Attributes

  logicWorld->SetVisAttributes (G4VisAttributes::Invisible); // The following block of code works too. 
  
//  G4VisAttributes* worldVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
//  worldVisAtt->SetForceWireframe(true);
//  worldVisAtt->SetVisibility(this->world_vis);
//  logicWorld->SetVisAttributes(worldVisAtt);
//  this->logicWorld = logicWorld; 
  
  return physiWorld ; 
  

}

void DetectorConstruction::SetWorldMaterial( G4String name )
{
  this->matWorldName = name;
  UpdateGeometry(); // auto update
}

void DetectorConstruction::SetWorldDimensions( G4ThreeVector vec )
{
	WorldSizeX = vec.x() ;
	WorldSizeY = vec.y() ; 
	WorldSizeZ = vec.z() ;
  UpdateGeometry(); // auto update
}

void DetectorConstruction::SetWorldVis( G4bool vis )
{
  this->world_vis = vis;
  UpdateGeometry(); // auto update
}

void DetectorConstruction::SetWorldMagneticField( G4ThreeVector vec )
{
    //expHallMagField->SetFieldValue(G4ThreeVector(vec.x(),vec.y(),vec.z()));
}

void DetectorConstruction::SetTabMagneticField(G4String PathAndTableName, G4double z_offset, G4double z_rotation)
{
  nonUniformMagneticField* tabulatedField = new nonUniformMagneticField(PathAndTableName,z_offset,z_rotation);
}

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}

void DetectorConstruction::SetGenericTargetMaterial( G4String name )
{
  this->setGenericTargetMaterial = true;
  this->genericTargetMaterial = name;
  SetGenericTarget();
}

void DetectorConstruction::SetGenericTargetDimensions( G4ThreeVector vec )
{
  this->setGenericTargetDimensions = true;
  this->genericTargetDimensions = vec;
  SetGenericTarget();
}

void DetectorConstruction::SetGenericTargetPosition( G4ThreeVector vec )
{
  this->setGenericTargetPosition = true;
  this->genericTargetPosition = vec;
  SetGenericTarget();
}

void DetectorConstruction::SetGenericTarget()
{
  if( this->setGenericTargetMaterial ) 
  {
    if( this->setGenericTargetDimensions ) 
    {
      if( this->setGenericTargetPosition ) 
      {
        G4String name = this->genericTargetMaterial;
        G4double vec_x = this->genericTargetDimensions.x()/mm;
        G4double vec_y = this->genericTargetDimensions.y()/mm;
        G4double vec_z = this->genericTargetDimensions.z()/mm;
        ApparatusGenericTarget* pApparatusGenericTarget = new ApparatusGenericTarget();
        pApparatusGenericTarget->Build(name, vec_x, vec_y, vec_z);
        G4RotationMatrix* rotate = new G4RotationMatrix;
        pApparatusGenericTarget->PlaceApparatus(logicWorld, this->genericTargetPosition, rotate);
      }
 		}
	}
}
void DetectorConstruction::SetFieldBoxMaterial( G4String name )
{
  this->setFieldBoxMaterial = true;
  this->fieldBoxMaterial = name;
  SetFieldBox();
}

void DetectorConstruction::SetFieldBoxDimensions( G4ThreeVector vec )
{
  this->setFieldBoxDimensions = true;
  this->fieldBoxDimensions = vec;
  SetFieldBox();
}

void DetectorConstruction::SetFieldBoxPosition( G4ThreeVector vec )
{
  this->setFieldBoxPosition = true;
  this->fieldBoxPosition = vec;
  SetFieldBox();
}

void DetectorConstruction::SetFieldBoxMagneticField( G4ThreeVector vec )
{
  this->setFieldBoxMagneticField = true;
  this->fieldBoxMagneticField = vec;
  SetFieldBox();
}

void DetectorConstruction::SetFieldBox( )
{
  if( this->setFieldBoxMagneticField && this->setFieldBoxMaterial && 
  		this->setFieldBoxDimensions 	 && this->setFieldBoxPosition   ) 
		{
			G4String name = this->fieldBoxMaterial;
			G4double vec_x = this->fieldBoxDimensions.x()/mm;
			G4double vec_y = this->fieldBoxDimensions.y()/mm;
			G4double vec_z = this->fieldBoxDimensions.z()/mm;
			ApparatusFieldBox* pApparatusFieldBox = new ApparatusFieldBox();
			pApparatusFieldBox->Build(name, vec_x, vec_y, vec_z, this->fieldBoxMagneticField);
			G4RotationMatrix* rotate = new G4RotationMatrix;
			pApparatusFieldBox->PlaceApparatus(logicWorld, this->fieldBoxPosition, rotate);
		}
}

void DetectorConstruction::AddBox()
{
    if(box_thickness != 0.0*mm) 
    {
    	DetectionSystemBox* pBox = new DetectionSystemBox(	box_inner_dimensions.x(),
        																									box_inner_dimensions.y(), 
        																									box_inner_dimensions.z(), 
        																									box_thickness, 
        																									box_mat, 
        																									box_colour ) ;
        pBox->Build() ;
        pBox->PlaceDetector( logicWorld ) ;
    }
}

void DetectorConstruction::AddGrid()
{
    if(grid_size != 0.0*mm) 
    {
    	DetectionSystemGrid* pGrid = new DetectionSystemGrid(	grid_dimensions.x(),
        																										grid_dimensions.y(), 
        																										grid_dimensions.z(), 
        																										grid_size, 
        																										grid_mat, 
        																										grid_colour ) ;
        pGrid->Build();
        pGrid->PlaceDetector( logicWorld );
    }
}

void DetectorConstruction::AddApparatusSpiceTargetChamber()
{
   //Create Target Chamber
   ApparatusSpiceTargetChamber* pApparatusSpiceTargetChamber = new ApparatusSpiceTargetChamber();
   pApparatusSpiceTargetChamber->Build( logicWorld );
    
}

void DetectorConstruction::AddApparatus8piVacuumChamber()
{
   //Create Vacuum Chamber
   Apparatus8piVacuumChamber* pApparatus8piVacuumChamber = new Apparatus8piVacuumChamber();
   pApparatus8piVacuumChamber->Build( logicWorld );
}

void DetectorConstruction::AddApparatus8piVacuumChamberAuxMatShell(G4double thickness)
{
   //Create Shell Around Vacuum Chamber
   Apparatus8piVacuumChamberAuxMatShell* pApparatus8piVacuumChamberAuxMatShell = new Apparatus8piVacuumChamberAuxMatShell();
   pApparatus8piVacuumChamberAuxMatShell->Build( logicWorld, thickness );
}

void DetectorConstruction::AddDetectionSystemGammaTracking(G4int ndet)
{
  // Describe Placement
  G4ThreeVector direction = G4ThreeVector(0,0,1);
  G4ThreeVector move = 0.0 * direction;
  G4RotationMatrix* rotate = new G4RotationMatrix;
  rotate->rotateX(0.0);
  rotate->rotateY(0.0);
  rotate->rotateZ(0.0);

  G4int detector_number = 0;
	
	DetectionSystemGammaTracking* pGammaTracking = new DetectionSystemGammaTracking() ;
	pGammaTracking->Build() ;	
	
  pGammaTracking->PlaceDetector( logicWorld, move, rotate, detector_number );
}

void DetectorConstruction::AddDetectionSystemSodiumIodide(G4int ndet)
{
  // Describe Placement
  G4double detectorAngles[8][2] = {0};
  G4double theta,phi,position;
  G4ThreeVector move,direction;

  detectorAngles[0][0] 	= 0.0;
  detectorAngles[1][0] 	= 45.0;
  detectorAngles[2][0] 	= 90.0;
  detectorAngles[3][0] 	= 135.0;
  detectorAngles[4][0] 	= 180.0;
  detectorAngles[5][0] 	= 225.0;
  detectorAngles[6][0] 	= 270.0;
  detectorAngles[7][0] 	= 315.0;
  detectorAngles[0][1] 	= 90.0;
  detectorAngles[1][1] 	= 90.0;
  detectorAngles[2][1] 	= 90.0;
  detectorAngles[3][1] 	= 90.0;
  detectorAngles[4][1] 	= 90.0;
  detectorAngles[5][1] 	= 90.0;
  detectorAngles[6][1] 	= 90.0;
  detectorAngles[7][1] 	= 90.0;

  DetectionSystemSodiumIodide* pSodiumIodide = new DetectionSystemSodiumIodide() ;
  pSodiumIodide->Build() ;

  for(G4int detector_number = 0; detector_number < ndet; detector_number++)
  {
    phi = detectorAngles[detector_number][0]*deg; // Creates a ring in phi plane
    theta = detectorAngles[detector_number][1]*deg;

    direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
    position = 25.0*cm + (pSodiumIodide->GetDetectorLengthOfUnitsCM()/2.0);
    move = position * direction;

    G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector
    rotate->rotateX(theta);
    rotate->rotateY(0);
    rotate->rotateZ(phi+0.5*M_PI);

    pSodiumIodide->PlaceDetector( logicWorld, move, rotate, detector_number ) ;
  }
}

void DetectorConstruction::AddDetectionSystemLanthanumBromide(G4int ndet)
{
    DetectionSystemLanthanumBromide* pDetectionSystemLanthanumBromide = new DetectionSystemLanthanumBromide();
    pDetectionSystemLanthanumBromide->Build();

  for(G4int detector_number = 0; detector_number < ndet; detector_number++)
  {
    pDetectionSystemLanthanumBromide->PlaceDetector(logicWorld, detector_number);
  }
}

// Temporary Function for testing purposes
void DetectorConstruction::AddDetectionSystemGriffinCustomDetector( G4int ndet = 0 ){

  griffinDetectorsMap[griffinDetectorsMapIndex] = this->customDetectorNumber ; 
  griffinDetectorsMapIndex++;


	// NOTE: ndet served no purpose in this case but I left it in just in case this needs to be modified later. The position of a detector placed using this function must be set using
	// SetDeadLayer. 

  DetectionSystemGriffin* pGriffinCustom = new DetectionSystemGriffin( this->extensionSuppressorLocation , this->detectorShieldSelect, this->detectorRadialDistance, this->hevimetSelector ); // Select Forward (0) or Back (1)

  pGriffinCustom->BuildDeadLayerSpecificCrystal(this->customDetectorNumber-1);

  pGriffinCustom->PlaceDeadLayerSpecificCrystal( logicWorld, this->customDetectorNumber-1, this->customDetectorPosition-1, useTigressPositions ) ;

  pGriffinCustom->BuildEverythingButCrystals();

  pGriffinCustom->PlaceEverythingButCrystals( logicWorld, this->customDetectorNumber-1, this->customDetectorPosition-1, useTigressPositions ) ;


}

void DetectorConstruction::AddDetectionSystemGriffinCustom(G4int ndet)
{

    G4int det_num;
    G4int pos_num;

    for( det_num = 1; det_num <= ndet; det_num++ ) {
        pos_num = det_num;

        griffinDetectorsMap[griffinDetectorsMapIndex] = det_num;
        griffinDetectorsMapIndex++;

        DetectionSystemGriffin* pGriffinCustom = new DetectionSystemGriffin( this->extensionSuppressorLocation,  this->detectorShieldSelect ,  this->detectorRadialDistance, this->hevimetSelector ) ; // Select Forward (0) or Back (1)

        pGriffinCustom->BuildDeadLayerSpecificCrystal(det_num-1);
        pGriffinCustom->PlaceDeadLayerSpecificCrystal( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;
        pGriffinCustom->BuildEverythingButCrystals();
        pGriffinCustom->PlaceEverythingButCrystals( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;

    }
}

void DetectorConstruction::AddDetectionSystemGriffinShieldSelect( G4int ShieldSelect ){
  this->detectorShieldSelect = ShieldSelect ; 
}

void DetectorConstruction::AddDetectionSystemGriffinSetRadialDistance( G4double detectorDist ){
  this->detectorRadialDistance = detectorDist ; 
}

void DetectorConstruction::AddDetectionSystemGriffinSetExtensionSuppLocation( G4int detectorPos ){
  this->extensionSuppressorLocation = detectorPos ; 
}

void DetectorConstruction::AddDetectionSystemGriffinSetDeadLayer( G4ThreeVector params )
{

  this->customDetectorNumber 		= (G4int)params.x(); // det_num
  this->customDetectorPosition  = (G4int)params.y(); // pos_num
  this->customDetectorVal			  = (G4int)params.z(); // Unused at the moment. 

}

void DetectorConstruction::AddDetectionSystemGriffinForward(G4int ndet)
{
//  G4double theta,phi,position;
//  G4ThreeVector move,direction;

//  DetectionSystemGriffin* pGriffinForward = new DetectionSystemGriffin(0, 1, this->griffinFwdBackPosition); // Select Forward (0) or Back (1)
//  pGriffinForward->Build();

//  for( G4int detector_number = 0; detector_number < ndet; detector_number++ )
//  {
//    direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
//    position = this->griffinFwdBackPosition;
//    move = position * direction;

//    G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector

//    pGriffinForward->PlaceDetector( logicWorld, move, rotate, detector_number ) ;
//  }

    G4int det_num;
    G4int pos_num;
    G4int config  = 0;

    for( det_num = 1; det_num <= ndet; det_num++ ) {
        pos_num = det_num;

        griffinDetectorsMap[griffinDetectorsMapIndex] = det_num;
        griffinDetectorsMapIndex++;

        DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, this->griffinFwdBackPosition, this->hevimetSelector); // Select Forward (0) or Back (1)

        pGriffinDLS->BuildDeadLayerSpecificCrystal(det_num-1);
        pGriffinDLS->PlaceDeadLayerSpecificCrystal( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;
        pGriffinDLS->BuildEverythingButCrystals();
        pGriffinDLS->PlaceEverythingButCrystals( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;

    }
}

void DetectorConstruction::AddDetectionSystemGriffinForwardDetector(G4int ndet)
{
//  G4double theta,phi,position;
//  G4ThreeVector move,direction;


//  DetectionSystemGriffin* pGriffinForward = new DetectionSystemGriffin(0, 1, this->griffinFwdBackPosition); // Select Forward (0) or Back (1)
//  pGriffinForward->Build();

//  direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
//  position = this->griffinFwdBackPosition;
//  move = position * direction;

//  G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector

//  pGriffinForward->PlaceDetector( logicWorld, move, rotate, ndet ) ;


  G4int det_num = ndet;
  G4int pos_num = ndet;
  G4int config  = 0;
  griffinDetectorsMap[griffinDetectorsMapIndex] = det_num;
  griffinDetectorsMapIndex++;


  DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, this->griffinFwdBackPosition, this->hevimetSelector ); // Select Forward (0) or Back (1)


  pGriffinDLS->BuildDeadLayerSpecificCrystal(det_num-1);

  pGriffinDLS->PlaceDeadLayerSpecificCrystal( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;

  pGriffinDLS->BuildEverythingButCrystals();

  pGriffinDLS->PlaceEverythingButCrystals( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;


}

void DetectorConstruction::AddDetectionSystemGriffinBack(G4int ndet)
{
//  G4double theta,phi,position;
//  G4ThreeVector move,direction;


//  DetectionSystemGriffin* pGriffinBack = new DetectionSystemGriffin(1, 1, this->griffinFwdBackPosition ) ; // Select Forward (0) or Back (1)
//  pGriffinBack->Build();

//  for(G4int detector_number = 0; detector_number < ndet; detector_number++)
//  {
//    direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
//    position = this->griffinFwdBackPosition;
//    move = position * direction;

//    G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector

//    pGriffinBack->PlaceDetector( logicWorld, move, rotate, detector_number ) ;
//  }

  G4int det_num;
  G4int pos_num;
  G4int config  = 1;

  for( det_num = 1; det_num <= ndet; det_num++ ) {
      pos_num = det_num;

      griffinDetectorsMap[griffinDetectorsMapIndex] = det_num;
      griffinDetectorsMapIndex++;

      DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, this->griffinFwdBackPosition, this->hevimetSelector ); // Select Forward (0) or Back (1)

      pGriffinDLS->BuildDeadLayerSpecificCrystal(det_num-1);
      pGriffinDLS->PlaceDeadLayerSpecificCrystal( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;
      pGriffinDLS->BuildEverythingButCrystals();
      pGriffinDLS->PlaceEverythingButCrystals( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;

  }

}

void DetectorConstruction::AddDetectionSystemGriffinBackDetector(G4int ndet)
{
//  G4double theta,phi,position;
//  G4ThreeVector move,direction;


//  DetectionSystemGriffin* pGriffinBack = new DetectionSystemGriffin(1, 1, this->griffinFwdBackPosition ); // Select Forward (0) or Back (1)
//  pGriffinBack->Build();

//  direction = G4ThreeVector(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
//  position = this->griffinFwdBackPosition;
//  move = position * direction;

//  G4RotationMatrix* rotate = new G4RotationMatrix; 		//rotation matrix corresponding to direction vector

//  pGriffinBack->PlaceDetector( logicWorld, move, rotate, ndet ) ;

    G4int det_num = ndet;
    G4int pos_num = ndet;
    G4int config  = 1;

    griffinDetectorsMap[griffinDetectorsMapIndex] = det_num;
    griffinDetectorsMapIndex++;

    DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, this->griffinFwdBackPosition, this->hevimetSelector); // Select Forward (0) or Back (1)

    pGriffinDLS->BuildDeadLayerSpecificCrystal(det_num-1);
    pGriffinDLS->PlaceDeadLayerSpecificCrystal( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;
    pGriffinDLS->BuildEverythingButCrystals();
    pGriffinDLS->PlaceEverythingButCrystals( logicWorld, det_num-1, pos_num-1, useTigressPositions ) ;
}

void DetectorConstruction::AddDetectionSystemGriffinHevimet(G4int input)
{
  // Includes hevimet. 
  this->hevimetSelector = input ; 

}

// This will be reaplced with the addGriffinCustomDetector function. The dead layer must be set using
// the SetCustomDeadLayer command. This will take longer for many different detectors in different configurations, 
// but it is possible to place multiple custom detectors using addGriffinCustom as well. 
//void DetectorConstruction::AddDetectionSystemGriffinPositionConfig(G4ThreeVector input)
//{
//  G4int det_num = (G4int)input.x();
//  G4int pos_num = (G4int)input.y();
//  G4int config  = (G4int)input.z();


////  DetectionSystemGriffin* pGriffinBack = new DetectionSystemGriffin( config, 1, this->griffinFwdBackPosition ); // Select Forward (0) or Back (1)
////  pGriffinBack->BuildDeadLayerSpecificDetector(det_num-1);
////  pGriffinBack->PlaceDeadLayerSpecificDetector( logicWorld, det_num-1, pos_num-1 ) ;

//  griffinDetectorsMap[griffinDetectorsMapIndex] = det_num;
//  griffinDetectorsMapIndex++;

//  DetectionSystemGriffin* pGriffinDLS = new DetectionSystemGriffin(config, 1, this->griffinFwdBackPosition); // Select Forward (0) or Back (1)

//  pGriffinDLS->BuildDeadLayerSpecificCrystal(det_num-1);
//  pGriffinDLS->PlaceDeadLayerSpecificCrystal( logicWorld, det_num-1, pos_num-1 ) ;
//  pGriffinDLS->BuildEverythingButCrystals();
//  pGriffinDLS->PlaceEverythingButCrystals( logicWorld, det_num-1, pos_num-1 ) ;

//}


void DetectorConstruction::AddDetectionSystemSceptar(G4int ndet)
{

  DetectionSystemSceptar* pSceptar = new DetectionSystemSceptar() ;
  pSceptar->Build() ;
	
  pSceptar->PlaceDetector( logicWorld, ndet ) ;
}

void DetectorConstruction::SetSpiceResolutionVariables(G4double intercept, G4double gain)
{
	RootManager::SpiceResolution[0] = intercept;
	RootManager::SpiceResolution[1] = gain;

}

void DetectorConstruction::AddDetectionSystemSpice(G4int nRings)
{

	DetectionSystemSpice* pSpice = new DetectionSystemSpice() ;
	pSpice->Build(); 
	
  G4int NumberSeg = 12; // Segments in Phi
  G4int segmentID=0;
  G4double annularDetectorDistance = 115*mm /*+ 150*mm*/;
  G4ThreeVector pos(0,0,-annularDetectorDistance); 

  pSpice->PlaceDetectorMount(logicWorld,pos);
  pSpice->PlaceAnnularClamps(logicWorld,pos);   
  pSpice->PlaceGuardRing(logicWorld, pos);

  for(int ring = 0; ring<nRings; ring++)
    {
      for(int Seg=0; Seg<NumberSeg; Seg++)
		{
		  pSpice->PlaceDetector(logicWorld, pos, ring, Seg, segmentID);
		  segmentID++;
		} // end for(int Seg=0; Seg<NumberSeg; Seg++)
    } // end for(int ring = 0; ring<nRings; ring++)
}

void DetectorConstruction::AddDetectionSystemS3(G4int nRings = 24, G4double posX = 0 , G4double posY = 0 , G4int posZ = 21, G4double AngleOffset = 20)
{
	DetectionSystemS3* pS3 = new DetectionSystemS3();
	pS3->Build();
	
	G4int detID=0;
	G4int NumberSeg = 32; // Segments in Phi
	
	G4ThreeVector pos(posX*mm,posY*mm,posZ*mm); 
	pS3->PlaceS3Mount(logicWorld, pos, AngleOffset);
  	pS3->PlaceGuardRing(logicWorld, pos);
  	
  for(int ring = 0; ring<nRings; ring++)
	{
		for(int Seg=0; Seg<NumberSeg; Seg++)
		{
			pS3->PlaceDetector(logicWorld, pos, AngleOffset , ring, Seg, detID);
			detID++;
		} // end for(int Seg=0; Seg<NumberSeg; Seg++)
	} // end for(int ring = 0; ring<nRings; ring++)
}

void DetectorConstruction::AddDetectionSystemPaces(G4int ndet)
{
	DetectionSystemPaces* pPaces = new DetectionSystemPaces() ; 
	pPaces->Build() ;
	
	pPaces->PlaceDetector( logicWorld, ndet ) ;
}

void DetectorConstruction::AddDetectionSystemNew(G4int ndet)
{
   //Create Target Chamber
   DetectionSystemNew* pNew = new DetectionSystemNew();
   pNew->Build( logicWorld );
//   pNew->Place( logicWorld) ;

}

void DetectorConstruction::AddNewSquareDetector(G4int nDet)
{
  	NewSquareDetector* pNewSquare = new NewSquareDetector();
	pNewSquare->Build();
	
  
    for(int detector = 1; detector<(nDet+1); detector++)
    {
        pNewSquare->PlaceGuardRing(logicWorld, detector);


        pNewSquare->PlaceDetector(logicWorld, detector);
		
    }
} //end NewSquareDetector


