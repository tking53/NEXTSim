#ifndef CENTER_OF_MASS_HPP
#define CENTER_OF_MASS_HPP

#include "G4ThreeVector.hh"

class centerOfMass{
  public:
	centerOfMass() : totalMass(0) { }

	bool empty() const { return (totalMass == 0); }
	
	double getTotalMass() const { return totalMass; }
	
	G4ThreeVector getCenter() const;

	double getCenterX() const;
	
	double getCenterY() const;
	
	double getCenterZ() const;
	
	void clear();
	
	void addPoint(const G4ThreeVector &v, const double &mass=1);
  
	void print();
  
  private:
	double totalMass;
	
	G4ThreeVector center;
};

#endif
