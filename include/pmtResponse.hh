#ifndef PMT_RESPONSE_HH
#define PMT_RESPONSE_HH

#include <vector>

class TGraph;
class TF1;

class spectralResponse{
  public:
	/// Default constructor.
	spectralResponse() : spectrum(NULL), extrapolateLow(NULL), extrapolateHigh(NULL), xmin(0), xmax(0) { }

	/// Destructor.
	~spectralResponse();

	/// Load response function from a file.
	bool load(const char *fname);

	/// Return the PMT quantum efficiency for a given wavelength.
	double eval(const double &wavelength);

  private:
	TGraph *spectrum;
	
	TF1 *extrapolateLow;
	TF1 *extrapolateHigh;
	
	double xmin, xmax;
	
	void scanSpectrum();
	
	void close();
};

class pmtResponse{
  public:
	/// Default constructor.
	pmtResponse();
	
	/// Pulse constructor.
	pmtResponse(const double &risetime_, const double &falltime_);

	/// Destructor.
	~pmtResponse();

	size_t getPulseLength() const { return pulseLength; }

	/// Return the maximum value of the pulse.
	float getMaximum() const { return maximum; }

	/// Return a pointer to the array of Poly CFD parameters.
	double *getCfdParams() { return cfdPar; }

	/// Return the maximum ADC index of the pulse.
	unsigned short getMaximumIndex() const { return maxIndex; }

	double *getRawPulse(){ return rawPulse; }
	
	unsigned short *getDigitizedPulse(){ return pulseArray; }

	/// Set the rise time of the single photon pulse (in ns).
	void setRisetime(const double &risetime_);
	
	/// Set the decay time of the single photon pulse (in ns).
	void setFalltime(const double &falltime_);

	/// Set the FWHM spread in the photo-electron transit time (in ns).
	void setTransitTimeSpread(const double &spread_){ timeSpread = spread_; }

	/// Set the time delay of the pulse (in ns).
	void setTraceDelay(const double &traceDelay_){ traceDelay = traceDelay_; }

	/// Set the gain. 
	void setGain(const double &gain_){ gain = gain_; }

	/// Set the length of the pulse in ADC bins.
	void setPulseLength(const size_t &len);

	/// Set the length of the pulse in nanoseconds.
	void setPulseLengthInNanoSeconds(const double &length);

	/// Set the dynamic bit range of the ADC.
	void setBitRange(const size_t &len);

	/// Disable use of PMT spectral response function.
	void disableSpectralResponse(){ useSpectralResponse = false; }

	/// Load PMT spectral response from root file.
	bool loadSpectralResponse(const char *fname);

	/// Add a photon signal to the raw pulse.
	void addPhoton(const double &arrival, const double &wavelength=0);

	/// "Digitize" the raw light pulse.
	void digitize(const double &baseline=0, const double &jitter=0);

	/// Integrate the baseline corrected trace for QDC in the range [start_, stop_] and return the result.
	float integratePulse(const size_t &start_=0, const size_t &stop_=0);

	/// Integrate the baseline corrected trace for QDC in the range [maxIndex-start_, maxIndex+stop_] and return the result.
	float integratePulseFromMaximum(const short &start_=5, const short &stop_=10);

	/// Perform traditional CFD analysis on the waveform.
	float analyzeCFD(const float &F_=0.5, const size_t &D_=1, const size_t &L_=1);
	
	/// Perform polynomial CFD analysis on the waveform.
	float analyzePolyCFD(const float &F_=0.5);

	/// Copy the digitized trace into another array.
	void copyTrace(unsigned short *arr, const size_t &len);

	/// Copy the digitized trace into a vector.
	void copyTrace(std::vector<unsigned short> &vec);

	/// Clear the ADC trace and all analysis variables.
	void clear();

	/// Print pulse parameters.
	void print();

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

  private:
	double cfdPar[7]; /// Cfd fitting parameters.	

	double risetime;
	double falltime;
	double timeSpread;
	double amplitude;
	double peakOffset;
	double peakMaximum;
	double traceDelay;
	double gain;
	
	float maximum; /// The maximum value of the trace.
	float baseline; /// Baseline offset of the trace.

	unsigned short maxIndex; /// The index of the maximum trace bin (in ADC clock ticks).

	unsigned int adcBins;

	size_t pulseLength; /// Length of the raw and digitized ADC light pulses.
	
	bool isDigitized;
	bool useSpectralResponse;
	
	double *rawPulse;

	unsigned short *pulseArray;

	spectralResponse spec;
	
	void update();
	
	double eval(const double &t, const double &dt=0);
	
	/// Find the pulse maximum.
	float findMaximum();
};

#endif
