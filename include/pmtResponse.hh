#ifndef PMT_RESPONSE_HH
#define PMT_RESPONSE_HH

#include <vector>

class TGraph;
class TF1;

class spectralResponse{
  public:
	///< Default constructor.
	spectralResponse() : spectrum(NULL), extrapolateLow(NULL), extrapolateHigh(NULL), xmin(0), xmax(0) { }

	///< Destructor.
	~spectralResponse();

	void getRange(double &min_, double &max_);

	TGraph *getSpectrum(){ return spectrum; }
	
	TF1 *getExtrapolateLow(){ return extrapolateLow; }
	
	TF1 *getExtrapolateHigh(){ return extrapolateHigh; }

	void copy(spectralResponse *other);

	///< Load response function from a file.
	bool load(const char *fname);

	///< Return the PMT quantum efficiency for a given wavelength.
	double eval(const double &wavelength);

  private:
	TGraph *spectrum; ///< Pointer to a TGraph containing the PMT quantum efficiency spectrum (i.e. percentage vs. photon wavelength in nm)
	
	TF1 *extrapolateLow; ///< Pointer to a TF1 used to extrapolate below the minimum wavelenth in the quantum efficiency spectrum
	TF1 *extrapolateHigh; ///< Pointer to a TF1 used to extrapolate above the maximum wavelenth in the quantum efficiency spectrum
	
	double xmin; ///< Minimum optical photon wavelenth defined in the quantum efficiency spectrum (in nm)
	double xmax; ///< Maximum optical photon wavelenth defined in the quantum efficiency spectrum (in nm)
	
	void scanSpectrum();
	
	void close();
};

class pmtResponse{
  public:
	///< Default constructor.
	pmtResponse();
	
	///< Pulse constructor.
	pmtResponse(const double &risetime_, const double &falltime_);

	///< Destructor.
	~pmtResponse();

	size_t getPulseLength() const { return pulseLength; }

	///< Return the maximum value of the pulse.
	double getMaximum() const { return maximum; }

	///< Return the baseline of the pulse.
	double getBaseline() const { return baseline; }

	///< Return the pulse phase at the maximum.
	double getMaximumTime() const { return maximumTime; }

	///< Return a pointer to the array of Poly CFD parameters.
	double *getCfdParams() { return cfdPar; }

	///< Return the maximum ADC index of the pulse.
	unsigned short getMaximumIndex() const { return maxIndex; }

	///< Return a pointer to the raw light pulse array.
	double *getRawPulse(){ return rawPulse; }
	
	///< Return a pointer to the digitized pulse array.
	unsigned short *getDigitizedPulse(){ return pulseArray; }

	///< Return true if the digitized pulse is saturated and false otherwise.
	bool getPulseIsSaturated() const { return pulseIsSaturated; }

	///< Return the gain-weighted arrival time of the photon pulse.
	double getWeightedPhotonArrivalTime() const ;

	///< Return the minimum photon arrival time of the pulse.
	double getMinimumPhotonArrivalTime() const { return minimumArrivalTime; }

	///< Return a pointer to the PMT spectral response (quantum efficiency).
	spectralResponse *getSpectralResponse(){ return &spec; }

	///< Set the rise time of the single photon pulse (in ns).
	void setRisetime(const double &risetime_);
	
	///< Set the decay time of the single photon pulse (in ns).
	void setFalltime(const double &falltime_);

	///< Set the FWHM spread in the photo-electron transit time (in ns).
	void setTransitTimeSpread(const double &spread_){ timeSpread = spread_; }

	///< Set the time delay of the pulse (in ns).
	void setTraceDelay(const double &traceDelay_){ traceDelay = traceDelay_; }

	///< Set the gain. 
	void setGain(const double &gain_){ gain = gain_; }

	///< Set the length of the pulse in ADC bins.
	void setPulseLength(const size_t &len);

	///< Set the length of the pulse in nanoseconds.
	void setPulseLengthInNanoSeconds(const double &length);

	///< Set the dynamic bit range of the ADC.
	void setBitRange(const size_t &len);

	///< Set the single photon response function type.
	void setFunctionType(const int &type){ functionType = type; }

	///< Disable use of PMT spectral response function.
	void disableSpectralResponse(){ useSpectralResponse = false; }

	///< Load PMT spectral response from root file.
	bool loadSpectralResponse(const char *fname);

	///< Copy spectral response from another pmtResponse object.
	void copySpectralResponse(spectralResponse *spec_){ spec.copy(spec_); }

	///< Add a photon signal to the raw pulse.
	void addPhoton(const double &arrival, const double &wavelength=0, const double &gain_=1);

	///< "Digitize" the raw light pulse.
	void digitize(const double &baseline=0, const double &jitter=0);

	///< Integrate the baseline corrected trace for QDC in the range [start_, stop_] and return the result.
	double integratePulse(const size_t &start_=0, const size_t &stop_=0);

	///< Integrate the baseline corrected trace for QDC in the range [maxIndex-start_, maxIndex+stop_] and return the result.
	double integratePulseFromMaximum(const short &start_=5, const short &stop_=10);

	///< Perform traditional CFD analysis on the waveform.
	double analyzeCFD(const double &F_=0.5, const size_t &D_=1, const size_t &L_=1);
	
	///< Perform polynomial CFD analysis on the waveform.
	double analyzePolyCFD(const double &F_=0.5);

	///< Copy the digitized trace into another array.
	void copyTrace(unsigned short *arr, const size_t &len);

	///< Copy the digitized trace into a vector.
	void copyTrace(std::vector<unsigned short> &vec);

	///< Clear the ADC trace and all analysis variables.
	void clear();

	///< Print pulse parameters.
	void print();

	///< Print raw pulse.
	void printRaw();

	///< Return the period of the ADC clock in ns.
	static int getAdcClockTick();

	/** Calculate the parameters for a second order polynomial which passes through 3 points.
	  * @param x0 - Initial x value. Sequential x values are assumed to be x0, x0+1, and x0+2.
	  * @param y  - Pointer to the beginning of the array of unsigned shorts containing the three y values.
	  * @param p  - Pointer to the array of doubles for storing the three polynomial parameters.
	  * @return The maximum/minimum of the polynomial.
	  */
	static double calculateP2(const short &x0, unsigned short *y, double *p);

	/** Calculate the parameters for a third order polynomial which passes through 4 points.
	  * @param x0   - Initial x value. Sequential x values are assumed to be x0, x0+1, x0+2, and x0+3.
	  * @param y    - Pointer to the beginning of the array of unsigned shorts containing the four y values.
	  * @param p    - Pointer to the array of doubles for storing the three polynomial parameters.
	  * @param xmax - X value at the peak of the pulse.
	  * @return The local maximum/minimum of the polynomial.
	  */
	static double calculateP3(const short &x0, unsigned short *y, double *p, double &xmax);

  private:
	double cfdPar[7]; ///< Cfd fitting parameters.	

	double risetime; ///< Risetime of the single photon response function
	double falltime; ///< Decay time of the single photon response function
	double timeSpread; ///< Spread in the single photon response time offset based on the photo-electron transit time spread (in ns)
	double traceDelay; ///< Time offset of single photon response function (in ns)
	double gain; ///< Multiplicitive gain of the single photon response function

	double maximumTime; ///< The time at the trace maximum (in ns)
	double maximum; ///< The maximum value of the trace
	double baseline; ///< Baseline offset of the trace

	unsigned short maxIndex; ///< The index of the maximum trace bin (in ADC clock ticks)

	unsigned int adcBins; ///< Dynamic range of ADC in discrete channels

	size_t pulseLength; ///< Length of the raw and digitized ADC light pulses (in ADC clock ticks)
	
	bool isDigitized; ///< Flag indicating that the light response pulse has been digitized
	bool useSpectralResponse; ///< Flag indicating that PMT anode quantum efficiency spectrum should be used to compute anode gain
	bool pulseIsSaturated; ///< Flag indicating that the digitized pulse has exceeded the maximum ADC dynamic range
	
	double *rawPulse; ///< Array to store un-digitized raw light response pulse

	unsigned short *pulseArray; ///< Array to store digitized light response pulse

	spectralResponse spec; ///< Anode quantum efficiency

	double minimumArrivalTime; ///< Minimum photon arrival time

	int functionType; ///< Integer indicating the single photon response function to use to build the light response pulse

	std::vector<double> arrivalTimes; ///< Vector of all photon arrival times
	std::vector<double> photonWeights; ///< Vector of the gain of all photon detections
	
	double eval(const double &t, const double &dt=0);
	
	///< Find the pulse maximum.
	double findMaximum();
};

#endif
