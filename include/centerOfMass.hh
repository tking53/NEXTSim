#ifndef CENTER_OF_MASS_HPP
#define CENTER_OF_MASS_HPP

#include <limits>

#include "G4ThreeVector.hh"

class TGraph;
class G4Step;

class centerOfMass{
  public:
	centerOfMass() : Npts(0), NnotDetected(0), totalMass(0), t0(std::numeric_limits<double>::max()), tSum(0), lambdaSum(0), center(0, 0, 0), spec(NULL), useSpectralResponse(false) { }

	~centerOfMass();

	bool empty() const { return (totalMass == 0); }
	
	double getTotalMass() const { return totalMass; }
	
	G4ThreeVector getCenter() const;

	double getCenterX() const;
	
	double getCenterY() const;
	
	double getCenterZ() const;

	double getDetectionEfficiency() const { return (Npts > 0 ? double(Npts)/(Npts+NnotDetected) : -1); }

	double getAvgWavelength() const { return (Npts > 0 ? lambdaSum/Npts : -1); }

	double getAvgArrivalTime() const { return (Npts > 0 ? tSum/Npts : -1); }

	double getMinArrivalTime() const { return (Npts > 0 ? t0 : -1); }
	
	void getArrivalTimes(unsigned short *arr, const size_t &len, const size_t &offset=0);
	
	bool loadSpectralResponse(const char *fname, const char *name="spec");
	
	void disableSpectralResponse(){ useSpectralResponse = false; }
	
	void clear();
	
	bool addPoint(const G4Step *step, const double &mass=1);
  
	void print();
  
  private:
	unsigned short arrivalTimes[100];
	
	size_t Npts;
	size_t NnotDetected;
	
	double totalMass;
	
	double t0;
	
	double tSum;

	double lambdaSum;
	
	G4ThreeVector center;
	
	TGraph *spec;
	
	bool useSpectralResponse;
};

#endif
