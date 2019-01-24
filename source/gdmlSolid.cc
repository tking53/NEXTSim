#include "G4VisAttributes.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"

#ifdef USE_GDML
#include "G4GDMLParser.hh"
#endif

#include "gdmlSolid.hh"

///////////////////////////////////////////////////////////////////////////////
// class gdmlFrame
///////////////////////////////////////////////////////////////////////////////

gdmlSegment::gdmlSegment() : rot(NULL), physV(NULL), logV(NULL) {
	unit[0] = G4ThreeVector(1, 0, 0);
	unit[1] = G4ThreeVector(0, 1, 0);
	unit[2] = G4ThreeVector(0, 0, 1);
}

#ifdef USE_GDML
gdmlSegment::gdmlSegment(G4GDMLParser *parser, const G4int &daughter) : rot(NULL), physV(NULL), logV(NULL) {
	// Get the parent.
	G4VPhysicalVolume *W = parser->GetWorldVolume();
	G4LogicalVolume *W_log = W->GetLogicalVolume();

	// Get the geometry.
	physV = W_log->GetDaughter(daughter);
	logV = physV->GetLogicalVolume();

	// Get the rotation.
	G4ThreeVector r = parser->GetRotation(logV->GetName()+"_rot");
	rot = new G4RotationMatrix();
	rot->rotateX(r.getX()*rad);
	rot->rotateY(r.getY()*rad);
	rot->rotateZ(r.getZ()*rad);
	
	// Get the position.
	pos = parser->GetPosition(logV->GetName()+"_pos");
	
	unit[0] = G4ThreeVector(1, 0, 0);
	unit[1] = G4ThreeVector(0, 1, 0);
	unit[2] = G4ThreeVector(0, 0, 1);

	for(size_t i = 0; i < 3; i++){ 
		unit[i] = (*rot) * unit[i];
	}
	
	// Rotate the frame of the object.
	pos = this->transform(pos);
}
#endif

G4ThreeVector gdmlSegment::getOffset(const G4RotationMatrix &matrix) const {
	double values[3];
	G4ThreeVector newUnit[3] = {unit[0], unit[1], unit[2]};
	for(size_t i = 0; i < 3; i++){ // Rotate the frame of the object.
		newUnit[i] = matrix * newUnit[i];
		values[i] = newUnit[i].getX()*pos.getX() + newUnit[i].getY()*pos.getY() + newUnit[i].getZ()*pos.getZ();
	}
	return G4ThreeVector(values[0], values[1], values[2]);
}

void gdmlSegment::rotate(const G4RotationMatrix &matrix){
	for(size_t i = 0; i < 3; i++){ // Rotate the frame of the object.
		unit[i] = matrix * unit[i];
	}
}

void gdmlSegment::rotate(const double &x, const double &y, const double &z){
	G4RotationMatrix mat;
	mat.rotateX(x*deg);
	mat.rotateY(y*deg);
	mat.rotateZ(z*deg);
	this->rotate(mat);
}

G4ThreeVector gdmlSegment::transform(const G4ThreeVector &vec) const {
	double values[3];
	for(size_t i = 0; i < 3; i++){ // Rotate the frame of the object.
		values[i] = unit[i].getX()*vec.getX() + unit[i].getY()*vec.getY() + unit[i].getZ()*vec.getZ();
	}
	return G4ThreeVector(values[0], values[1], values[2]);	
}

///////////////////////////////////////////////////////////////////////////////
// class gdmlSolid
///////////////////////////////////////////////////////////////////////////////

gdmlSolid::gdmlSolid(const char *fname){
	this->read(fname);
}

G4LogicalVolume *gdmlSolid::read(const char *fname, G4OpticalSurface *surface/*=NULL*/, const bool &checkOverlaps/*=false*/){
#ifdef USE_GDML
	G4GDMLParser parser;
	
	parser.Read(fname, false);

	world = parser.GetWorldVolume();
	worldLogV = world->GetLogicalVolume();
	
	// Set the name of the object.
	name = worldLogV->GetName();
	
	for(size_t i = 0; i < 3; i++){
		size[i] = -1E10;
	}

	for(G4int i = 0; i < worldLogV->GetNoDaughters(); i++){ // Loop over the daughters.
		daughters.push_back(gdmlSegment(&parser, i));
		
		// Get the maximum size of the object.
		G4ThreeVector p = daughters.back().getOffset();
		size[0] = std::max(size[0], std::fabs(p.getX()));
		size[1] = std::max(size[1], std::fabs(p.getY()));
		size[2] = std::max(size[2], std::fabs(p.getZ()));
		
		nDaughters++;
	}

	G4NistManager* manNist = G4NistManager::Instance();

	// Define the parent object.
	parent_physV = new G4Box("assembly", size[0], size[1], size[2]);
	parent_logV = new G4LogicalVolume(parent_physV, manNist->FindOrBuildMaterial("G4_AIR"), "assembly_logV");
	parent_logV->SetVisAttributes(G4VisAttributes::Invisible);
	
	for(std::vector<gdmlSegment>::iterator iter = daughters.begin(); iter != daughters.end(); iter++){ // Place the daughters within the parent.
		G4RotationMatrix *r = iter->getRotationMatrix();
		G4LogicalVolume *log = iter->getLogicalVolume();
		log->SetMaterial(manNist->FindOrBuildMaterial("G4_SILICON_DIOXIDE"));
		placements.push_back(new G4PVPlacement(r, G4ThreeVector(0, 0, 0), log, log->GetName(), parent_logV, true, 0, checkOverlaps));
	}

	if(surface){ // Define logical border surfaces.
		for(std::vector<G4PVPlacement*>::iterator iter1 = placements.begin(); iter1 != placements.end(); iter1++){ 
			for(std::vector<G4PVPlacement*>::iterator iter2 = placements.begin(); iter2 != placements.end(); iter2++){
				if(iter1 == iter2) continue; // No border with itself.
				borders.push_back(new G4LogicalBorderSurface("Surface", (*iter1), (*iter2), surface));
			}
		}
	}
	
	return parent_logV;
#else
	return NULL;
#endif
}

void gdmlSolid::placeSolid(G4RotationMatrix *rotation, const G4ThreeVector &position, G4LogicalVolume *parent, std::vector<G4PVPlacement*> &volumes, const bool &checkOverlaps/*=false*/){
	for(std::vector<gdmlSegment>::iterator iter = daughters.begin(); iter != daughters.end(); iter++){ // Place the daughters within the parent.
		G4LogicalVolume *log = iter->getLogicalVolume();
		volumes.push_back(new G4PVPlacement(rotation, position, log, log->GetName(), parent, true, 0, checkOverlaps));
	}
}

void gdmlSolid::setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, G4PVPlacement *phys){
	for(std::vector<G4PVPlacement*>::iterator iter = placements.begin(); iter != placements.end(); iter++){ 
		new G4LogicalBorderSurface(borderName, (*iter), phys, surface);
	}
}

void gdmlSolid::setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, const std::vector<G4PVPlacement*> &phys){
	for(std::vector<G4PVPlacement*>::const_iterator iter1 = phys.begin(); iter1 != phys.end(); iter1++){ 
		for(std::vector<G4PVPlacement*>::const_iterator iter2 = phys.begin(); iter2 != phys.end(); iter2++){
			if(iter1 == iter2) continue; // No border with itself.
			new G4LogicalBorderSurface(borderName, (*iter1), (*iter2), surface);
		}
	}
}

void gdmlSolid::setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, const std::vector<G4PVPlacement*> &p1, G4PVPlacement *p2){
	for(std::vector<G4PVPlacement*>::const_iterator iter1 = p1.begin(); iter1 != p1.end(); iter1++){ 
		new G4LogicalBorderSurface(borderName, (*iter1), p2, surface);
	}
}

void gdmlSolid::setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, const std::vector<G4PVPlacement*> &p1, const std::vector<G4PVPlacement*> &p2){
	for(std::vector<G4PVPlacement*>::const_iterator iter1 = p1.begin(); iter1 != p1.end(); iter1++){ 
		for(std::vector<G4PVPlacement*>::const_iterator iter2 = p2.begin(); iter2 != p2.end(); iter2++){
			if(iter1 == iter2) continue; // No border with itself.
			new G4LogicalBorderSurface(borderName, (*iter1), (*iter2), surface);
		}
	}
}

void gdmlSolid::rotate(const double &x, const double &y, const double &z){
	G4RotationMatrix matrix;
	matrix.rotateX(x*deg);
	matrix.rotateY(y*deg);
	matrix.rotateZ(z*deg);
	for(std::vector<gdmlSegment>::iterator iter = daughters.begin(); iter != daughters.end(); iter++){ 
		iter->rotate(matrix);
	}
}

void gdmlSolid::clear(){
	name = ""; 
	parent_physV = NULL;
	parent_logV = NULL;
	world = NULL;
	worldLogV = NULL;
	nDaughters = 0;

	for(size_t i = 0; i < placements.size(); i++){
		delete placements.at(i);
	}
	for(size_t i = 0; i < placements.size(); i++){
		delete borders.at(i);
	}
	
	placements.clear();
	borders.clear();
	daughters.clear();
	
	size[0] = 0;
	size[1] = 0;
	size[2] = 0;
}
