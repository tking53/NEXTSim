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
	
	pmtResponse *getAnodeResponse(){ return anodeResponse; }
	
	void getAnodeCurrents(double *array) const ;
	
	double getReconstructedX() const ;
	
	double getReconstructedY() const ;
	
	void getGainMatrix(std::vector<std::vector<double> > &matrix) const { matrix = gainMatrix; }
	
	void getCountMatrix(std::vector<std::vector<int> > &matrix) const { matrix = countMatrix; }
	
	short setNumColumns(const short &col_);
	
	short setNumRows(const short &row_);

	double setActiveAreaWidth(const double &width_);
	
	double setActiveAreaHeight(const double &height_);
	
	void setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_);
	
	bool loadSpectralResponse(const char *fname);
	
	bool loadGainMatrix(const char *fname);

	void copySpectralResponse(centerOfMass *other);
	
	void copyGainMatrix(centerOfMass *other);
	
	void clear();
	
	bool addPoint(const double &energy, const double &time, const G4ThreeVector &position, const double &mass=1);

	void printCounts() const ;

	void print() const ;
  
  private:
	short Ncol; ///< Number of PMT anode columns
	short Nrow; ///< Number of PMT anode rows
	
	size_t Npts; ///< Number of photons added to the weighted average
	size_t NnotDetected; ///< Number of photons not detected by the sensitive surface
	
	double totalMass; ///< Total photon weight for the weighted average
	double t0; ///< Photon minimum time-of-arrival
	double tSum; ///< Sum of all photon arrival times
	double lambdaSum; ///< Sum of all photon wavelengths
	
	double activeWidth; ///< Width of the active anode area (in mm)
	double activeHeight; ///< Height of the active anode area (in mm)
	
	double pixelWidth; ///< Width of each individual PMT anode
	double pixelHeight; ///< Height of each individual PMT anode
	
	double anodeCurrent[4]; ///< Sum of the current at each Anger Logic readout
	
	G4ThreeVector center; ///< 3d sum of all photon detection points
	
	pmtResponse response; ///< Light pulse response of the dynode
	
	pmtResponse anodeResponse[4]; // Light pulse response of the four Anger Logic readouts
	
	std::vector<std::vector<double> > gainMatrix; ///< Matrix containing the gain of each PSPMT anode (in percent)
	std::vector<std::vector<int> > countMatrix; ///< Matrix containing the number of photon counts of each PSPMT anode
	
	void increment(const int &x, const int &y);
	
	double getGain(const int &x, const int &y);
	
	double *getCurrent(const int &x, const int &y);
};

#endif
