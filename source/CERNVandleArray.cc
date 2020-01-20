// $Id: LeribssVandleArray.cc 26.01.2016 A Fijalkowska $
//
/// \file LeribssVandleArray.cc
/// \brief Implementation of the LeribssVandleArray class, mostly Sergey's Ilyushkin code
//
//

#include "CERNVandleArray.hh"
#include "G4SystemOfUnits.hh"
#include "G4Transform3D.hh"

CERNVandleArray::CERNVandleArray()
{
	SetBasicSizes();	
	vandleBar = new VANDLEBar(moduleSize);	
} 

CERNVandleArray::~CERNVandleArray()
{
	
}

void CERNVandleArray::SetBasicSizes()
{
	moduleSize = 1;//medium bar
    radius = 1.00*m;
  	nrOfModules = 26;  	
	
}


void CERNVandleArray::Place(G4RotationMatrix *pRot, 
                               G4ThreeVector &tlate, 
                               const G4String &pName, 
                               G4LogicalVolume *pMotherLogical,  
                               G4int pCopyNo)
{
	
	G4cout << "The Center Radius of the cylinder is :" 
	       << radius / cm 
	       << " cm" 
	       << G4endl;

	const G4ThreeVector zero(0.0,0.0,0.0);
	G4ThreeVector centerPosition(0.0,0.0,0.0);


	static G4double angle = -0.822080895*rad;
	const G4double deltaAngle = 0.0608948811*rad;


	for( int i = 0; i < nrOfModules; ++i )
	{
		if(i == 3)
		{
			angle += deltaAngle;
			continue;
		}
		G4RotationMatrix *rot = new G4RotationMatrix();
		rot->rotateY(-angle);		
	    rot->rotateX(90*degree);
		centerPosition[0] = radius * sin(angle);
		centerPosition[2] = radius * cos(angle);
		
        
		G4String cylinderString( "Bar");
		std::stringstream cylinderModuleID;
		cylinderModuleID << i;
		cylinderString += cylinderModuleID.str();
        centerPosition += tlate; 
        
	
	
		vandleBar->Place(rot, 
		                 centerPosition,
		                 cylinderString, 
		                 pMotherLogical, 
		                 nrOfModules * pCopyNo + i);              
		angle += deltaAngle;
	}
}

void CERNVandleArray::ConstructSDandField()
{
	vandleBar->ConstructSDandField();
}
