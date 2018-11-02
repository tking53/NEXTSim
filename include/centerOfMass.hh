#ifndef CENTER_OF_MASS_HPP
#define CENTER_OF_MASS_HPP

#include <limits>

#include "G4ThreeVector.hh"

class centerOfMass{
  public:
	centerOfMass() : Npts(0), totalMass(0), t0(std::numeric_limits<double>::max()), tSum(0) { }

	bool empty() const { return (totalMass == 0); }
	
	double getTotalMass() const { return totalMass; }
	
	G4ThreeVector getCenter() const;

	double getCenterX() const;
	
	double getCenterY() const;
	
	double getCenterZ() const;

	double getAvgArrivalTime() const { return (Npts > 0 ? tSum/Npts : -1); }

	double getMinArrivalTime() const { return (Npts > 0 ? t0 : -1); }
	
	void getArrivalTimes(unsigned short *arr, const size_t &len, const size_t &offset=0);
	
	void clear();
	
	void addPoint(const G4ThreeVector &v, const double &time, const double &mass=1);
  
	void print();
  
  private:
	unsigned short arrivalTimes[100];
	
	size_t Npts;
	
	double totalMass;
	
	double t0;
	
	double tSum;
	
	G4ThreeVector center;
};

#endif
