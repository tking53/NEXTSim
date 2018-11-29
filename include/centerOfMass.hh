#ifndef CENTER_OF_MASS_HPP
#define CENTER_OF_MASS_HPP

#include <limits>

#include "G4ThreeVector.hh"

class TGraph;
class G4Step;

class TraceProcessor{
  public:
	/// Default constructor.
	TraceProcessor() : maximum(0), maxIndex(0) { }
	
	/// Destructor.
	~TraceProcessor();
	
	/// Return the maximum value of the pulse.
	float GetMaximum() const { return maximum; }

	unsigned short GetMaximumIndex() const { return maxIndex; }
	
	/// Integrate the baseline corrected trace for QDC in the range [start_, stop_] and return the result.
	float IntegratePulse(unsigned short *pulse, const size_t &len, const size_t &start_=0, const size_t &stop_=0);

	/// Perform traditional CFD analysis on the waveform.
	float AnalyzeCFD(unsigned short *pulse, const size_t &len, const float &F_=0.5, const size_t &D_=1, const size_t &L_=1);
	
	/// Perform polynomial CFD analysis on the waveform.
	float AnalyzePolyCFD(unsigned short *pulse, const size_t &len, const float &F_=0.5);
	
  private:
	float maximum; /// The maximum value of the trace.

	double cfdPar[7]; /// Cfd fitting parameters.
	
	unsigned short maxIndex; /// The index of the maximum trace bin (in ADC clock ticks).
	
	/// Find the pulse maximum.
	float FindMaximum(unsigned short *pulse, const size_t &len);
	
	/** Calculate the parameters for a second order polynomial which passes through 3 points.
	  * \param[in]  x0 - Initial x value. Sequential x values are assumed to be x0, x0+1, and x0+2.
	  * \param[in]  y  - Pointer to the beginning of the array of unsigned shorts containing the three y values.
	  * \param[out] p  - Pointer to the array of doubles for storing the three polynomial parameters.
	  * \return The maximum/minimum of the polynomial.
	  */
	static double calculateP2(const short &x0, unsigned short *y, double *p);

	/** Calculate the parameters for a third order polynomial which passes through 4 points.
	  * \param[in]  x0 - Initial x value. Sequential x values are assumed to be x0, x0+1, x0+2, and x0+3.
	  * \param[in]  y  - Pointer to the beginning of the array of unsigned shorts containing the four y values.
	  * \param[out] p  - Pointer to the array of doubles for storing the three polynomial parameters.
	  * \return The local maximum/minimum of the polynomial.
	  */
	static double calculateP3(const short &x0, unsigned short *y, double *p);
};

class pmtResponse{
  public:
	pmtResponse() : risetime(4.0), falltime(20.0), amplitude(0), peakOffset(0), peakMaximum(0), traceDelay(0), gain(1E4) { this->update(); }
	
	pmtResponse(const double &risetime_, const double &falltime_) : risetime(risetime_), falltime(falltime_), amplitude(0), peakOffset(0), peakMaximum(0), gain(1) { this->update(); }

	/// Set the rise time of the pulse (in ns).
	void setRisetime(const double &risetime_);
	
	/// Set the decay time of the pulse (in ns).
	void setFalltime(const double &falltime_);

	/// Set the time delay of the pulse (in ns).
	void setTraceDelay(const double &traceDelay_){ traceDelay = traceDelay_; }

	/// Set the gain. 
	void setGain(const double &gain_){ gain = gain_; }

	/// "Digitize" a photon signal and add it to the pulse.
	void digitize(const double &arrival, double *arr, const size_t &len);

	/// Print pulse parameters.
	void print();

  private:
	double risetime;
	double falltime;
	double amplitude;
	double peakOffset;
	double peakMaximum;
	double traceDelay;
	double gain;
	
	void update();
	
	double eval(const double &t, const double &dt=0);
};

class centerOfMass{
  public:
	centerOfMass() : Ncol(-1), Nrow(-1), Npts(0), NnotDetected(0), totalMass(0), t0(std::numeric_limits<double>::max()), tSum(0), lambdaSum(0),
	                 activeWidth(0), activeHeight(0), pixelWidth(0), pixelHeight(0), center(0, 0, 0), spec(NULL), useSpectralResponse(false) { }

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

	size_t getNumDetected() const { return Npts; }
	
	size_t getNumNotDetected() const { return NnotDetected; }

	short getNumColumns() const { return Ncol; }
	
	short getNumRows() const { return Nrow; }
	
	double getActiveAreaWidth() const { return activeWidth; }
	
	double getActiveAreaHeight() const { return activeHeight; }
	
	void getArrivalTimes(unsigned short *arr, const size_t &len) const ;
	
	float getPulsePhase(unsigned short *arr, const size_t &len, const float &F_=0.5) const ;
	
	short setNumColumns(const short &col_);
	
	short setNumRows(const short &row_);

	double setActiveAreaWidth(const double &width_);
	
	double setActiveAreaHeight(const double &height_);
	
	void setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_);
	
	bool loadSpectralResponse(const char *fname, const char *name="spec");
	
	void disableSpectralResponse(){ useSpectralResponse = false; }
	
	void clear();
	
	bool addPoint(const G4Step *step, const double &mass=1);
  
	void print();
  
  private:
	double arrivalTimes[100];
	
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
	
	G4ThreeVector center;
	
	pmtResponse response;
	
	TGraph *spec;
	
	bool useSpectralResponse;
};

#endif
