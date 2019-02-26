#ifndef CENTER_OF_MASS_HPP
#define CENTER_OF_MASS_HPP

#include <limits>
#include <vector>

#include "G4ThreeVector.hh"

#include "pmtResponse.hh"

class G4Step;

class centerOfMass{
  public:
	centerOfMass() : Ncol(-1), Nrow(-1), Npts(0), NnotDetected(0), totalMass(0), t0(std::numeric_limits<double>::max()), tSum(0), lambdaSum(0),
	                 activeWidth(0), activeHeight(0), pixelWidth(0), pixelHeight(0), center(0, 0, 0), response() { }

	~centerOfMass();

	bool empty() const { return (totalMass == 0); }
	
	double getTotalMass() const { return totalMass; }
	
	G4ThreeVector getCenter() const;

	double getCenterX() const;
	
	double getCenterY() const;
	
	double getCenterZ() const;

	bool getCenterSegment(G4ThreeVector &pos, short &col, short &row) const ;

	bool getCenterSegment(short &col, short &row) const ;
	
	double getDetectionEfficiency() const { return (Npts > 0 ? double(Npts)/(Npts+NnotDetected) : -1); }

	double getAvgWavelength() const { return (Npts > 0 ? lambdaSum/Npts : -1); }

	double getAvgArrivalTime() const { return (Npts > 0 ? tSum/Npts : -1); }

	double getMinArrivalTime() const { return (Npts > 0 ? t0 : -1); }

	size_t getNumDetected() const { return Npts; }
	
	size_t getNumNotDetected() const { return NnotDetected; }

	short getNumColumns() const { return Ncol; }
	
	short getNumRows() const { return Nrow; }
	
	double getActiveAreaWidth() const { return activeWidth; }
	
	double getActiveAreaHeight() const { return activeHeight; }
	
	pmtResponse *getPmtResponse(){ return &response; }
	
	void getAnodeCurrents(double *array) const ;
	
	double getReconstructedX() const ;
	
	double getReconstructedY() const ;
	
	short setNumColumns(const short &col_);
	
	short setNumRows(const short &row_);

	double setActiveAreaWidth(const double &width_);
	
	double setActiveAreaHeight(const double &height_);
	
	void setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_);
	
	bool loadSpectralResponse(const char *fname);
	
	bool loadGainMatrix(const char *fname);
	
	void clear();
	
	bool addPoint(const double &energy, const double &time, const G4ThreeVector &position, const double &mass=1);
  
	void print();
  
  private:
	short Ncol;
	short Nrow;
	
	size_t Npts;
	size_t NnotDetected;
	
	double totalMass;
	double t0;
	double tSum;
	double lambdaSum;
	
	double activeWidth;
	double activeHeight;
	
	double pixelWidth;
	double pixelHeight;
	
	double anodeCurrent[4];
	
	G4ThreeVector center;
	
	pmtResponse response;
	
	std::vector<std::vector<double> > gainMatrix;
	
	double getGain(const int &x, const int &y);
	
	double *getCurrent(const int &x, const int &y);
};

#endif
