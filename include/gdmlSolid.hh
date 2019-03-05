#include <vector>
#include <string>

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4RotationMatrix.hh"
#include "G4OpticalSurface.hh"

#include "globals.hh"

#ifdef USE_GDML
class G4GDMLParser;
#endif

class gdmlSegment{
  public:
	gdmlSegment();

#ifdef USE_GDML
	gdmlSegment(G4GDMLParser *parser, const G4int &daughter);
#endif
  
	G4ThreeVector getUnitX() const { return unit[0]; }
  	
  	G4ThreeVector getUnitY() const { return unit[1]; }
  	
	G4ThreeVector getUnitZ() const { return unit[2]; }

	G4ThreeVector getOffset() const { return pos; }
	
	G4ThreeVector getOffset(const G4RotationMatrix &matrix) const ;
	
	G4RotationMatrix *getRotationMatrix(){ return rot; }
	
	G4VPhysicalVolume *getPhysicalVolume(){ return physV; }
	
	G4LogicalVolume *getLogicalVolume(){ return logV; }

	void rotate(const G4RotationMatrix &matrix);
	
	void rotate(const double &x, const double &y, const double &z);

	G4ThreeVector transform(const G4ThreeVector &vec) const ;

  private:
	G4RotationMatrix *rot;
	G4VPhysicalVolume *physV;  
	G4LogicalVolume *logV;
	
	G4ThreeVector pos;
	G4ThreeVector unit[3];
};

class gdmlSolid{
  public:
	gdmlSolid() : name(), parent(NULL), parent_logV(NULL), parent_physV(NULL), world(NULL), worldLogV(NULL), nDaughters(0) { }
	
	gdmlSolid(const char *fname, const G4String &materialName);

	std::string getName() const { return name; }

	size_t getNumDaughters() const { return nDaughters; }

	bool isLoaded() const { return (nDaughters > 0); }

	double getWidth() const ;
	
	double getThickness() const ;
	
	double getLength() const ;

	double getSizeX() const { return 2*size.getX(); }
	
	double getSizeY() const { return 2*size.getY(); }
	
	double getSizeZ() const { return 2*size.getZ(); }	

	G4ThreeVector *getPosition(){ return &position; }
	
	G4RotationMatrix *getRotation(){ return &rotation; }

	G4Box *getBoundingBox(){ return parent; }
	
	G4LogicalVolume *getLogicalVolume(){ return parent_logV; }
	
	G4PVPlacement *getPhysicalVolume(){ return parent_physV; }

	G4LogicalVolume *getWorldLogicalVolume(){ return worldLogV; }

	G4LogicalVolume *read(const char *fname, const G4String &materialName, const bool &checkOverlaps=false);

	G4PVPlacement *placeSolid(G4LogicalVolume *parent_, const bool &checkOverlaps=false);
	
	G4PVPlacement *placeSolid(G4RotationMatrix *rot, const G4ThreeVector &pos, G4LogicalVolume *parent_, const bool &checkOverlaps=false);
	
	void placeSolid(G4RotationMatrix *rot, const G4ThreeVector &pos, G4LogicalVolume *parent_, std::vector<G4PVPlacement*> &volumes, const bool &checkOverlaps=false);

	void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface);

	void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, G4PVPlacement *phys);

	static void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, const std::vector<G4PVPlacement*> &phys);

	static void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, const std::vector<G4PVPlacement*> &p1, G4PVPlacement *p2);

	static void setLogicalBorders(const G4String &borderName, G4OpticalSurface *surface, const std::vector<G4PVPlacement*> &p1, const std::vector<G4PVPlacement*> &p2);

	void setPosition(const G4ThreeVector &pos_){ position = pos_; }
	
	void setRotation(const G4ThreeVector &rot_);

	void rotate(const double &x, const double &y, const double &z);

	void clear();

  private:
	std::string name;
  
	G4Box *parent;
	G4LogicalVolume *parent_logV;
	G4PVPlacement *parent_physV;
  
	G4VPhysicalVolume *world;
	G4LogicalVolume *worldLogV;
  
	std::vector<G4PVPlacement*> placements;
	std::vector<G4LogicalBorderSurface*> borders;

	std::vector<gdmlSegment> daughters;
	
	size_t nDaughters;
	
	G4ThreeVector position;
	G4RotationMatrix rotation;
	
	G4ThreeVector size;
};
