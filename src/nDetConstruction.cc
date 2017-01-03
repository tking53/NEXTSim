//
// $Id: nDetConstruction.cc,v1.0 Sept., 2015 $
// Written by Dr. Xiaodong Zhang
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "nDetConstruction.hh"
#include "G4SDManager.hh"

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
  G4double margin = 5*mm;

  expHallX = assemblyBoxX + margin;
  expHallY = assemblyBoxY + margin;
  expHallZ = assemblyBoxZ + margin;
 
  return;
} // end of construction function.

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

nDetConstruction::~nDetConstruction()
{} // end of deconstruction function.

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* nDetConstruction::Construct()
{

  // build experiment hall
  buildExpHall();

  // build assembly volume
  //buildAssembly();

  // build assembly volume
  buildDisk();

  return expHall_physV;
}

void nDetConstruction::buildExpHall()
{
  // used to find materials in the lib...
  G4NistManager* manNist = G4NistManager::Instance();
  
  // ---------------- materials ---------------
  // air
  G4Material* Air=manNist->FindOrBuildMaterial("G4_AIR");
 // G4Material* Vacuum=manNist->FindOrBuildMaterial("G4_VACUUM");
 
  //
  // ------------ Generate & Add Material Properties Table ------------
  //
  const G4int nEntries_Air = 2;
  G4double photonEnergy_Air[nEntries_Air] =   // from 800 nm to 300 nm 
              { 1.55*eV,  4.13*eV };
  G4double refractiveIndex_Air[nEntries_Air] =
            { 1.00, 1.00 };
  // note that the aborption data are not correct. If they are employed, the purpose is to kill photons quickly
  // to speed up the program...
  //G4double absorptionLength_Air[nEntries] = {90.2*mm, 90.2*mm};
  G4MaterialPropertiesTable* Air_MPT = new G4MaterialPropertiesTable();
  Air_MPT->AddProperty("RINDEX", photonEnergy_Air, refractiveIndex_Air, nEntries_Air);
  Air->SetMaterialPropertiesTable(Air_MPT);
//  Vacuum->SetMaterialPropertiesTable(Air_MPT);

  // **************** expHalll **************
  //      ------------- Volumes --------------
  
  G4Box* expHall_solidV = new G4Box("expHall_solidV",expHallX,expHallY,expHallZ);

  expHall_logV  = new G4LogicalVolume(expHall_solidV, Air, "expHall_logV",0,0,0);
 // expHall_logV  = new G4LogicalVolume(expHall_solidV, Vacuum, "expHall_logV",0,0,0);
  expHall_logV->SetVisAttributes(G4VisAttributes::Invisible);

  expHall_physV = new G4PVPlacement(0,G4ThreeVector(0., 0., 0.),expHall_logV,
                                   "expHall_physV",0,false,0);
 
  return;
} // end of buildExpHall function


// build scintillator array that consists of 30X30 EJ200 scintillator bars
// Each bar is read out by two SiPMs at the two ends
// Teflon tap is used to wrap the bars and take care diffusing light back into the scintillator
void nDetConstruction::buildAssembly()
{ 
  //************* Wrapping materials (such as Teflon) and EJ200  *************

  //-----------------------------------------------------------
  //     The assembly volume is defined as a wrapping material
  //-----------------------------------------------------------

  //--------- Material definition ---------

  G4double a, z;
  G4double density;
  G4int ncomponents, natoms;

  //Elements
 
  G4Element* C = new G4Element("Carbon", "C", z=6., a=12.01*g/mole);
  G4Element* F = new G4Element("Fluorine", "F", z=9., a=18.9984*g/mole);

  //Teflon
  G4Material* Teflon = new G4Material("Teflon", density=2.2*g/cm3,2);
  Teflon->AddElement(C,natoms=2);
  Teflon->AddElement(F,natoms=4);

/*  //Teflon made of hydrogen
  G4Element* H = new G4Element("Hydrogen", "H", z=1., a=1.00794*g/mole);
  G4Material* Teflon = new G4Material("Teflon", density=0.1*g/cm3,1);
  Teflon->AddElement(H,natoms=1);
*/
  G4Box* assemblyBox = new G4Box("assembly", assemblyBoxX, assemblyBoxY, assemblyBoxZ);
  assembly_logV = new G4LogicalVolume(assemblyBox, Teflon, "assembly");
  assembly_physV = new G4PVPlacement(0,
				     G4ThreeVector(assemblyPx, assemblyPy, assemblyPz),
 				     assembly_logV,
				     "assembly_physV",
				     expHall_logV,
				     false, 0);

  // optical properties
  G4OpticalSurface* TeflonSurface = new G4OpticalSurface("TeflonSurface",
							 glisur,
							 ground,
							 dielectric_metal,
							 0.1); // polish level

  G4MaterialPropertiesTable* teflonMPT = new G4MaterialPropertiesTable();

  G4double photonEnergy[2] = {2.*eV, 3.47*eV};
  G4double refl_teflon[2] = {0.99, 0.99};
  G4double effi_teflon[2] = {0, 0};
  
  teflonMPT->AddProperty("REFLECTIVITY", photonEnergy, refl_teflon, 2);
  teflonMPT->AddProperty("EFFICIENCY", photonEnergy, effi_teflon, 2);
   
  TeflonSurface->SetMaterialPropertiesTable(teflonMPT);  

  G4LogicalSkinSurface* assemblySkin = new G4LogicalSkinSurface("teflonSUrface", assembly_logV, TeflonSurface);

  //----------------------------------------------------------------
  //    scintillator bars
  //----------------------------------------------------------------  
  
  // material ....
  G4double atomicNumber_H = 1;
  G4double massOfMole_H = 1.00794*g/mole;
  G4Element* elH = new G4Element("Hydrogen", "H", atomicNumber_H, massOfMole_H);

  G4double atomicNumber_C = 6;
  G4double massOfMole_C = 12.01*g/mole;
  G4Element* elC = new G4Element("Carbon", "C", atomicNumber_C, massOfMole_C);

  // EJ200
  // EJ-200  => [C9H10]n
  G4double density_EJ200 = 1.023*g/cm3;
//  G4double density_EJ200 = 0.0867*g/cm3;
  G4Material* EJ200 = new G4Material("EJ200", density_EJ200, 2);
  EJ200->AddElement(elH, 10);
 //no longer // removing carbon to see how that effects backward scatters... KS 5/23/16
  EJ200->AddElement(elC,9);

  //
  // ------------ Generate & Add Material Properties Table ------------
  //
  const G4int nEntries = 5;

//
//   wavelength(nm) = 1242/energy(ev)
//   so the corresponding wavelength in nm show next
//   500, 475, 450, 425, 400
//
  G4double PhotonEnergy[nEntries] =
            { 2.484*eV, 2.615*eV, 2.760*eV, 2.922*eV, 3.105*eV };
  
  G4double RefractiveIndex_EJ200[nEntries] = { 1.58, 1.58, 1.58, 1.58, 1.58 };
  G4double Absorption_EJ200[nEntries] =  { 400*cm, 400*cm, 400*cm, 400*cm, 400*cm };
  G4double ScintilFast_EJ200[nEntries] = { 0.05, 0.25, 0.55, 1.00, 0.0};

  G4MaterialPropertiesTable* EJ200_MPT = new G4MaterialPropertiesTable();
  EJ200_MPT->AddProperty("RINDEX",       PhotonEnergy, RefractiveIndex_EJ200, nEntries);
  EJ200_MPT->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption_EJ200,      nEntries);
  EJ200_MPT->AddProperty("FASTCOMPONENT",PhotonEnergy, ScintilFast_EJ200,     nEntries);

  EJ200_MPT->AddConstProperty("SCINTILLATIONYIELD", 1/keV); //10,000 Photons per MeV 
  EJ200_MPT->AddConstProperty("RESOLUTIONSCALE",1.0); // Intrinsic resolution

  EJ200_MPT->AddConstProperty("RISETIMECONSTANT", 0.9*ns);
  EJ200_MPT->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);

  EJ200_MPT->AddConstProperty("YIELDRATIO",1);// the strength of the fast component as a function of total scintillation yield

  EJ200->SetMaterialPropertiesTable(EJ200_MPT);

  G4Box* ej200_solidV = new G4Box("ej200_solidV", ej200X, ej200Y, ej200Z);

  ej200_logV = new G4LogicalVolume(ej200_solidV, EJ200, "ej200_logV", 0, 0, 0);

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

  G4double massOfMole_O = 16.00*g/mole;
  G4double atomicNumber_O = 8;
  G4Element* O = new G4Element("Oxygen"  , "O", atomicNumber_O, massOfMole_O);

  G4double massOfMole_Si= 28.09*g/mole;
  G4double atomicNumber_Si= 14;
  G4Element* Si = new G4Element("Silicon", "Si", atomicNumber_Si, massOfMole_Si);

  // define the silicone optical grease, (C2H6OSi)n
  G4double density_grease = 1.06*g/cm3;
  G4Material* grease = new G4Material("Grease", density_grease, 4);
  grease->AddElement(elC, 2);
  grease->AddElement(elH, 6);
  grease->AddElement(O, 1);
  grease->AddElement(Si, 1);

  //
  // ------------ Generate & Add Material Properties Table ------------
  //
  const G4int n = 5;
  G4double Photon_Energy[n] = { 2.757*eV, 3.102*eV, 3.312*eV, 3.545*eV, 4.136*eV };
  G4double RefractiveIndex_grease[n] = {1.468, 1.473, 1.477, 1.482, 1.496};
  G4double Absorption_grease[n] = { 195*mm,  195*mm, 195*mm, 195*mm, 195*mm };
  G4MaterialPropertiesTable* greasePropertiesTable = new G4MaterialPropertiesTable();
  greasePropertiesTable->AddProperty("RINDEX",
                                Photon_Energy,
                                RefractiveIndex_grease,
                                n);
  greasePropertiesTable->AddProperty("ABSLENGTH", Photon_Energy, Absorption_grease, n);
  grease->SetMaterialPropertiesTable(greasePropertiesTable);

  // grease box ...
  G4Box* grease_solidV = new G4Box("grease", greaseX, greaseY, greaseZ);
  grease_logV = new G4LogicalVolume(grease_solidV, grease, "grease_logV", 0, 0, 0);
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

  // used to find materials in the lib...
  G4NistManager* manNist = G4NistManager::Instance();

  //
  // Resin window of SiPM, there is no details, so using SiO2 instead
  G4Material* SiO2 = manNist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");  

  //optical properties of SiO2 - fused silica or fused quartz
  G4double RefractiveIndex_SiO2[nEntries] = { 1.54, 1.54, 1.54, 1.54, 1.54 };
  G4double Absorption_SiO2[nEntries] =  {125*cm, 123.5*cm, 122*cm, 121*cm, 120*cm};

  G4MaterialPropertiesTable* SiO2_Property = new G4MaterialPropertiesTable();
  SiO2_Property->AddProperty("RINDEX",       PhotonEnergy, RefractiveIndex_SiO2, nEntries);
  SiO2_Property->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption_SiO2,      nEntries);
  SiO2->SetMaterialPropertiesTable(SiO2_Property);

  G4Box* qwSiPM_solidV = new G4Box("qwSiPM_solidV", qwSiPMx, qwSiPMy, qwSiPMz);

  qwSiPM_logV = new G4LogicalVolume(qwSiPM_solidV, SiO2, "qwSiPM_logV", 0, 0, 0);
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

  // Silicon material dedfinition
  G4Material* Si_SiPM=manNist->FindOrBuildMaterial("G4_Si");    

  // optical properties, 
  G4double  RefractiveReal_Si[nEntries] = { 4.293, 4.453, 4.676, 5.008, 5.587 };
  G4double   RefractiveImg_Si[nEntries] = { 0.045, 0.060, 0.091, 0.150, 0.303 };
  G4double EfficiencyIndex_Si[nEntries] = { 0.37, 0.42, 0.39, 0.36, 0.32 };

  G4MaterialPropertiesTable* Si_MPT = new G4MaterialPropertiesTable();
  Si_MPT->AddProperty("REALRINDEX",       PhotonEnergy, RefractiveReal_Si, nEntries);
  Si_MPT->AddProperty("IMAGINARYRINDEX",       PhotonEnergy, RefractiveImg_Si, nEntries);
  Si_MPT->AddProperty("EFFICIENCY",   PhotonEnergy, EfficiencyIndex_Si, nEntries);

  Si_SiPM->SetMaterialPropertiesTable(Si_MPT);

  // define the optical surface for SiPM
  G4OpticalSurface* SiPMSurface = new G4OpticalSurface("SiPMSurface");
  SiPMSurface->SetType(dielectric_metal);
  SiPMSurface->SetFinish(polished);
  SiPMSurface->SetModel(glisur);
  SiPMSurface->SetMaterialPropertiesTable(Si_MPT);

  G4Box* psSiPM_solidV = new G4Box("psSiPM_solidV", psSiPMx, psSiPMy, psSiPMz);

  psSiPM_logV = new G4LogicalVolume(psSiPM_solidV, Si_SiPM, "psSiPM_logV", 0, 0, 0);
  G4VisAttributes* psSiPM_VisAtt= new G4VisAttributes(G4Colour(0.0,1.0,1.0));//
  psSiPM_logV->SetVisAttributes(psSiPM_VisAtt);

  G4LogicalSkinSurface* psSiPMSurface = new G4LogicalSkinSurface("SiPMSurface", psSiPM_logV, SiPMSurface);

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

  //--------- Material definition ---------

  G4double a, z;
  G4double density;
  G4int ncomponents, natoms;

  //Elements
 
  G4Element* C = new G4Element("Carbon", "C", z=6., a=12.01*g/mole);
  G4Element* F = new G4Element("Fluorine", "F", z=9., a=18.9984*g/mole);

  //Teflon
  G4Material* Teflon = new G4Material("Teflon", density=2.2*g/cm3,2);
  Teflon->AddElement(C,natoms=2);
  Teflon->AddElement(F,natoms=4);

/*  //Teflon made of hydrogen
  G4Element* H = new G4Element("Hydrogen", "H", z=1., a=1.00794*g/mole);
  G4Material* Teflon = new G4Material("Teflon", density=0.1*g/cm3,1);
  Teflon->AddElement(H,natoms=1);
*/


  //----------------------------------------------------------------
  //    scintillator bars
  //----------------------------------------------------------------  
  
  // material ....
  G4double atomicNumber_H = 1;
  G4double massOfMole_H = 1.00794*g/mole;
  G4Element* elH = new G4Element("Hydrogen", "H", atomicNumber_H, massOfMole_H);

  G4double atomicNumber_C = 6;
  G4double massOfMole_C = 12.01*g/mole;
  G4Element* elC = new G4Element("Carbon", "C", atomicNumber_C, massOfMole_C);

  // EJ200
  // EJ-200  => [C9H10]n
  G4double density_EJ200 = 1.023*g/cm3;
//  G4double density_EJ200 = 0.0867*g/cm3;
  G4Material* EJ200 = new G4Material("EJ200", density_EJ200, 2);
  EJ200->AddElement(elH, 10);
 //no longer // removing carbon to see how that effects backward scatters... KS 5/23/16
  EJ200->AddElement(elC,9);


  G4double minDiskRadius=0*mm;
  G4double maxDiskRadius=50*mm;

  G4double diskLength=1.5*mm;

  G4double startAngle=0*deg;
  G4double endAngle=360*deg;


  G4Tubs* assemblyDisk = new G4Tubs("Disk",minDiskRadius,maxDiskRadius,diskLength,startAngle,endAngle);

  G4double BoxX=1.65*mm;
  G4double BoxY=1.65*mm;
  G4double BoxZ=1.65*mm;

  G4Box *Hole=new G4Box("Hole",BoxX,BoxY,BoxZ);

  G4SubtractionSolid *theSolid=new G4SubtractionSolid("cona",assemblyDisk,Hole);


  assembly_logV = new G4LogicalVolume(theSolid, EJ200, "Disk");


  G4RotationMatrix *rotationMatrix_Disk=new G4RotationMatrix;
  rotationMatrix_Disk->rotateY(90*deg);

  assembly_physV = new G4PVPlacement(rotationMatrix_Disk,
				     G4ThreeVector(assemblyPx, assemblyPy, assemblyPz),
 				     assembly_logV,
				     "assembly_physV",
				     expHall_logV,
				     false, 0);



  // optical properties
  G4OpticalSurface* TeflonSurface = new G4OpticalSurface("TeflonSurface",
							 glisur,
							 ground,
							 dielectric_metal,
							 0.1); // polish level

  G4MaterialPropertiesTable* teflonMPT = new G4MaterialPropertiesTable();

  G4double photonEnergy[2] = {2.*eV, 3.47*eV};
  G4double refl_teflon[2] = {0.99, 0.99};
  G4double effi_teflon[2] = {0, 0};
  
  teflonMPT->AddProperty("REFLECTIVITY", photonEnergy, refl_teflon, 2);
  teflonMPT->AddProperty("EFFICIENCY", photonEnergy, effi_teflon, 2);
   
  TeflonSurface->SetMaterialPropertiesTable(teflonMPT);  

  G4LogicalSkinSurface* assemblySkin = new G4LogicalSkinSurface("teflonSUrface", assembly_logV, TeflonSurface);

  //
  // ------------ Generate & Add Material Properties Table ------------
  //
  const G4int nEntries = 5;

//
//   wavelength(nm) = 1242/energy(ev)
//   so the corresponding wavelength in nm show next
//   500, 475, 450, 425, 400
//
  G4double PhotonEnergy[nEntries] =
            { 2.484*eV, 2.615*eV, 2.760*eV, 2.922*eV, 3.105*eV };
  
  G4double RefractiveIndex_EJ200[nEntries] = { 1.58, 1.58, 1.58, 1.58, 1.58 };
  G4double Absorption_EJ200[nEntries] =  { 400*cm, 400*cm, 400*cm, 400*cm, 400*cm };
  G4double ScintilFast_EJ200[nEntries] = { 0.05, 0.25, 0.55, 1.00, 0.0};

  G4MaterialPropertiesTable* EJ200_MPT = new G4MaterialPropertiesTable();
  EJ200_MPT->AddProperty("RINDEX",       PhotonEnergy, RefractiveIndex_EJ200, nEntries);
  EJ200_MPT->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption_EJ200,      nEntries);
  EJ200_MPT->AddProperty("FASTCOMPONENT",PhotonEnergy, ScintilFast_EJ200,     nEntries);

  EJ200_MPT->AddConstProperty("SCINTILLATIONYIELD", 1/keV); //10,000 Photons per MeV 
  EJ200_MPT->AddConstProperty("RESOLUTIONSCALE",1.0); // Intrinsic resolution

  EJ200_MPT->AddConstProperty("RISETIMECONSTANT", 0.9*ns);
  EJ200_MPT->AddConstProperty("FASTTIMECONSTANT", 2.1*ns);

  EJ200_MPT->AddConstProperty("YIELDRATIO",1);// the strength of the fast component as a function of total scintillation yield

  EJ200->SetMaterialPropertiesTable(EJ200_MPT);

  G4Box* ej200_solidV = new G4Box("ej200_solidV", ej200X, ej200Y, ej200Z);

  ej200_logV = new G4LogicalVolume(ej200_solidV, EJ200, "ej200_logV", 0, 0, 0);

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
  //ej200Assembly->MakeImprint(assembly_logV, transform3D_ej200);
  
  // grease Sanit-Gobain BC-630 silicone grease
  // optical grease

  G4double massOfMole_O = 16.00*g/mole;
  G4double atomicNumber_O = 8;
  G4Element* O = new G4Element("Oxygen"  , "O", atomicNumber_O, massOfMole_O);

  G4double massOfMole_Si= 28.09*g/mole;
  G4double atomicNumber_Si= 14;
  G4Element* Si = new G4Element("Silicon", "Si", atomicNumber_Si, massOfMole_Si);

  // define the silicone optical grease, (C2H6OSi)n
  G4double density_grease = 1.06*g/cm3;
  G4Material* grease = new G4Material("Grease", density_grease, 4);
  grease->AddElement(elC, 2);
  grease->AddElement(elH, 6);
  grease->AddElement(O, 1);
  grease->AddElement(Si, 1);

  //
  // ------------ Generate & Add Material Properties Table ------------
  //
  const G4int n = 5;
  G4double Photon_Energy[n] = { 2.757*eV, 3.102*eV, 3.312*eV, 3.545*eV, 4.136*eV };
  G4double RefractiveIndex_grease[n] = {1.468, 1.473, 1.477, 1.482, 1.496};
  G4double Absorption_grease[n] = { 195*mm,  195*mm, 195*mm, 195*mm, 195*mm };
  G4MaterialPropertiesTable* greasePropertiesTable = new G4MaterialPropertiesTable();
  greasePropertiesTable->AddProperty("RINDEX",
                                Photon_Energy,
                                RefractiveIndex_grease,
                                n);
  greasePropertiesTable->AddProperty("ABSLENGTH", Photon_Energy, Absorption_grease, n);
  grease->SetMaterialPropertiesTable(greasePropertiesTable);

  // grease box ...
  G4Box* grease_solidV = new G4Box("grease", greaseX, greaseY, greaseZ);
  grease_logV = new G4LogicalVolume(grease_solidV, grease, "grease_logV", 0, 0, 0);
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
  //greaseAssembly->MakeImprint(assembly_logV, transform3D_grease);

  // used to find materials in the lib...
  G4NistManager* manNist = G4NistManager::Instance();

  //
  // Resin window of SiPM, there is no details, so using SiO2 instead
  G4Material* SiO2 = manNist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");  

  //optical properties of SiO2 - fused silica or fused quartz
  G4double RefractiveIndex_SiO2[nEntries] = { 1.54, 1.54, 1.54, 1.54, 1.54 };
  G4double Absorption_SiO2[nEntries] =  {125*cm, 123.5*cm, 122*cm, 121*cm, 120*cm};

  G4MaterialPropertiesTable* SiO2_Property = new G4MaterialPropertiesTable();
  SiO2_Property->AddProperty("RINDEX",       PhotonEnergy, RefractiveIndex_SiO2, nEntries);
  SiO2_Property->AddProperty("ABSLENGTH",    PhotonEnergy, Absorption_SiO2,      nEntries);
  SiO2->SetMaterialPropertiesTable(SiO2_Property);

  G4Box* qwSiPM_solidV = new G4Box("qwSiPM_solidV", qwSiPMx, qwSiPMy, qwSiPMz);

  qwSiPM_logV = new G4LogicalVolume(qwSiPM_solidV, SiO2, "qwSiPM_logV", 0, 0, 0);
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
  //SiPMwindowAssembly->MakeImprint(assembly_logV, transform3D_SiPMwindow);

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // the portion of SiPM just following the front quartz window is used for sensing the photons
  // the material of this portion or photon sensing part is made of silicon.
  // 

  // Silicon material dedfinition
  G4Material* Si_SiPM=manNist->FindOrBuildMaterial("G4_Si");    

  // optical properties, 
  G4double  RefractiveReal_Si[nEntries] = { 4.293, 4.453, 4.676, 5.008, 5.587 };
  G4double   RefractiveImg_Si[nEntries] = { 0.045, 0.060, 0.091, 0.150, 0.303 };
  G4double EfficiencyIndex_Si[nEntries] = { 0.37, 0.42, 0.39, 0.36, 0.32 };

  G4MaterialPropertiesTable* Si_MPT = new G4MaterialPropertiesTable();
  Si_MPT->AddProperty("REALRINDEX",       PhotonEnergy, RefractiveReal_Si, nEntries);
  Si_MPT->AddProperty("IMAGINARYRINDEX",       PhotonEnergy, RefractiveImg_Si, nEntries);
  Si_MPT->AddProperty("EFFICIENCY",   PhotonEnergy, EfficiencyIndex_Si, nEntries);

  Si_SiPM->SetMaterialPropertiesTable(Si_MPT);

  // define the optical surface for SiPM
  G4OpticalSurface* SiPMSurface = new G4OpticalSurface("SiPMSurface");
  SiPMSurface->SetType(dielectric_metal);
  SiPMSurface->SetFinish(polished);
  SiPMSurface->SetModel(glisur);
  SiPMSurface->SetMaterialPropertiesTable(Si_MPT);

  G4Box* psSiPM_solidV = new G4Box("psSiPM_solidV", psSiPMx, psSiPMy, psSiPMz);

  psSiPM_logV = new G4LogicalVolume(psSiPM_solidV, Si_SiPM, "psSiPM_logV", 0, 0, 0);
  G4VisAttributes* psSiPM_VisAtt= new G4VisAttributes(G4Colour(0.0,1.0,1.0));//
  psSiPM_logV->SetVisAttributes(psSiPM_VisAtt);

  G4LogicalSkinSurface* psSiPMSurface = new G4LogicalSkinSurface("SiPMSurface", psSiPM_logV, SiPMSurface);

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
  //psSiPMAssembly->MakeImprint(assembly_logV, transform3D_psSiPM);

  return;
} // end of function. //
