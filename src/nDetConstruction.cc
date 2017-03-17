//
// $Id: nDetConstruction.cc,v1.0 Sept., 2015 $
// Written by Dr. Xiaodong Zhang
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include <G4UnionSolid.hh>
#include "nDetConstruction.hh"
#include "G4SDManager.hh"

#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4GeometryManager.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


nDetConstruction::nDetConstruction()
: expHall_logV(NULL), expHall_physV(NULL)

, assembly_logV(NULL), assembly_physV(NULL)
, assemblyBoxX(15.11*mm) // reducing size to eliminate non-physical PTFE.  Noticed excessive scattering of flourine and carbon.
//, assemblyBoxX(16.66667*mm) //10x30 bars KS 5/23/16
//, assemblyBoxX(50*mm)	// 30X30 scintillator bars, each has a dimension of 3X100X3 mm3
, assemblyBoxY(51.2*mm) // length of scintillator: 100 mm + 0.1 mm thick grease + 1 mm thick SiPM window + 0.1 mm thick sensitive part of SiPM on both ends of scintillator
, assemblyBoxZ(45.31*mm) // closing gaps.  Teflon tape measures ~.02 mm thick.
//, assemblyBoxZ(50*mm)	// the gaps of scintillators are filled with reflectors, such as Teflon

,ej200_logV(NULL), ej200X(1.5*mm), ej200Y(50*mm), ej200Z(1.5*mm)

,grease_logV(NULL), greaseX(1.5*mm), greaseY(0.05*mm), greaseZ(1.5*mm) // 100 micron thick grease on both ends of EJ200 bar

,qwSiPM_logV(NULL), qwSiPMx(1.5*mm), qwSiPMy(0.5*mm), qwSiPMz(1.5*mm)  // 1 mm thick SiPM window

,psSiPM_logV(NULL), psSiPMx(1.5*mm), psSiPMy(0.05*mm), psSiPMz(1.5*mm) // note that two sensing parts on both ends of scintillator are the same
{

  // the world volume is 10 mm bigger than assembly volume in three dimensions
  G4double margin = 20*mm;

  fDetectorMessenger=new nDetConstructionMessenger(this);

  fGeometry="hexagon";

  expHallX = assemblyBoxX + margin;
  expHallY = assemblyBoxY + margin;
  expHallZ = assemblyBoxZ + margin;

    //Build the materials
    DefineMaterials();


    //Make Sensitive Detectors

  //G4SDManager* SDMan = G4SDManager::GetSDMpointer();




  return;
} // end of construction function.

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetConstruction::~nDetConstruction()
{} // end of deconstruction function.

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* nDetConstruction::Construct()
{

    G4cout<<"nDetConstruction::Construct()-->"<<G4endl;

    if (expHall_physV) {
        G4GeometryManager::GetInstance()->OpenGeometry();
        G4PhysicalVolumeStore::GetInstance()->Clean();
        G4LogicalVolumeStore::GetInstance()->Clean();
        G4SolidStore::GetInstance()->Clean();
        //G4LogicalSkinSurface::CleanSurfaceTable();
        //G4LogicalBorderSurface::CleanSurfaceTable();
    }


    // build experiment hall
  buildExpHall();

  // build assembly volume
  //buildAssembly();

  //build Disk volume
    if(fGeometry == "disk" || fGeometry == "hexagon")
        buildDisk();
    //builds the Ellipse
    if(fGeometry == "ellipse" || fGeometry == "rectangle")
        buildEllipse();

  return expHall_physV;
}

void nDetConstruction::buildExpHall()
{
  // **************** expHalll **************
  //      ------------- Volumes --------------
  
  G4Box* expHall_solidV = new G4Box("expHall_solidV",expHallX,expHallY,expHallZ);

  expHall_logV  = new G4LogicalVolume(expHall_solidV, fAir, "expHall_logV",0,0,0);
 // expHall_logV  = new G4LogicalVolume(expHall_solidV, Vacuum, "expHall_logV",0,0,0);
  expHall_logV->SetVisAttributes(G4VisAttributes::Invisible);

  expHall_physV = new G4PVPlacement(0,G4ThreeVector(0., 0., 0.),expHall_logV,
                                   "expHall_physV",0,false,0);
 
  return;
} // end of buildExpHall function


// build scintillator array that consists of 10X30 EJ200 scintillator bars
// Each bar is read out by two SiPMs at the two ends
// Teflon tape is used to wrap the bars and take care diffusing light back into the scintillator
void nDetConstruction::buildAssembly()
{ 
  //************* Wrapping materials (such as Teflon) and EJ200  *************

  //-----------------------------------------------------------
  //     The assembly volume is defined as a wrapping material
  //-----------------------------------------------------------

   G4Box* assemblyBox = new G4Box("assembly", assemblyBoxX, assemblyBoxY, assemblyBoxZ);
  assembly_logV = new G4LogicalVolume(assemblyBox, fTeflon, "assembly");
  assembly_physV = new G4PVPlacement(0,
				     G4ThreeVector(assemblyPx, assemblyPy, assemblyPz),
 				     assembly_logV,
				     "assembly_physV",
				     expHall_logV,
				     false, 0);

  fWrapSkinSurface = new G4LogicalSkinSurface("teflonSurface", assembly_logV, fTeflonOpticalSurface);


  //----------------------------------------------------------------
  //    scintillator bars
  //----------------------------------------------------------------

  G4Box* ej200_solidV = new G4Box("ej200_solidV", ej200X, ej200Y, ej200Z);

  ej200_logV = new G4LogicalVolume(ej200_solidV, fEJ200, "ej200_logV", 0, 0, 0);

  G4VisAttributes* ej200_VisAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0));//blue
  ej200_logV->SetVisAttributes(ej200_VisAtt);

  // create assembly of scintillator bars
  //
  G4AssemblyVolume* ej200Assembly = new G4AssemblyVolume();
  // Rotation and translation of the Fiber logical volume
  G4RotationMatrix rotationMatrix_ej200;
  G4ThreeVector position_ej200(0., 0., 0.);
  G4Transform3D transform3D_ej200;

//  for(G4int x_index = 0; x_index<30; x_index++){
  for(G4int x_index = 0; x_index<10; x_index++){ //reduced array size to 10x30 KS 5/23/16
    for(G4int z_index = 0; z_index<30; z_index++){
      //position_ej200.setX(assemblyBoxX - assemblyBoxX/15*(1+x_index*2)/2);
      position_ej200.setX(assemblyBoxX - assemblyBoxX/5*(1+x_index*2)/2); //reduced KS 5/23/16
      position_ej200.setZ(assemblyBoxZ - assemblyBoxZ/15*(1+z_index*2)/2);
      position_ej200.setY(0.0);

      transform3D_ej200 = G4Transform3D(rotationMatrix_ej200, position_ej200);
      ej200Assembly->AddPlacedVolume(ej200_logV, transform3D_ej200);
    }
  }


   /*   position_ej200.setX(0.0); //reduced KS 5/23/16
      position_ej200.setZ(0.0);
      position_ej200.setY(0.0);
   */
      transform3D_ej200 = G4Transform3D(rotationMatrix_ej200, position_ej200);
      ej200Assembly->AddPlacedVolume(ej200_logV, transform3D_ej200);

  //  place the assembly
  //
  position_ej200.setX(0.0); position_ej200.setY(0.0); position_ej200.setZ(0.0);
  transform3D_ej200 = G4Transform3D(rotationMatrix_ej200, position_ej200);
  ej200Assembly->MakeImprint(assembly_logV, transform3D_ej200);

  // grease Sanit-Gobain BC-630 silicone grease
  // optical grease

  // grease box ...
  G4Box* grease_solidV = new G4Box("grease", greaseX, greaseY, greaseZ);
  grease_logV = new G4LogicalVolume(grease_solidV, fGrease, "grease_logV", 0, 0, 0);
  G4VisAttributes* grease_VisAtt= new G4VisAttributes(G4Colour(1.0,0.0,0.0));//red
  grease_logV->SetVisAttributes(grease_VisAtt);

  // create assembly of grease
  //
  G4AssemblyVolume* greaseAssembly = new G4AssemblyVolume();
  // Rotation and translation of the grease logical volume
  G4RotationMatrix rotationMatrix_grease;
  G4ThreeVector position_grease(0., 0., 0.);
  G4Transform3D transform3D_grease;

  //for(G4int x_index = 0; x_index<30; x_index++){
  for(G4int x_index = 0; x_index<10; x_index++){
    for(G4int z_index = 0; z_index<30; z_index++){
      //position_grease.setX(assemblyBoxX - assemblyBoxX/15*(1+x_index*2)/2);
      position_grease.setX(assemblyBoxX - assemblyBoxX/5*(1+x_index*2)/2);
      position_grease.setZ(assemblyBoxZ - assemblyBoxZ/15*(1+z_index*2)/2);
      position_grease.setY(ej200Y+greaseY);

      transform3D_grease = G4Transform3D(rotationMatrix_grease, position_grease);
      greaseAssembly->AddPlacedVolume(grease_logV, transform3D_grease);

      position_grease.setY(-1*(ej200Y+greaseY));

      transform3D_grease = G4Transform3D(rotationMatrix_grease, position_grease);
      greaseAssembly->AddPlacedVolume(grease_logV, transform3D_grease);
    }
  }

  //  place the assembly
  //
  position_grease.setX(0.0); position_grease.setY(0.0); position_grease.setZ(0.0);
  transform3D_grease = G4Transform3D(rotationMatrix_grease, position_grease);
  greaseAssembly->MakeImprint(assembly_logV, transform3D_grease);


  G4Box* qwSiPM_solidV = new G4Box("qwSiPM_solidV", qwSiPMx, qwSiPMy, qwSiPMz);

  qwSiPM_logV = new G4LogicalVolume(qwSiPM_solidV, fSiO2, "qwSiPM_logV", 0, 0, 0);
  G4VisAttributes* qwSiPM_VisAtt= new G4VisAttributes(G4Colour(0.0,1.0,0.0));//green
  qwSiPM_logV->SetVisAttributes(qwSiPM_VisAtt);

  // create assembly of SiPM window
  //
  G4AssemblyVolume* SiPMwindowAssembly = new G4AssemblyVolume();
  // Rotation and translation of the grease logical volume
  G4RotationMatrix rotationMatrix_SiPMwindow;
  G4ThreeVector position_SiPMwindow(0., 0., 0.);
  G4Transform3D transform3D_SiPMwindow;

//  for(G4int x_index = 0; x_index<30; x_index++){
  for(G4int x_index = 0; x_index<10; x_index++){
    for(G4int z_index = 0; z_index<30; z_index++){
      //position_SiPMwindow.setX(assemblyBoxX - assemblyBoxX/15*(1+x_index*2)/2);
      position_SiPMwindow.setX(assemblyBoxX - assemblyBoxX/5*(1+x_index*2)/2);
      position_SiPMwindow.setZ(assemblyBoxZ - assemblyBoxZ/15*(1+z_index*2)/2);
      position_SiPMwindow.setY(ej200Y+2*greaseY+qwSiPMy);

      transform3D_SiPMwindow = G4Transform3D(rotationMatrix_SiPMwindow, position_SiPMwindow);
      SiPMwindowAssembly->AddPlacedVolume(qwSiPM_logV, transform3D_SiPMwindow);

      position_SiPMwindow.setY(-1*(ej200Y+2*greaseY+qwSiPMy));

      transform3D_SiPMwindow = G4Transform3D(rotationMatrix_SiPMwindow, position_SiPMwindow);
      SiPMwindowAssembly->AddPlacedVolume(qwSiPM_logV, transform3D_SiPMwindow);
    }
  }

  //  place the assembly
  //
  position_SiPMwindow.setX(0.0); position_SiPMwindow.setY(0.0); position_SiPMwindow.setZ(0.0);
  transform3D_SiPMwindow = G4Transform3D(rotationMatrix_SiPMwindow, position_SiPMwindow);
  SiPMwindowAssembly->MakeImprint(assembly_logV, transform3D_SiPMwindow);

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // the portion of SiPM just following the front quartz window is used for sensing the photons
  // the material of this portion or photon sensing part is made of silicon.
  //


  G4Box* psSiPM_solidV = new G4Box("psSiPM_solidV", psSiPMx, psSiPMy, psSiPMz);


  psSiPM_logV = new G4LogicalVolume(psSiPM_solidV, fSil, "psSiPM_logV", 0, 0, 0);
  G4VisAttributes* psSiPM_VisAtt= new G4VisAttributes(G4Colour(0.0,1.0,1.0));//
  psSiPM_logV->SetVisAttributes(psSiPM_VisAtt);


    fSiPMSkinSurface = new G4LogicalSkinSurface("SiPMSurface", psSiPM_logV,fSiliconPMOpticalSurface);

  // create assembly of SiPM sensitive part
  //
  G4AssemblyVolume* psSiPMAssembly = new G4AssemblyVolume();
  // Rotation and translation of the grease logical volume
  G4RotationMatrix rotationMatrix_psSiPM;
  G4ThreeVector position_psSiPM(0., 0., 0.);
  G4Transform3D transform3D_psSiPM;

//  for(G4int x_index = 0; x_index<30; x_index++){
  for(G4int x_index = 0; x_index<10; x_index++){
    for(G4int z_index = 0; z_index<30; z_index++){
//      position_psSiPM.setX(assemblyBoxX - assemblyBoxX/15*(1+x_index*2)/2);
      position_psSiPM.setX(assemblyBoxX - assemblyBoxX/5*(1+x_index*2)/2);
      position_psSiPM.setZ(assemblyBoxZ - assemblyBoxZ/15*(1+z_index*2)/2);
      position_psSiPM.setY(ej200Y+2*greaseY+2*qwSiPMy+psSiPMy);

      transform3D_psSiPM = G4Transform3D(rotationMatrix_psSiPM, position_psSiPM);
      psSiPMAssembly->AddPlacedVolume(psSiPM_logV, transform3D_psSiPM);

      position_psSiPM.setY(-1*(ej200Y+2*greaseY+2*qwSiPMy+psSiPMy));

      transform3D_psSiPM = G4Transform3D(rotationMatrix_psSiPM, position_psSiPM);
      psSiPMAssembly->AddPlacedVolume(psSiPM_logV, transform3D_psSiPM);
    }
  }

  //  place the assembly
  //
  position_psSiPM.setX(0.0); position_psSiPM.setY(0.0); position_psSiPM.setZ(0.0);
  transform3D_psSiPM = G4Transform3D(rotationMatrix_psSiPM, position_psSiPM);


  psSiPMAssembly->MakeImprint(assembly_logV, transform3D_psSiPM);

  return;
} // end of function. //





void nDetConstruction::buildDisk()
{ 
  //************* Wrapping materials (such as Teflon) and EJ200  *************

  //-----------------------------------------------------------
  //     The assembly volume is defined as a wrapping material
  //-----------------------------------------------------------



    greaseX = 1.5 * mm;
    greaseZ = 1.5 * mm;
    qwSiPMx = 1.5 * mm;
    qwSiPMz = 1.5 * mm;
    psSiPMx = 1.5 * mm;
    psSiPMz = 1.5 * mm;


    G4double teflonThickness=0.11*mm;

    G4double minDiskRadius=0*mm;
    G4double maxDiskRadius=50*mm+teflonThickness;

    G4double diskLength=1.5*mm+teflonThickness;

    G4double startAngle=0*deg;
    G4double endAngle=360*deg;

    G4int nSides=6;
    G4int nPlanes=2;

    const G4double zplanes[]={-diskLength,diskLength};
    const G4double maxRadius[]={maxDiskRadius,maxDiskRadius};
    const G4double minRadius[]={minDiskRadius,minDiskRadius};

    G4VSolid *assemblyDisk=NULL;

    if(fGeometry=="disk")
    assemblyDisk = (G4Tubs*)new G4Tubs("Disk",minDiskRadius,maxDiskRadius,diskLength,startAngle,endAngle);

    else
    assemblyDisk= (G4Polyhedra*) new G4Polyhedra("Hexagon",startAngle,endAngle,nSides,nPlanes,zplanes,minRadius,maxRadius);

    G4double BoxX=1.52*mm;
    G4double BoxY=1.52*mm;
    G4double BoxZ=1.65*mm;

    G4Box *Hole=new G4Box("Hole",BoxX,BoxY,BoxZ);

    G4SubtractionSolid *theSolid=new G4SubtractionSolid("Wrapping",assemblyDisk,Hole);


    assembly_logV = new G4LogicalVolume(theSolid, fTeflon, "Wrap");


    G4RotationMatrix *rotationMatrix_Disk=new G4RotationMatrix;
    rotationMatrix_Disk->rotateY(90*deg);

    assembly_physV = new G4PVPlacement(rotationMatrix_Disk,
                                       G4ThreeVector(assemblyPx, assemblyPy, assemblyPz),
                                       assembly_logV,
                                       "assembly_physV",
                                       expHall_logV,
                                       false, 0);

    //----------------------------------------------------------------
    //    scintillator bars
    //----------------------------------------------------------------


  G4LogicalSkinSurface* assemblySkin = new G4LogicalSkinSurface("teflonSUrface", assembly_logV, fTeflonOpticalSurface);


     minDiskRadius=0*mm;
     maxDiskRadius=50*mm;
    
     diskLength=1.5*mm;
    
     startAngle=0*deg;
     endAngle=360*deg;

    const G4double zplanes2[]={-diskLength,diskLength};
    const G4double maxRadius2[]={maxDiskRadius,maxDiskRadius};
    const G4double minRadius2[]={minDiskRadius,minDiskRadius};

    G4VSolid *scintillatorDisk=NULL;
    if(fGeometry=="disk")
        scintillatorDisk = (G4Tubs*)new G4Tubs("SciDisk",minDiskRadius,maxDiskRadius,diskLength,startAngle,endAngle);
    else
        scintillatorDisk= (G4Polyhedra*) new G4Polyhedra("SciHexagon",startAngle,endAngle,nSides,nPlanes,zplanes2,minRadius2,maxRadius2);



    BoxX=2.70*mm;
    BoxY=2.70*mm;
    BoxZ=1.62*mm;
    
    G4Box *Hole2=new G4Box("Hole",BoxX,BoxY,BoxZ);
    
    G4SubtractionSolid *theScint=new G4SubtractionSolid("Detector",scintillatorDisk,Hole2);
  
    
  ej200_logV = new G4LogicalVolume(theScint, fEJ200, "ej200_logV", 0, 0, 0);

  G4VisAttributes* ej200_VisAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0));//blue
  //ej200_VisAtt->SetForceSolid(true);
  ej200_logV->SetVisAttributes(ej200_VisAtt);

  //ej200_logV->SetSensitiveDetector(fScintSD);

  // create assembly of scintillator
  //
  
  G4RotationMatrix *rotationMatrix_Scint=new G4RotationMatrix;

    
  assembly_physV = new G4PVPlacement(rotationMatrix_Scint,
                                       G4ThreeVector(assemblyPx, assemblyPy, assemblyPz),
                                       ej200_logV,
                                       "Scint_physV",
                                       assembly_logV,
                                       false, 0);
  
  // grease Sanit-Gobain BC-630 silicone grease
  // optical grease


  // grease box ...
  G4Box* grease_solidV = new G4Box("grease", greaseX, greaseY, greaseZ);
  grease_logV = new G4LogicalVolume(grease_solidV, fGrease, "grease_logV", 0, 0, 0);
  G4VisAttributes* grease_VisAtt= new G4VisAttributes(G4Colour(1.0,0.0,0.0));//red
  //grease_VisAtt->SetForceSolid(true);
  grease_logV->SetVisAttributes(grease_VisAtt);

  // create assembly of grease
  //
  G4AssemblyVolume* greaseAssembly = new G4AssemblyVolume();
  // Rotation and translation of the grease logical volume
  G4RotationMatrix rotationMatrix_grease;
  G4ThreeVector position_grease(0., 0., 0.);
  G4Transform3D transform3D_grease;

  for(G4int i = 0; i<2; i++){
      //position_grease.setX(assemblyBoxX - assemblyBoxX/15*(1+x_index*2)/2);
      if(i==0){
          position_grease.setX(0);
          position_grease.setZ(0);
          position_grease.setY(BoxY-greaseY);

          transform3D_grease = G4Transform3D(rotationMatrix_grease, position_grease);
          greaseAssembly->AddPlacedVolume(grease_logV, transform3D_grease);

          position_grease.setY(-1*(BoxY-greaseY));

          transform3D_grease = G4Transform3D(rotationMatrix_grease, position_grease);
          greaseAssembly->AddPlacedVolume(grease_logV, transform3D_grease);
      }
      else{
          rotationMatrix_grease.rotateZ(90*deg);
          position_grease.setY(0);
          position_grease.setZ(0);
          position_grease.setX(BoxY-greaseY);
          
          transform3D_grease = G4Transform3D(rotationMatrix_grease, position_grease);
          greaseAssembly->AddPlacedVolume(grease_logV, transform3D_grease);
          
          position_grease.setX(-1*(BoxY-greaseY));
          
          transform3D_grease = G4Transform3D(rotationMatrix_grease, position_grease);
          greaseAssembly->AddPlacedVolume(grease_logV, transform3D_grease);
          
      }
  }

  //  place the assembly
  //
  position_grease.setX(0.0); position_grease.setY(0.0); position_grease.setZ(0.0);
  transform3D_grease = G4Transform3D(rotationMatrix_grease, position_grease);
  greaseAssembly->MakeImprint(assembly_logV, transform3D_grease);


  G4Box* qwSiPM_solidV = new G4Box("qwSiPM_solidV", qwSiPMx, qwSiPMy, qwSiPMz);

  qwSiPM_logV = new G4LogicalVolume(qwSiPM_solidV, fSiO2, "qwSiPM_logV", 0, 0, 0);
  G4VisAttributes* qwSiPM_VisAtt= new G4VisAttributes(G4Colour(0.0,1.0,0.0));//green
  //qwSiPM_VisAtt->SetForceSolid(true);
  qwSiPM_logV->SetVisAttributes(qwSiPM_VisAtt);

  // create assembly of SiPM window
  //
  G4AssemblyVolume* SiPMwindowAssembly = new G4AssemblyVolume();
  // Rotation and translation of the grease logical volume
  G4RotationMatrix rotationMatrix_SiPMwindow;
  G4ThreeVector position_SiPMwindow(0., 0., 0.);
  G4Transform3D transform3D_SiPMwindow;

  for(G4int i = 0; i<2; i++){
      if(i==0){
      position_SiPMwindow.setX(0);
      position_SiPMwindow.setZ(0);
      position_SiPMwindow.setY(BoxY-2*greaseY-qwSiPMy);

      transform3D_SiPMwindow = G4Transform3D(rotationMatrix_SiPMwindow, position_SiPMwindow);
      SiPMwindowAssembly->AddPlacedVolume(qwSiPM_logV, transform3D_SiPMwindow);

      position_SiPMwindow.setY(-1*(BoxY-2*greaseY-qwSiPMy));

      transform3D_SiPMwindow = G4Transform3D(rotationMatrix_SiPMwindow, position_SiPMwindow);
      SiPMwindowAssembly->AddPlacedVolume(qwSiPM_logV, transform3D_SiPMwindow);
      }
      else{
          rotationMatrix_SiPMwindow.rotateZ(90*deg);
          position_SiPMwindow.setY(0);
          position_SiPMwindow.setZ(0);
          position_SiPMwindow.setX(BoxY-2*greaseY-qwSiPMy);
          
          transform3D_SiPMwindow = G4Transform3D(rotationMatrix_SiPMwindow, position_SiPMwindow);
          SiPMwindowAssembly->AddPlacedVolume(qwSiPM_logV, transform3D_SiPMwindow);
          
          position_SiPMwindow.setX(-1*(BoxY-2*greaseY-qwSiPMy));
          
          transform3D_SiPMwindow = G4Transform3D(rotationMatrix_SiPMwindow, position_SiPMwindow);
          SiPMwindowAssembly->AddPlacedVolume(qwSiPM_logV, transform3D_SiPMwindow);
      
      }
  }
  

  //  place the assembly
  //
  position_SiPMwindow.setX(0.0); position_SiPMwindow.setY(0.0); position_SiPMwindow.setZ(0.0);
  transform3D_SiPMwindow = G4Transform3D(rotationMatrix_SiPMwindow, position_SiPMwindow);
  SiPMwindowAssembly->MakeImprint(assembly_logV, transform3D_SiPMwindow);

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // the portion of SiPM just following the front quartz window is used for sensing the photons
  // the material of this portion or photon sensing part is made of silicon.
  // 

  // define the optical surface for SiPM
  G4OpticalSurface* SiPMSurface = new G4OpticalSurface("SiPMSurface");
  SiPMSurface->SetType(dielectric_metal);
  SiPMSurface->SetFinish(polished);
  SiPMSurface->SetModel(glisur);
  SiPMSurface->SetMaterialPropertiesTable(fSilMPT);

  G4Box* psSiPM_solidV = new G4Box("psSiPM_solidV", psSiPMx, psSiPMy, psSiPMz);



    psSiPM_logV = new G4LogicalVolume(psSiPM_solidV,fSil, "psSiPM_logV", 0, 0, 0);
  G4VisAttributes* psSiPM_VisAtt= new G4VisAttributes(G4Colour(0.0,1.0,1.0));//
  psSiPM_VisAtt->SetForceSolid(true);
  psSiPM_logV->SetVisAttributes(psSiPM_VisAtt);

  //Add Sensitive Detector to SiPM
   // psSiPM_logV->SetSensitiveDetector(fSiPMSD);

  G4LogicalSkinSurface* psSiPMSurface = new G4LogicalSkinSurface("SiPMSurface", psSiPM_logV, SiPMSurface);

  // create assembly of SiPM sensitive part
  //
  G4AssemblyVolume* psSiPMAssembly = new G4AssemblyVolume();
  // Rotation and translation of the grease logical volume
  G4RotationMatrix rotationMatrix_psSiPM;
  G4ThreeVector position_psSiPM(0., 0., 0.);
  G4Transform3D transform3D_psSiPM;

  for(G4int i = 0; i<2; i++){
      if(i==0){
      position_psSiPM.setX(0);
      position_psSiPM.setZ(0);
      position_psSiPM.setY(BoxY-2*greaseY-2*qwSiPMy-psSiPMy);

      transform3D_psSiPM = G4Transform3D(rotationMatrix_psSiPM, position_psSiPM);
      psSiPMAssembly->AddPlacedVolume(psSiPM_logV, transform3D_psSiPM);

      position_psSiPM.setY(-1*(BoxY-2*greaseY-2*qwSiPMy-psSiPMy));

      transform3D_psSiPM = G4Transform3D(rotationMatrix_psSiPM, position_psSiPM);
      psSiPMAssembly->AddPlacedVolume(psSiPM_logV, transform3D_psSiPM);
      }
      else{
          rotationMatrix_psSiPM.rotateZ(90*deg);
          position_psSiPM.setY(0);
          position_psSiPM.setZ(0);
          position_psSiPM.setX(BoxY-2*greaseY-2*qwSiPMy-psSiPMy);
          
          transform3D_psSiPM = G4Transform3D(rotationMatrix_psSiPM, position_psSiPM);
          psSiPMAssembly->AddPlacedVolume(psSiPM_logV, transform3D_psSiPM);
          
          position_psSiPM.setX(-1*(BoxY-2*greaseY-2*qwSiPMy-psSiPMy));
          
          transform3D_psSiPM = G4Transform3D(rotationMatrix_psSiPM, position_psSiPM);
          psSiPMAssembly->AddPlacedVolume(psSiPM_logV, transform3D_psSiPM);
      
      }
  }

  //  place the assembly
  //
  position_psSiPM.setX(0.0); position_psSiPM.setY(0.0); position_psSiPM.setZ(0.0);
  transform3D_psSiPM = G4Transform3D(rotationMatrix_psSiPM, position_psSiPM);
    psSiPMAssembly->MakeImprint(assembly_logV, transform3D_psSiPM);

  return;
} // end of function. //


void nDetConstruction::buildEllipse() {

    G4double inches=25.4*mm;

    G4double teflonThickness=0.11*mm;

    //First we build the box
    G4double xdimension=1.18/2*inches+teflonThickness;
    G4double ydimension=0.24/2*inches+teflonThickness;
    G4double zdimension=1.94/2*inches+teflonThickness;

    G4Box *theBox=new G4Box("theBox",xdimension,ydimension,zdimension);

    //Now the trapezoid

    G4double dy1=ydimension;
    G4double dy2=ydimension;
    G4double dx1=xdimension;
    G4double dx2=ydimension;
    G4double dz=0.5*inches;


    //Ellipse shaped
    G4Trd *theTrapezoid1=new G4Trd("theTrapezoid1", dx1, dx2, dy1, dy2, dz);

    G4Trd *theTrapezoid2=new G4Trd("theTrapezoid2", dx1, dx2, dy1, dy2, dz);

    G4Box *wrappinBox=new G4Box("theBox1",ydimension,ydimension,(greaseY+qwSiPMy+psSiPMy));

    G4Box *wrappinBox2=new G4Box("theBox2",ydimension,ydimension,(greaseY+qwSiPMy+psSiPMy));

    G4ThreeVector translation1(0,0,(1.94/2+1./2)*inches);

    G4UnionSolid *theWrapping0=new G4UnionSolid("theWrapping0",theBox,theTrapezoid1,0,translation1);

    G4ThreeVector translation11(0,0,(1.94/2+1.)*inches+1*(greaseY+qwSiPMy+psSiPMy));

    G4UnionSolid *theWrapping1=new G4UnionSolid("theWrapping1",theWrapping0,wrappinBox,0,translation11);

    G4ThreeVector translation2(0,0,-(1.94/2+1./2)*inches);
    G4RotationMatrix *rot=new G4RotationMatrix();

    rot->rotateX(180*deg);

    G4UnionSolid *theWrapping2=new G4UnionSolid("theWrapping",theWrapping1,theTrapezoid2,rot,translation2);

    G4ThreeVector translation22(0,0,-1*((1.94/2+1.)*inches+1*(greaseY+qwSiPMy+psSiPMy)));



    //theBox
    xdimension=1.18/2*inches+teflonThickness;
    ydimension=0.24/2*inches+teflonThickness;
    zdimension=(1.94/2+1)*inches+teflonThickness+2*(greaseY+qwSiPMy+psSiPMy);

    G4VSolid *theWrapping=NULL;

    if(fGeometry == "ellipse")
        theWrapping=new G4UnionSolid("theWrapping",theWrapping2,wrappinBox2,rot,translation22);
    else
        theWrapping=new G4Box("theWrapping",xdimension,ydimension,zdimension);



    assembly_logV=new G4LogicalVolume(theWrapping,fTeflon,"Wrapping_log");

    G4VPhysicalVolume *Wrapping_physVol=new G4PVPlacement(0,G4ThreeVector(0,0,0),assembly_logV,"Wrap",expHall_logV,0,0,true);

    G4LogicalSkinSurface *thePlasticSkin=new G4LogicalSkinSurface("Wrapping",assembly_logV,fTeflonOpticalSurface);




    //Building the Scintillator

    //First we build the box
    xdimension=1.18/2*inches;
    ydimension=0.24/2*inches;
    zdimension=1.94/2*inches;

    G4Box *theBoxScint=new G4Box("theBoxScint",xdimension,ydimension,zdimension);

    //Now the trapezoid

    dy1=ydimension;
    dy2=ydimension;
    dx1=xdimension;
    dx2=ydimension;
    dz=0.5*inches;

    G4Trd *theTrapezoidScint1=new G4Trd("theTrapezoidScint1", dx1, dx2, dy1, dy2, dz);

    G4Trd *theTrapezoidScint2=new G4Trd("theTrapezoidScint2", dx1, dx2, dy1, dy2, dz);

    G4UnionSolid *thePlastic0=new G4UnionSolid("thePlastic0",theBoxScint,theTrapezoidScint1,0,translation1);

    //translation2(0,0,-(1.94/2+1./2)*inches);

    //G4RotationMatrix *rot2=new G4RotationMatrix();

    //rot->rotateX(180*deg);



    //theBox
    xdimension=1.18/2*inches;
    ydimension=0.24/2*inches;
    zdimension=(1.94/2+1)*inches;

    G4VSolid *thePlastic=NULL;
    if(fGeometry=="ellipse")
       thePlastic=new G4UnionSolid("thePlastic",thePlastic0,theTrapezoidScint2,rot,translation2);
    else
       thePlastic=new G4Box("thePlastic",xdimension,ydimension,zdimension);


    ej200_logV=new G4LogicalVolume(thePlastic,fEJ200,"plastic_log");
    //thePlastic_log->SetSensitiveDetector(fScintSD);
    G4VisAttributes* ej200_VisAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0));//blue
    ej200_logV->SetVisAttributes(ej200_VisAtt);
    G4VPhysicalVolume *physVol=new G4PVPlacement(0,G4ThreeVector(0,0,0),ej200_logV,"Scint",assembly_logV,0,0,true);

    buildSiPMs();
    //psSiPM_logV->SetSensitiveDetector(fSiPMSD);
    return;

}

//****************************************** Material Definitions  *********************************************//

void nDetConstruction::DefineMaterials() {

    //Elements
    G4double z;
    G4double a;
    fH=new G4Element("Hydrogen", "H", z=1., a=1.00794*g/mole);
    fC=new G4Element("Carbon", "C", z=6., a=12.01*g/mole);
    fO=new G4Element("Oxygen", "O", z=8., a=16.00*g/mole);
    fF=new G4Element("Fluorine", "F", z=9., a=18.9984*g/mole);
    fSi=new G4Element("Silicon", "Si", z=14., a=28.09*g/mole);

    //Materials & Properties
    G4NistManager* manNist = G4NistManager::Instance();

    fAir=manNist->FindOrBuildMaterial("G4_AIR");

    const G4int nEntries_Air = 2;
    G4double photonEnergy_Air[nEntries_Air] =   // from 800 nm to 300 nm
            { 1.55*eV,  4.13*eV };
    G4double refractiveIndex_Air[nEntries_Air] =
            { 1.00, 1.00 };
    // note that the aborption data are not correct. If they are employed, the purpose is to kill photons quickly
    // to speed up the program...
    //G4double absorptionLength_Air[nEntries] = {90.2*mm, 90.2*mm};
    fAirMPT = new G4MaterialPropertiesTable();
    fAirMPT->AddProperty("RINDEX", photonEnergy_Air, refractiveIndex_Air, nEntries_Air);
    fAir->SetMaterialPropertiesTable(fAirMPT);

    G4double density;
    int natoms;
    fTeflon= new G4Material("Teflon", density=2.2*g/cm3,2);
    fTeflon->AddElement(fC,natoms=2);
    fTeflon->AddElement(fF,natoms=4);

    fTeflonMPT = new G4MaterialPropertiesTable();

    const G4int nEntries_Teflon = 2;

    G4double photonEnergy_teflon[nEntries_Teflon] = {2.*eV, 3.47*eV};
    G4double refl_teflon[nEntries_Teflon] = {0.99, 0.99};
    G4double effi_teflon[nEntries_Teflon] = {0, 0};

    fTeflonMPT->AddProperty("REFLECTIVITY", photonEnergy_teflon, refl_teflon, nEntries_Teflon);
    fTeflonMPT->AddProperty("EFFICIENCY", photonEnergy_teflon, effi_teflon, nEntries_Teflon);



    // EJ-200  => [C9H10]n
    density = 1.023*g/cm3;
    fEJ200 = new G4Material("EJ200", density, 2);
    fEJ200->AddElement(fH, 10);
    //no longer // removing carbon to see how that effects backward scatters... KS 5/23/16
    fEJ200->AddElement(fC,9);

    const G4int nEntries_EJ200 = 5;

//   wavelength(nm) = 1242/energy(ev)
//   so the corresponding wavelength in nm show next
//   500, 475, 450, 425, 400

    G4double PhotonEnergy[nEntries_EJ200] =
            { 2.484*eV, 2.615*eV, 2.760*eV, 2.922*eV, 3.105*eV };

    G4double RefractiveIndex_EJ200[nEntries_EJ200] = { 1.58, 1.58, 1.58, 1.58, 1.58 };
    G4double Absorption_EJ200[nEntries_EJ200] =  { 400*cm, 400*cm, 400*cm, 400*cm, 400*cm };
    G4double ScintilFast_EJ200[nEntries_EJ200] = { 0.05, 0.25, 0.55, 1.00, 0.0};

    fEJ200MPT = new G4MaterialPropertiesTable();
    fEJ200MPT->AddProperty("RINDEX",       PhotonEnergy, RefractiveIndex_EJ200, nEntries_EJ200);
    fEJ200MPT->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption_EJ200,      nEntries_EJ200);
    fEJ200MPT->AddProperty("FASTCOMPONENT",PhotonEnergy, ScintilFast_EJ200,     nEntries_EJ200);

    fEJ200MPT->AddConstProperty("SCINTILLATIONYIELD", 1/keV); //10,000 Photons per MeV
    fEJ200MPT->AddConstProperty("RESOLUTIONSCALE",1.0); // Intrinsic resolution

    fEJ200MPT->AddConstProperty("RISETIMECONSTANT", 0.9*ns);
    fEJ200MPT->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);

    fEJ200MPT->AddConstProperty("YIELDRATIO",1);// the strength of the fast component as a function of total scintillation yield

    fEJ200->SetMaterialPropertiesTable(fEJ200MPT);

    // define the silicone optical grease, (C2H6OSi)n
    density = 1.06*g/cm3;
    fGrease = new G4Material("Grease", density, 4);
    fGrease->AddElement(fC, natoms=2);
    fGrease->AddElement(fH, natoms=6);
    fGrease->AddElement(fO, natoms=1);
    fGrease->AddElement(fSi, natoms=1);

    const G4int nEntries_Grease = 5;
    G4double Photon_Energy[nEntries_Grease] = { 2.757*eV, 3.102*eV, 3.312*eV, 3.545*eV, 4.136*eV };
    G4double RefractiveIndex_grease[nEntries_Grease] = {1.468, 1.473, 1.477, 1.482, 1.496};
    G4double Absorption_grease[nEntries_Grease] = { 195*mm,  195*mm, 195*mm, 195*mm, 195*mm };
    fGreaseMPT=new G4MaterialPropertiesTable();

    fGreaseMPT->AddProperty("RINDEX", Photon_Energy, RefractiveIndex_grease, nEntries_Grease);
    fGreaseMPT->AddProperty("ABSLENGTH", Photon_Energy, Absorption_grease, nEntries_Grease);

    fGrease->SetMaterialPropertiesTable(fGreaseMPT);

    fSiO2 = manNist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    const G4int nEntries_SiO2 = 5;

    //optical properties of SiO2 - fused silica or fused quartz
    G4double RefractiveIndex_SiO2[nEntries_SiO2] = { 1.54, 1.54, 1.54, 1.54, 1.54 };
    G4double Absorption_SiO2[nEntries_SiO2] =  {125*cm, 123.5*cm, 122*cm, 121*cm, 120*cm};

    fSiO2MPT = new G4MaterialPropertiesTable();
    fSiO2MPT->AddProperty("RINDEX", PhotonEnergy, RefractiveIndex_SiO2, nEntries_SiO2);
    fSiO2MPT->AddProperty("ABSLENGTH", PhotonEnergy, Absorption_SiO2,nEntries_SiO2);
    fSiO2->SetMaterialPropertiesTable(fSiO2MPT);

    fSil=manNist->FindOrBuildMaterial("G4_Si");

    // optical properties,
    const G4int nEntries_Sil = 5;
    G4double  RefractiveReal_Si[nEntries_Sil] = { 4.293, 4.453, 4.676, 5.008, 5.587 };
    G4double   RefractiveImg_Si[nEntries_Sil] = { 0.045, 0.060, 0.091, 0.150, 0.303 };
    G4double EfficiencyIndex_Si[nEntries_Sil] = { 0.37, 0.42, 0.39, 0.36, 0.32 };

    fSilMPT = new G4MaterialPropertiesTable();
    fSilMPT->AddProperty("REALRINDEX", PhotonEnergy, RefractiveReal_Si, nEntries_Sil);
    fSilMPT->AddProperty("IMAGINARYRINDEX", PhotonEnergy, RefractiveImg_Si, nEntries_Sil);
    fSilMPT->AddProperty("EFFICIENCY",   PhotonEnergy, EfficiencyIndex_Si, nEntries_Sil);

    fSil->SetMaterialPropertiesTable(fSilMPT);

    // optical surfaces
    fTeflonOpticalSurface = new G4OpticalSurface("TeflonSurface",
                                                 glisur,
                                                 ground,
                                                 dielectric_metal,
                                                 0.1); // polish level

    fTeflonOpticalSurface->SetMaterialPropertiesTable(fTeflonMPT);


    fSiliconPMOpticalSurface = new G4OpticalSurface("SiPMSurface");
    fSiliconPMOpticalSurface->SetType(dielectric_metal);
    fSiliconPMOpticalSurface->SetFinish(polished);
    fSiliconPMOpticalSurface->SetModel(glisur);
    fSiliconPMOpticalSurface->SetMaterialPropertiesTable(fSilMPT);


    return;
}

void nDetConstruction::buildSiPMs() {

    G4double inches=25.4*mm;
    
    if(fGeometry == "ellipse" || fGeometry == "rectangle") {
        greaseX = 0.12 * inches;
        greaseZ = 0.12 * inches;
        qwSiPMx = 0.12 * inches;
        qwSiPMz = 0.12 * inches;
        psSiPMx = 0.12 * inches;
        psSiPMz = 0.12 * inches;
    }

    fNdetectors=2;
    G4int factor=1;

    G4ThreeVector mother_position(0, 0, 0);


    //The Grease
    G4Box* grease_solidV = new G4Box("grease", greaseX, greaseY, greaseZ);
    grease_logV = new G4LogicalVolume(grease_solidV, fGrease, "grease_logV", 0, 0, 0);
    G4VisAttributes* grease_VisAtt= new G4VisAttributes(G4Colour(1.0,0.0,0.0));//red
    //grease_VisAtt->SetForceSolid(true);
    grease_logV->SetVisAttributes(grease_VisAtt);

    G4AssemblyVolume *theGrease=new G4AssemblyVolume();

    for(G4int m=0;m<fNdetectors;m++) {

        if(m==1)
            factor=-1;
        G4ThreeVector position(0, 0, factor*(3.94 / 2 * inches+greaseY));
        G4RotationMatrix *rot = new G4RotationMatrix();
        rot->rotateX(90 * deg);

        theGrease->AddPlacedVolume(grease_logV, position, rot);
    }
    theGrease->MakeImprint(assembly_logV,mother_position,NULL);


    //The SiPM window
    G4Box* qwSiPM_solidV = new G4Box("qwSiPM_solidV", qwSiPMx, qwSiPMy, qwSiPMz);

    qwSiPM_logV = new G4LogicalVolume(qwSiPM_solidV,fSiO2,"SiPMwindow_log");

    G4VisAttributes* window_VisAtt= new G4VisAttributes(G4Colour(0.0,1.0,1.0));//cyan
    //window_VisAtt->SetForceSolid(true);
    qwSiPM_logV->SetVisAttributes(window_VisAtt);

    factor=1;

    G4AssemblyVolume* theSiPMWindows=new G4AssemblyVolume();
    for(G4int m=0;m<fNdetectors;m++) {

        if(m==1)
            factor=-1;
        G4ThreeVector position(0, 0, factor*(3.94 / 2 * inches + qwSiPMy+2*greaseY));
        G4RotationMatrix *rot = new G4RotationMatrix();
        rot->rotateX(90 * deg);

        theSiPMWindows->AddPlacedVolume(qwSiPM_logV, position, rot);
    }
    theSiPMWindows->MakeImprint(assembly_logV,mother_position,NULL);

    //the Sensitive surface

    G4Box* psSiPM_solidV = new G4Box("psSiPM_solidV", psSiPMx, psSiPMy, psSiPMz);


    psSiPM_logV = new G4LogicalVolume(psSiPM_solidV,fSil, "psSiPM_logV", 0, 0, 0);

    G4LogicalSkinSurface *SiPMSurface=new G4LogicalSkinSurface("theSiPM",psSiPM_logV,fSiliconPMOpticalSurface);

    G4VisAttributes* psSiPM_VisAtt= new G4VisAttributes(G4Colour(0.1,1.0,0.0));//magenta
    psSiPM_VisAtt->SetForceSolid(true);
    psSiPM_logV->SetVisAttributes(psSiPM_VisAtt);
    factor=1;
    G4AssemblyVolume* psSiPMAssembly = new G4AssemblyVolume();
    for(G4int m=0;m<fNdetectors;m++) {

        if(m==1)
            factor=-1;
        G4ThreeVector position(0, 0, factor*(3.94 / 2 * inches + 2*qwSiPMy+2*greaseY+psSiPMy));
        G4RotationMatrix *rot = new G4RotationMatrix();
        rot->rotateX(90 * deg);

        psSiPMAssembly->AddPlacedVolume(psSiPM_logV, position, rot);
    }
    psSiPMAssembly->MakeImprint(assembly_logV,mother_position,NULL);

    return;
}

void nDetConstruction::ConstructSDandField(){


    G4cout<<"nDetConstruction::ConstructSDandField()-->"<<G4endl;


    if(ej200_logV) {
        fScintSD = new nDetSD("/theScintSD");

        G4cout << "fScintSD--> " << fScintSD << G4endl;

        SetSensitiveDetector(ej200_logV, fScintSD);
    }


    if(psSiPM_logV) {

        fSiPMSD = new SiPMSD("/theSiPMSD");

        G4cout << "fSiPMSD--> " << fSiPMSD << G4endl;

        SetSensitiveDetector(psSiPM_logV, fSiPMSD);
    }

}

void nDetConstruction::UpdateGeometry(){

    // clean-up previous geometry
    G4SolidStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    //define new one
    //G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
    //G4RunManager::GetRunManager()->GeometryHasBeenModified();
    G4RunManager::GetRunManager()->ReinitializeGeometry();

    return;

}