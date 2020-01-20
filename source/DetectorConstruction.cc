//
// $Id: DetectorConstruction.cc 12.16.2016 A. Fijalkowska $
//
/// \file DetectorConstruction.cc
/// \brief DetectorConstruction class
//
//
#include "DetectorConstruction.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4UImanager.hh"
#include "G4SystemOfUnits.hh"

//ARGONE
//#include "ArgoneFrame.hh"
#include "ArgoneAuxDet.hh"
#include "ArgoneGeArray.hh"
#include "ArgoneHPGeHolder.hh"
#include "ArgoneBeamLine.hh"
#include "ArgoneTapeBox.hh"
#include "ArgoneBetaDet.hh"
#include "ArgoneLN2Tank.hh"
#include "Floor.hh"


//CERN
#include "Tape.hh"
//#include "Cubic_Chamber.hh"
#include "IS530_Chamber.hh"
#include "IS530_Plastic.hh"
#include "Polyhedron.hh"
#include "CloverSingleDetector.hh"
#include "CloverQuadDetector.hh"
#include "CloverSingleBuchDetector.hh"
#include "CloverQuadBuchDetector.hh"
#include "global.hh"

#include "CERNFrame.hh"
#include "CERNFloor.hh"
#include "CERNTapeBox.hh"
#include "CERNSupport.hh"

DetectorConstruction::DetectorConstruction()
{
  experimentalHallLogic = NULL;
  experimentalHallPhys = NULL;
}



DetectorConstruction::~DetectorConstruction() {}


G4VPhysicalVolume* DetectorConstruction::Construct(){

  if (experimentalHallPhys) {
     G4GeometryManager::GetInstance()->OpenGeometry();
     G4PhysicalVolumeStore::GetInstance()->Clean();
     G4LogicalVolumeStore::GetInstance()->Clean();
     G4SolidStore::GetInstance()->Clean();
     G4LogicalSkinSurface::CleanSurfaceTable();
     G4LogicalBorderSurface::CleanSurfaceTable();
  }

  materialsManager = MaterialsManager::GetInstance();
  return ConstructDetector();
}


G4VPhysicalVolume* DetectorConstruction::ConstructDetector()
{

  G4double expHall_x = 5.*m;
  G4double expHall_y = 5.*m;
  G4double expHall_z = 5.*m;
  G4Material* vaccum = materialsManager->GetAir();
  //Create experimental hall
  G4Box* experimentalHallSolid
    = new G4Box("experimentalHallSolid",expHall_x,expHall_y,expHall_z);
  experimentalHallLogic = new G4LogicalVolume(experimentalHallSolid,
                                             vaccum,
                                             "experimentalHallLogic",
                                             0,0,0);
  experimentalHallPhys = new G4PVPlacement(0,
                                           G4ThreeVector(),
                                           experimentalHallLogic,
                                           "expHall",
                                           0,
                                           false,
                                           0);

  experimentalHallLogic->SetVisAttributes(G4VisAttributes::Invisible);

  int smallSize = 0;
  int mediumSize = 1;
  int largeSize = 2;
  //call VANDLEBar fabric method
  //vandleBar = new VANDLEBar(mediumSize);
  cernVandleArray = new CERNVandleArray();
  G4ThreeVector vandlePosition = G4ThreeVector(0.0,0.0, 5.7*cm);

 
  G4RotationMatrix* rot = new G4RotationMatrix();	
  	
  cernVandleArray->Place(rot, 
                         vandlePosition,
                         "cernVandleArray", 
                          experimentalHallLogic, 
                          0);
                          
   CERNFloor* cernFloor = new CERNFloor();
   G4RotationMatrix* floorRot = new G4RotationMatrix();
   G4double floorXPos = -126.5*cm;
   G4ThreeVector floorPosition = G4ThreeVector(floorXPos,0., 0.);
   cernFloor->Place(floorRot, floorPosition, "cernFloor", experimentalHallLogic);
   
   CERNFrame* cernFrame = new CERNFrame();
   G4RotationMatrix* rotFrame = new G4RotationMatrix();
   rotFrame->rotateX(0*degree);
   G4double frameXPos = -22*cm;
   G4double frameZPos = 25*cm;
   G4ThreeVector framePosition = G4ThreeVector(frameXPos, 0, frameZPos);
   cernFrame->Place(rotFrame, framePosition, "cernFrame", experimentalHallLogic);

   CERNTapeBox* tapeBox = new CERNTapeBox();
   G4RotationMatrix* rotTapeBox = new G4RotationMatrix();
   G4double tapeBoxXPos = -86.55*cm;
   G4ThreeVector tapeBoxPosition = G4ThreeVector(tapeBoxXPos, 0, 0);
   tapeBox->Place(rotTapeBox, tapeBoxPosition, "cernTapeBox", experimentalHallLogic);                 
   
    
   CERNSupport* cernSupport = new CERNSupport();
   G4RotationMatrix* rotSupport = new G4RotationMatrix(); 
   G4ThreeVector supportPos(0.0,0.0, 5.7*cm);  
   cernSupport->Place(rotSupport, supportPos, "cernSupport", experimentalHallLogic);        
 //THIS                 


   ConstructCERNElements();
                                     
   return experimentalHallPhys;
}




void DetectorConstruction::ConstructCERNElements()
{
	
  vector<CloverQuadDetector*> 		clquad_array;
  vector<CloverQuadBuchDetector*> 	clquadbuch_array;
  vector<Tape*>						tape_array;
  vector<Polyhedron*>				poly_array;
  //vector<Cubic_Chamber*>			cubic_chamber_array;
  vector<IS530_Chamber*>			IS530_chamber_array;
  vector<IS530_Plastic*>			IS530_plastic_array;


// ___________________________ DETECTORS and PARTS _________________________________
// defined in the "config.dat" file


	for(int l=0;l<gLines;l++){
		
		
		if(1==gType[l]){   									// Clover KU
			clquad_array.push_back(		new CloverQuadDetector(	experimentalHallPhys,
																(G4double) gDistance[l]*mm,
																(G4double) (gTheta[l]*deg),
																(G4double) (gPhi[l]*deg),
																(G4double) (gSpin[l]*deg),
																clquad_array.size()));
			
		}
		if(2==gType[l]){  								// Clover Buch
			clquadbuch_array.push_back(	new CloverQuadBuchDetector(	experimentalHallPhys,
																	(G4double) gDistance[l]*mm,
																	(G4double) (gTheta[l]*deg),
																	(G4double) (gPhi[l]*deg),
																	(G4double) (gSpin[l]*deg),
																	clquadbuch_array.size()));
		}

		if(8==gType[l]){  								// Tape
			tape_array.push_back(new Tape(experimentalHallPhys));
			if(1<tape_array.size()){
				cout<<"\nYou can only contruct the tape one time!!!\n";
				exit(0);
			}
		}
		if(10==gType[l]){  								// Polyhedron 
			poly_array.push_back(new Polyhedron(experimentalHallPhys,
												(G4double) gDistance[l]*mm,
												(G4double) (gTheta[l]*deg),
												(G4double) (gPhi[l]*deg),
												(G4double) (gSpin[l]*deg)));
			if(1<poly_array.size()){
				cout<<"\nYou can only contruct the Polyhedron frame one time!!!\n";
				exit(0);
			}
		}		
		/*if(11==gType[l]){  													// Cubic Chamber
			cubic_chamber_array.push_back(new Cubic_Chamber(mother,
															(G4double) gDistance[l]*mm,
															(G4double) (gTheta[l]*deg),
															(G4double) (gPhi[l]*deg),
															(G4double) (gSpin[l]*deg)));
			if(1<cubic_chamber_array.size()){
				cout<<"\nYou can only contruct the Cubic chamber one time!!!\n";
				exit(0);
			}
			
		}	*/	
		if(12==gType[l]){  				// IS530 Chamber 
			IS530_chamber_array.push_back(new IS530_Chamber(experimentalHallPhys,
															(G4double) gDistance[l]*mm,
															(G4double) (gTheta[l]*deg),
															(G4double) (gPhi[l]*deg),
															(G4double) (gSpin[l]*deg)));
			if(1<IS530_chamber_array.size()){
				cout<<"\nYou can only contruct the IS530 chamber frame one time!!!\n"; 
				exit(0);
			}
			
		}
		if(13==gType[l]){  							// IS530 Plastic - non-sensitive detector 
			IS530_plastic_array.push_back(new IS530_Plastic(experimentalHallPhys,
															(G4double) gDistance[l]*mm,
															(G4double) (gTheta[l]*deg),
															(G4double) (gPhi[l]*deg),
															(G4double) (gSpin[l]*deg)));
			if(1<IS530_plastic_array.size()){
				cout<<"\nYou can only contruct the IS530 Plastic one time!!!\n"; 
				exit(0);
			}
		}
			
	}

  //Construction

  // 1. Clover KU Leuven
  for (int clq=0; clq<clquad_array.size(); clq++){
    clquad_array.at(clq)->Construct();
  }
  // 2. Clover Bucharest
  for (int clq=0; clq<clquadbuch_array.size(); clq++){
    clquadbuch_array.at(clq)->Construct();
  }

  // 8. Tape
  for (int t=0; t<tape_array.size(); t++){
    tape_array.at(t)->Construct();
  }

  // 10. OSIRIS chamber
  for (int pl=0; pl<poly_array.size(); pl++){
    poly_array.at(pl)->Construct();
  }
  /*
  // 11. Cubic chamber
  for (int cc=0; cc<cubic_chamber_array.size(); cc++){
    cubic_chamber_array.at(cc)->Construct();
  }*/
  // 12. IS530 chamber
  for (int is=0; is<IS530_chamber_array.size(); is++){
    IS530_chamber_array.at(is)->Construct();
  }
  // 13. IS530 plastic - non sensitive detector
  for (int is=0; is<IS530_plastic_array.size(); is++){
    IS530_plastic_array.at(is)->Construct();
  }


 
  cout << "CERN setup - DONE" <<endl;
	
}

void DetectorConstruction::ConstructSDandField() 
{
	cernVandleArray->ConstructSDandField();
    //vandleBar->ConstructSDandField();
}





