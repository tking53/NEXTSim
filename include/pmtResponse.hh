#ifndef PMT_RESPONSE_HH
#define PMT_RESPONSE_HH

#include <vector>

class TGraph;
class TF1;

/*! \class spectralResponse
 *  \brief Class used to interpolate PMT anode quantum efficiency from an input spectrum.
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 23, 2019
*/

class spectralResponse{
  public:
	/** Default constructor
	  */
	spectralResponse() : spectrum(NULL), extrapolateLow(NULL), extrapolateHigh(NULL), xmin(0), xmax(0) { }

	/** Destructor
	  */
	~spectralResponse();

	/** Get the range of the quantum efficiency spectrum
	  * @param min_ The minimum wavelength defined in the spectrum (in nm)
	  * @param max_ The maximum wavelength defined in the spectrum (in nm)
	  */
	void getRange(double &min_, double &max_);

	/** Get a pointer to the quantum efficiency spectrum TGraph
	  */
	TGraph *getSpectrum(){ return spectrum; }

	/** Get a pointer to the TF1 used to extrapolate below the minimum wavelenth in the quantum efficiency spectrum
	  */	
	TF1 *getExtrapolateLow(){ return extrapolateLow; }

	/** Get a pointer to the TF1 used to extrapolate above the maximum wavelenth in the quantum efficiency spectrum
	  */	
	TF1 *getExtrapolateHigh(){ return extrapolateHigh; }

	/** Copy the quantum efficiency spectrum from another spectralResponse object
	  * @param other Pointer to another spectralResponse from which the spectrum will be copied
	  */
	void copy(spectralResponse *other);

	/** Load the quantum efficiency spectrum from a root file
	  * @param fname Path to a root file containing a TGraph named 'spec' and two TF1s named 'exlow' and 'exhigh'
	  * @return True if all objects are loaded successfully and return false otherwise
	  */
	bool load(const char *fname);

	/** Return the quantum efficiency for a given wavelength
	  * @param wavelength Optical photon wavelength (in nm)
	  * @return 
	  */
	double eval(const double &wavelength);

  private:
	TGraph *spectrum; ///< Pointer to a TGraph containing the PMT quantum efficiency spectrum (i.e. percentage vs. photon wavelength in nm)
	
	TF1 *extrapolateLow; ///< Pointer to a TF1 used to extrapolate below the minimum wavelenth in the quantum efficiency spectrum
	TF1 *extrapolateHigh; ///< Pointer to a TF1 used to extrapolate above the maximum wavelenth in the quantum efficiency spectrum
	
	double xmin; ///< Minimum optical photon wavelenth defined in the quantum efficiency spectrum (in nm)
	double xmax; ///< Maximum optical photon wavelenth defined in the quantum efficiency spectrum (in nm)

	/** Scan the spectrum and set the minimum and maximum defined optical photon wavelengths
	  */	
	void scanSpectrum();

	/** Delete the spectrum and clear all values
	  */	
	void close();
};

/*! \class pmtResponse
 *  \brief Class used to simulate the light response due to detection of multiple optical photons.
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 23, 2019
*/

class pmtResponse{
  public:
	/** Default constructor
	  */
	pmtResponse();
	
	/** Pulse constructor
	  * @param risetime_ The rise time of the single photon response function (in ns)
	  * @param falltime_ The decay time of the single photon response function (in ns)
	  */
	pmtResponse(const double &risetime_, const double &falltime_);

	/** Destructor
	  */
	~pmtResponse();

	/** Get the length of the light pulse (in ADC clock ticks)
	  */
	size_t getPulseLength() const { return pulseLength; }

	/** Get the maximum value of the pulse (in ADC channels)
	  */
	double getMaximum() const { return maximum; }

	/** Get the baseline of the pulse (in ADC channels)
	  */
	double getBaseline() const { return baseline; }

	/** Get the pulse phase at the maximum (in ns)
	  */
	double getMaximumTime() const { return maximumTime; }

	/** Get a pointer to the array of Poly CFD parameters
	  * @return Pointer to an array of 7 doubles 
	  */
	double *getCfdParams() { return cfdPar; }

	/** Get the maximum ADC index of the pulse (in ADC clock ticks)
	  */
	unsigned short getMaximumIndex() const { return maxIndex; }

	/** Get a pointer to the raw light pulse array
	  */
	double *getRawPulse(){ return rawPulse; }
	
	/** Get a pointer to the digitized pulse array
	  */
	unsigned short *getDigitizedPulse(){ return pulseArray; }

	/** Return true if the digitized pulse is saturated and return false otherwise
	  */
	bool getPulseIsSaturated() const { return pulseIsSaturated; }

	/** Get the gain-weighted arrival time of the photon pulse (in ns)
	  */
	double getWeightedPhotonArrivalTime() const ;

	/** Get the minimum photon arrival time of the pulse (in ns)
	  */
	double getMinimumPhotonArrivalTime() const { return minimumArrivalTime; }

	/** Get a pointer to the PMT quantum efficiency spectrum
	  */
	spectralResponse *getSpectralResponse(){ return &spec; }

	/** Set the rise time of the single photon pulse
	  * @param risetime_ The rise time of the single photon response function (in ns)
	  */
	void setRisetime(const double &risetime_);
	
	/** Set the decay time of the single photon pulse
	  * @param falltime_ The decaytime of the single photon response function (in ns)
	  */
	void setFalltime(const double &falltime_);

	/** Set the FWHM spread in the photo-electron transit time
	  * @param spread_ The uncertainty spread in the arrival time of the single photon response (in ns)
	  */
	void setTransitTimeSpread(const double &spread_){ timeSpread = spread_; }

	/** Set the time delay of the pulse
	  * @param traceDelay_ The offset in the arrival time of the single photon response (in ns)
	  */
	void setTraceDelay(const double &traceDelay_){ traceDelay = traceDelay_; }

	/** Set the gain
	  * @param gain_ The multiplicitive gain of the single photon response
	  */
	void setGain(const double &gain_){ gain = gain_; }

	/** Set the length of the pulse
	  * @param len_ The length of the light response pulse (in ADC clock ticks)
	  */
	void setPulseLength(const size_t &len);

	/** Set the length of the pulse 
	  * @param length The length of the light response pulse (in ns)
	  */
	void setPulseLengthInNanoSeconds(const double &length);

	/** Set the dynamic bit range of the ADC
	  * @param len_ The number of bits of dynamic range the ADC has
	  */
	void setBitRange(const size_t &len);

	/** Set the single photon response function type
	  * @param type The single photon response function to use [0=double exponential, 1=vandle function]
	  */
	void setFunctionType(const int &type){ functionType = type; }

	/** Disable use of PMT quantum efficiency spectrum
	  */
	void disableSpectralResponse(){ useSpectralResponse = false; }

	/** Load PMT spectral response from root file
	  * @param fname Path to a root file containing the anode quantum efficiency spectrum
	  * @return True if the spectrum is loaded successfully and return false otherwise
	  */
	bool loadSpectralResponse(const char *fname);

	/** Copy quantum efficiency spectrum from another pmtResponse object
	  * @param spec_ Pointer to a spectralResponse from which the spectrum will be copied
	  */
	void copySpectralResponse(spectralResponse *spec_){ spec.copy(spec_); }

	/** Add a photon signal to the raw pulse
	  * @param arrival Arrival time of the optical photon (in ns)
	  * @param wavelength Wavelength of the optical photon (in nm)
	  * @param gain_ Additional multiplicitive gain to use for single photon response
	  */
	void addPhoton(const double &arrival, const double &wavelength=0, const double &gain_=1);

	/** Digitize the raw light pulse
	  * @param baseline The light pulse baseline as a fraction of the full ADC dynamic range
	  * @param jitter The random jitter in the light pulse baseline as a fraction of the full ADC dynamic range
	  */
	void digitize(const double &baseline=0, const double &jitter=0);

	/** Integrate the light pulse in a range and return the result
	  * @param start_ Inclusive start index for integration (in ADC clock ticks)
	  * @param stop_ Inclusive stop index for integration (in ADC clock ticks)
	  * @return The integral of the baseline corrected light pulse in the range [start_, stop_]
	  */
	double integratePulse(const size_t &start_=0, const size_t &stop_=0);

	/** Integrate the light pulse in a range about the maximum and return the result
	  * @param start_ The number of ADC clock ticks to the left of the light pulse max index to begin integration
	  * @param stop_ The number of ADC clock ticks to the right of the light pulse max index to stop integration
	  * @return The integral of the baseline corrected light pulse in the range [maxIndex-start_, maxIndex+stop_]
	  */
	double integratePulseFromMaximum(const short &start_=5, const short &stop_=10);

	/** Perform traditional CFD analysis on the waveform using <a href="https://www.xia.com/Papers/TimeRes_DigConstFracDiscrimination.pdf">XIA CFD Algorithm</a>
	  * @param F_ The CFD crossing point as a function of the pulse height
	  * @param D_ CFD delay length (in ADC clock ticks)
	  * @param L_ Running average length (in ADC clock ticks)
	  * @return The phase of the pulse, i.e. the CFD crossing point (in ns)
	  */
	double analyzeCFD(const double &F_=0.5, const size_t &D_=1, const size_t &L_=1);
	
	/** Perform polynomial CFD analysis (PolyCFD) on the waveform
	  * @param F_ The PolyCFD crossing point as a function of the pulse height
	  * @return The phase of the pulse, i.e. the PolyCFD crossing point (in ns)
	  */
	double analyzePolyCFD(const double &F_=0.5);

	/** Copy the digitized trace into an array
	  * @param arr The array of unsigned shorts into which the digitized trace will be copied
	  * @param len The length of the array pointed at by arr
	  */
	void copyTrace(unsigned short *arr, const size_t &len);

	/** Copy the digitized trace into a vector
	  * @param vec The vector of unsigned shorts into which the digitized trace will be copied
	  */
	void copyTrace(std::vector<unsigned short> &vec);

	/** Clear the ADC trace and all analysis variables
	  */
	void clear();

	/** Print pulse parameters to stdout
	  */
	void print();

	/** Print the raw pulse to stdout
	  */
	void printRaw();

	/** Get the period of the ADC clock (in ns)
	  */
	static int getAdcClockTick();

	/** Calculate the parameters for a second order polynomial which passes through 3 points.
	  * @param x0 - Initial x value. Sequential x values are assumed to be x0, x0+1, and x0+2.
	  * @param y  - Pointer to the beginning of the array of unsigned shorts containing the three y values.
	  * @param p  - Pointer to the array of doubles for storing the three polynomial parameters.
	  * @return The maximum/minimum of the polynomial
	  */
	static double calculateP2(const short &x0, unsigned short *y, double *p);

	/** Calculate the parameters for a third order polynomial which passes through 4 points.
	  * @param x0   - Initial x value. Sequential x values are assumed to be x0, x0+1, x0+2, and x0+3.
	  * @param y    - Pointer to the beginning of the array of unsigned shorts containing the four y values.
	  * @param p    - Pointer to the array of doubles for storing the three polynomial parameters.
	  * @param xmax - X value at the peak of the pulse.
	  * @return The local maximum/minimum of the polynomial
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
	
	/** Compute the baseline and maximum of the light pulse
	  * @return The maximum of the light pulse if the array is properly initialized and return -9999 otherwise
	  */
	double findMaximum();
};

#endif
