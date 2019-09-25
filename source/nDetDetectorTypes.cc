/** @file nDetDetectorTypes.cc
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu
  * @date September 25, 2019
  * @brief Contains all pre-defined detector construction types
  */

#include "G4SystemOfUnits.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"

#include "nDetDetectorTypes.hh"

///////////////////////////////////////////////////////////////////////////////
// class nextModuleType
///////////////////////////////////////////////////////////////////////////////

void nextModuleType::prepareToBuild(){
	// Set the maximum detector body size
	maxBodySize = G4ThreeVector(fDetectorWidth, fDetectorHeight, fDetectorLength);
}

void nextModuleType::buildDetector(){
	const G4double cellWidth = GetSegmentWidth();
	const G4double cellHeight = GetSegmentHeight();
	
	// Get the number of rows and columns for this segmented detector
	int Ncol = fNumColumns;
	int Nrow = fNumRows;

	// Update the detector's copy numbers
	firstSegmentCopyNum = scintCopyNum; 
	lastSegmentCopyNum = firstSegmentCopyNum; //startCopyNum+col*row;

    // Construct the scintillator cell
    G4Box *cellScint = new G4Box("scintillator", cellWidth/2, cellHeight/2, fDetectorLength/2);
    G4LogicalVolume *cellScint_logV = new G4LogicalVolume(cellScint, scintMaterial, "scint_log");
    cellScint_logV->SetVisAttributes(scintVisAtt);

	G4Box *mylarVertLayer = NULL;
	G4Box *mylarHorizLayer = NULL;
	
	G4LogicalVolume *mylarVertLayer_logV = NULL;
	G4LogicalVolume *mylarHorizLayer_logV = NULL;

	// Build the wrapping.
	G4PVPlacement *wrapping_physV = NULL;
	if(WrappingEnabled()){
		// Construct the outer wrapping.
		G4Box *wrappingBox = new G4Box("wrappingBox", fDetectorWidth/2+fWrappingThickness, fDetectorHeight/2+fWrappingThickness, fDetectorLength/2);
		G4Box *scintBox = new G4Box("scintBox", fDetectorWidth/2, fDetectorHeight/2, fDetectorLength/2);
		
		G4SubtractionSolid *wrappingBody = new G4SubtractionSolid("wrapping", wrappingBox, scintBox);
		G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrappingBody, wrappingMaterial, "wrapping_logV");
		wrapping_logV->SetVisAttributes(wrappingVisAtt);
		
		// Place the outer wrapping into the assembly.
		wrapping_physV = addToDetectorBody(wrapping_logV, "Wrapping");
		
		// Construct vertical and horizontal reflector layers for later use.
		mylarVertLayer = new G4Box("mylarVertLayer", fWrappingThickness/2, fDetectorHeight/2, fDetectorLength/2);
		mylarHorizLayer = new G4Box("mylarHorizLayer", cellWidth/2, fWrappingThickness/2, fDetectorLength/2);

		mylarVertLayer_logV = new G4LogicalVolume(mylarVertLayer, wrappingMaterial, "mylarVertLayer_logV");
		mylarHorizLayer_logV = new G4LogicalVolume(mylarHorizLayer, wrappingMaterial, "mylarHorizLayer_logV");
		
		mylarVertLayer_logV->SetVisAttributes(wrappingVisAtt);
		mylarHorizLayer_logV->SetVisAttributes(wrappingVisAtt);
	}

	// Place the scintillator segments into the assembly.
	std::vector<G4PVPlacement*> mylarVertLayer_physV(Ncol, NULL);
	std::vector<std::vector<G4PVPlacement*> > mylarHorizLayer_physV(Ncol, std::vector<G4PVPlacement*>(Nrow, NULL));
	std::vector<std::vector<G4PVPlacement*> > cellScint_physV(Ncol, std::vector<G4PVPlacement*>(Nrow, NULL));	
	for(int col = 0; col < Ncol; col++){
		for(int row = 0; row < Nrow; row++){
			G4ThreeVector cellCenter(-fDetectorWidth/2 + col*fWrappingThickness + (col+0.5)*cellWidth, -fDetectorHeight/2 + row*fWrappingThickness + (row+0.5)*cellHeight, 0);

			// Copy numbers (segment IDs), indexed from 1
			std::stringstream stream; stream << "Scint-" << col << "," << row;
			cellScint_physV[col][row] = addSegmentToBody(cellScint_logV, stream.str(), cellCenter);
			scintBody_physV.push_back(cellScint_physV[col][row]);
		
			// Place vertical and horizontal reflectors.
			if(WrappingEnabled()){ 
				if(row == 0 && col != Ncol-1){ // New vertical reflector layer.
					std::stringstream stream2; stream2 << "Wrapping-" << col;
					mylarVertLayer_physV[col] = addToDetectorBody(mylarVertLayer_logV, stream2.str().c_str(), G4ThreeVector(cellCenter.getX()+cellWidth/2+fWrappingThickness/2, 0, 0));
				}
				if(row != Nrow-1){ // New horizontal reflector layer.
					std::stringstream stream2; stream2 << "Wrapping-" << col << "," << row;
					mylarHorizLayer_physV[col][row] = addToDetectorBody(mylarHorizLayer_logV, stream2.str().c_str(), G4ThreeVector(cellCenter.getX(), cellCenter.getY()+cellHeight/2+fWrappingThickness/2, 0));
				}
			}
		}
	}

	// Define logical reflector surfaces.
	if(WrappingEnabled()){ 
		for(int col = 0; col < Ncol; col++){
			for(int row = 0; row < Nrow; row++){
				G4PVPlacement *cellPhysical = cellScint_physV[col][row];
				
				int leftCol = col-1;
				int rightCol = col+1;
				int downRow = row-1;
				int upRow = row+1;
				
				// Border with the outer wrapping.
				if((col == 0 || row == 0) || (col == Ncol-1 || row == Nrow-1)) 
					new G4LogicalBorderSurface("Wrapping", cellPhysical, wrapping_physV, wrappingOpSurf);
				
				// Internal reflector layers.
				if(leftCol >= 0 && leftCol < Ncol) // Left side vertical layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarVertLayer_physV.at(col-1), wrappingOpSurf);
				if(rightCol >= 0 && rightCol < Ncol) // Right side vertical layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarVertLayer_physV.at(col), wrappingOpSurf);
				if(downRow >= 0 && downRow < Nrow) // Bottom side horizontal layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarHorizLayer_physV.at(col).at(row-1), wrappingOpSurf);
				if(upRow >= 0 && upRow < Nrow) // Top side vertical layer.
					new G4LogicalBorderSurface("Wrapping", cellPhysical, mylarHorizLayer_physV.at(col).at(row), wrappingOpSurf);
			}
		}
	}
	
	// Update the scintillator copy number.
	scintCopyNum += Nrow*Ncol;
	
	// Update the Z offset and layer width/height
	layerSizeX = fDetectorWidth;
	layerSizeY = fDetectorHeight;
	offsetZ = fDetectorLength/2;
}

///////////////////////////////////////////////////////////////////////////////
// class ellipticalType
///////////////////////////////////////////////////////////////////////////////

void ellipticalType::prepareToBuild(){
	if(fTrapezoidLength <= 0) // User failed to specify the trapezoid length, use a default
		fTrapezoidLength = 10*mm;

	// Width of the detector (defined by the trapezoid length, trapezoid angle, and SiPM dimensions).
	fDetectorWidth = 2*(pmtWidth/2+(fTrapezoidLength/std::tan(fTrapezoidAngle*deg)))*mm;
	fDetectorLength += 2*fTrapezoidLength;
	
	// Set the maximum detector body size
	maxBodySize = G4ThreeVector(fDetectorWidth, fDetectorHeight, fDetectorLength);
}

void ellipticalType::buildDetector(){
	const G4double deltaWidth = fWrappingThickness/std::sin(fTrapezoidAngle*deg);
	const G4double bodyLength = fDetectorLength - 2*fTrapezoidLength;

	// Create the geometry.
	G4Trd *innerTrapezoid = new G4Trd("innerTrapezoid", fDetectorWidth/2, pmtWidth/2, fDetectorHeight/2, pmtHeight/2, fTrapezoidLength/2);
	G4Trd *outerTrapezoid = new G4Trd("outerTrapezoid", fDetectorWidth/2+deltaWidth, pmtWidth/2+deltaWidth, fDetectorHeight/2+deltaWidth, pmtHeight/2+deltaWidth, fTrapezoidLength/2);
	G4Box *innerBody = new G4Box("innerBody", fDetectorWidth/2, fDetectorHeight/2, bodyLength/2);
	G4Box *outerBody = new G4Box("outerBody", fDetectorWidth/2+deltaWidth, fDetectorHeight/2+deltaWidth, bodyLength/2);

	// Build the detector body using unions.
	G4RotationMatrix *trapRot = new G4RotationMatrix;
	trapRot->rotateY(180*deg);
	G4UnionSolid *scintBody1 = new G4UnionSolid("", innerBody, innerTrapezoid, 0, G4ThreeVector(0, 0, +bodyLength/2+fTrapezoidLength/2));
	G4UnionSolid *scintBody2 = new G4UnionSolid("scint", scintBody1, innerTrapezoid, trapRot, G4ThreeVector(0, 0, -bodyLength/2-fTrapezoidLength/2));
	G4LogicalVolume *scint_logV = new G4LogicalVolume(scintBody2, scintMaterial, "scint_logV");	
    scint_logV->SetVisAttributes(scintVisAtt);

	// Place the scintillator inside the assembly.
	G4PVPlacement *ellipseBody_physV = addToDetectorBody(scint_logV, "Scint");

	scintBody_physV.push_back(ellipseBody_physV);

	// Build the wrapping.
	G4PVPlacement *ellipseWrapping_physV = NULL;
	if(WrappingEnabled()){
		G4UnionSolid *wrappingBody1 = new G4UnionSolid("", outerBody, outerTrapezoid, 0, G4ThreeVector(0, 0, +bodyLength/2+fTrapezoidLength/2));
		G4UnionSolid *wrappingBody2 = new G4UnionSolid("", wrappingBody1, outerTrapezoid, trapRot, G4ThreeVector(0, 0, -bodyLength/2-fTrapezoidLength/2));
		G4SubtractionSolid *wrappingBody3 = new G4SubtractionSolid("wrapping", wrappingBody2, scintBody2);
		G4LogicalVolume *wrapping_logV = new G4LogicalVolume(wrappingBody3, wrappingMaterial, "wrapping_logV");		
		wrapping_logV->SetVisAttributes(wrappingVisAtt);

		// Place the wrapping around the scintillator.
		ellipseWrapping_physV = addToDetectorBody(wrapping_logV, "Wrapping");
	}

	// Account for the trapezoids and update the layer width/height
	layerSizeX = fDetectorWidth;
	layerSizeY = fDetectorHeight;
	offsetZ = fDetectorLength/2;
	
	// Directly modify the size of the grease layer.
	layerSizeX = pmtWidth;

    // Reflective wrapping.
    if(WrappingEnabled())
	    new G4LogicalBorderSurface("Wrapping", ellipseBody_physV, ellipseWrapping_physV, wrappingOpSurf);
}

///////////////////////////////////////////////////////////////////////////////
// class rectangularType
///////////////////////////////////////////////////////////////////////////////

void rectangularType::prepareToBuild(){
	// Set the maximum detector body size
	maxBodySize = G4ThreeVector(fDetectorWidth, fDetectorHeight, fDetectorLength);
}

void rectangularType::buildDetector(){
	G4Box *plateBody = new G4Box("", fDetectorWidth/2, fDetectorHeight/2, fDetectorLength/2);
    G4LogicalVolume *plateBody_logV = new G4LogicalVolume(plateBody, scintMaterial, "plateBody_logV");
    plateBody_logV->SetVisAttributes(scintVisAtt);

	// Place the scintillator inside the assembly.
	G4PVPlacement *plateBody_physV = addToDetectorBody(plateBody_logV, "Scint");

	scintBody_physV.push_back(plateBody_physV);

	// Build the wrapping.
	if(WrappingEnabled()){
		G4Box *plateWrappingBox = new G4Box("", fDetectorWidth/2 + fWrappingThickness, fDetectorHeight/2 + fWrappingThickness, fDetectorLength/2);
		G4SubtractionSolid *plateWrapping = new G4SubtractionSolid("", plateWrappingBox, plateBody);
		G4LogicalVolume *plateWrapping_logV = new G4LogicalVolume(plateWrapping, wrappingMaterial, "plateWrapping_logV");
		plateWrapping_logV->SetVisAttributes(wrappingVisAtt);
		
		// Place the wrapping around the scintillator.
		G4PVPlacement *plateWrapping_physV = addToDetectorBody(plateWrapping_logV, "Wrapping");
		
		// Reflective wrapping.
		
		new G4LogicalBorderSurface("Wrapping", plateBody_physV, plateWrapping_physV, wrappingOpSurf);
	}
	
	// Update the Z offset and layer width/height
	layerSizeX = fDetectorWidth;
	layerSizeY = fDetectorHeight;
	offsetZ = fDetectorLength/2;
}

///////////////////////////////////////////////////////////////////////////////
// class cylindricalType
///////////////////////////////////////////////////////////////////////////////

void cylindricalType::prepareToBuild(){
	SetSquarePMTs(false); // Disable square PMTs
	
	// Set the maximum detector body size. Note here that X=Y=fDetectorWidth
	maxBodySize = G4ThreeVector(fDetectorWidth, fDetectorWidth, fDetectorLength);
}

void cylindricalType::buildDetector(){
	// Make sure the height and width match
	fDetectorHeight = fDetectorWidth;

    G4Tubs *cylinderBody = new G4Tubs("scintBody", 0, fDetectorWidth/2, fDetectorLength/2, 0, 2*CLHEP::pi);
    G4LogicalVolume *cylinderBody_logV = new G4LogicalVolume(cylinderBody, scintMaterial, "cylinderBody_logV");
    cylinderBody_logV->SetVisAttributes(scintVisAtt);

	// Place the scintillator inside the assembly.
	G4PVPlacement *cylinderBody_physV = addToDetectorBody(cylinderBody_logV, "Scint");

	scintBody_physV.push_back(cylinderBody_physV);

	// Build the wrapping.
	if(WrappingEnabled()){
		G4Tubs *cylinderWrappingBox = new G4Tubs("outerWrapping", 0, fDetectorWidth/2 + fWrappingThickness, fDetectorLength/2, 0, 2*CLHEP::pi);
		G4SubtractionSolid *cylinderWrapping = new G4SubtractionSolid("", cylinderWrappingBox, cylinderBody);
		G4LogicalVolume *cylinderWrapping_logV = new G4LogicalVolume(cylinderWrapping, wrappingMaterial, "cylinderWrapping_logV");
		cylinderWrapping_logV->SetVisAttributes(wrappingVisAtt);
		
		// Place the wrapping around the scintillator.
		G4PVPlacement *cylinderWrapping_physV = addToDetectorBody(cylinderWrapping_logV, "Wrapping");	
		
		// Reflective wrapping.
		new G4LogicalBorderSurface("Wrapping", cylinderBody_physV, cylinderWrapping_physV, wrappingOpSurf);
	}
	
	// Update the Z offset and layer width/height
	layerSizeX = fDetectorWidth;
	layerSizeY = fDetectorHeight;
	offsetZ = fDetectorLength/2;
}
