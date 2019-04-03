
#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TGraph.h"
#include "TF1.h"

#include "Randomize.hh"

#include "pmtResponse.hh"

#define ADC_CLOCK_TICK 4 // ns

///////////////////////////////////////////////////////////////////////////////
// class spectralResponse
///////////////////////////////////////////////////////////////////////////////

/// Destructor.
spectralResponse::~spectralResponse(){
	this->close();
}

/// Load response function from a file.
bool spectralResponse::load(const char *fname){
	this->close();

	TFile *f = new TFile(fname, "READ");
	if(!f->IsOpen()) return false;
	TGraph *g1 = (TGraph*)f->Get("spec");
	if(!g1){
		f->Close();
		return false;
	}
	spectrum = (TGraph*)g1->Clone("spec");
	
	TF1 *f1 = (TF1*)f->Get("exlow");
	TF1 *f2 = (TF1*)f->Get("exhigh");
	
	if(f1) extrapolateLow = (TF1*)f1->Clone("ex1");
	if(f2) extrapolateHigh = (TF1*)f2->Clone("ex2");
	
	this->scanSpectrum();
	
	f->Close();
	return true;
}

/// Return the PMT quantum efficiency for a given wavelength.
double spectralResponse::eval(const double &wavelength){
	if(wavelength < xmin){ // Attempt to extrapolate toward smaller wavelengths.
		if(!extrapolateLow) return 0;
		return extrapolateLow->Eval(wavelength);
	}
	else if(wavelength <= xmax){ // Interpolate the distribution.
		return spectrum->Eval(wavelength);
	}
	else{ // Attempt to extrapolate toward larger wavelengths.
		if(!extrapolateHigh) return 0;
		return extrapolateHigh->Eval(wavelength);
	}
	return 0;
}

void spectralResponse::scanSpectrum(){
	double x, y;
	spectrum->GetPoint(0, x, y);
	xmin = x;
	spectrum->GetPoint(spectrum->GetN()-1, x, y);
	xmax = x;
}

void spectralResponse::close(){
	if(spectrum) 
		delete spectrum;
	if(extrapolateLow) 
		delete extrapolateLow;
	if(extrapolateHigh) 
		delete extrapolateHigh;
	spectrum = NULL;
	extrapolateLow = NULL;
	extrapolateHigh = NULL;
	xmin = 0;
	xmax = 0;
}

///////////////////////////////////////////////////////////////////////////////
// class pmtResponse
///////////////////////////////////////////////////////////////////////////////

pmtResponse::pmtResponse() : risetime(4.0), falltime(20.0), timeSpread(0), amplitude(0), peakOffset(0), peakMaximum(0), traceDelay(50), gain(1E4),
                             maximum(-9999), baseline(-9999), maxIndex(0), adcBins(4096), pulseLength(100), isDigitized(false), useSpectralResponse(false), 
                             pulseIsSaturated(false), rawPulse(NULL), pulseArray(NULL), spec(), minimumArrivalTime(0) {
	this->update();
	this->setPulseLength(pulseLength);
}

pmtResponse::pmtResponse(const double &risetime_, const double &falltime_) : risetime(risetime_), falltime(falltime_), timeSpread(0), amplitude(0), peakOffset(0), peakMaximum(0), traceDelay(50), gain(1E4),
                                                                             maximum(-9999), baseline(-9999), maxIndex(0), adcBins(4096), pulseLength(100), isDigitized(false), useSpectralResponse(false), 
                                                                             pulseIsSaturated(false), rawPulse(NULL), pulseArray(NULL), spec(), minimumArrivalTime(0) {
	this->update();
	this->setPulseLength(pulseLength);
}

pmtResponse::~pmtResponse(){
	if(pulseLength > 0){
		delete[] rawPulse;
		delete[] pulseArray;
	}
}

/// Return the gain-weighted arrival time of the photon pulse.
double pmtResponse::getWeightedPhotonArrivalTime() const {
	double weightedAverage = 0;
	double totalWeight = 0;
	std::vector<double>::const_iterator iter1, iter2;
	for(iter1 = arrivalTimes.begin(), iter2 = photonWeights.begin(); iter1 != arrivalTimes.end() && iter2 != photonWeights.end(); iter1++, iter2++){
		weightedAverage += (*iter2)*(*iter1);
		totalWeight += (*iter2);
	}
	return weightedAverage/totalWeight;
}

void pmtResponse::setRisetime(const double &risetime_){ 
	risetime = risetime_; 
	update();
}

void pmtResponse::setFalltime(const double &falltime_){ 
	falltime = falltime_; 
	update();
}

/// Set the length of the pulse in ADC bins.
void pmtResponse::setPulseLength(const size_t &len){
	if(pulseLength > 0){
		delete[] rawPulse;
		delete[] pulseArray;
	}
	pulseLength = len;
	rawPulse = new double[len];
	pulseArray = new unsigned short[len];
	this->clear();
}

/// Set the length of the pulse in nanoseconds.
void pmtResponse::setPulseLengthInNanoSeconds(const double &length){
	size_t len = (size_t)ceil(length/ADC_CLOCK_TICK);
	this->setPulseLength(len);
}

/// Set the dynamic bit range of the ADC.
void pmtResponse::setBitRange(const size_t &len){
	adcBins = 1;
	for(size_t i = 0; i < len; i++){
		adcBins *= 2;
	}
}

/// Load PMT spectral response from root file.
bool pmtResponse::loadSpectralResponse(const char *fname){
	return (useSpectralResponse = spec.load(fname));
}

void pmtResponse::addPhoton(const double &arrival, const double &wavelength/*=0*/, const double &gain_/*=1*/){
	double efficiency = 1;
	if(useSpectralResponse){ // Compute the quantum efficiency of the PMT for this wavelength.
		efficiency = spec.eval(wavelength)/100;
	}

	arrivalTimes.push_back(arrival);
	photonWeights.push_back(gain_);

	if(arrival < minimumArrivalTime)
		minimumArrivalTime = arrival;

	//double dt = arrival - peakOffset + traceDelay; // Arrival time is the peak of the pulse.
	double dt = arrival + traceDelay; // Arrival time is the leading edge of the pulse.
	if(timeSpread > 0){ // Smear the time offset based on the photo-electron transit time spread.
		dt += (G4UniformRand()-0.5)*timeSpread;
	}
	if(dt < 0) dt = 0;
	double dy;
	double time = dt;
	size_t index = (size_t)floor(dt/ADC_CLOCK_TICK);
	time += ADC_CLOCK_TICK/2;
	while(index < pulseLength){
		dy = gain_*efficiency*eval(time, dt);
		rawPulse[index] += dy;
		time += ADC_CLOCK_TICK;
		index++;
	}
}

void pmtResponse::digitize(const double &baseline_/*=0*/, const double &jitter_/*=0*/){
	if(isDigitized) return;
	pulseIsSaturated = false;
	for(size_t i = 0; i < pulseLength; i++){
		unsigned int value;
		if(rawPulse[i] == 0)
			value = 0;
		else{
			unsigned int bin = (unsigned int)floor(rawPulse[i]);
			if(bin >= adcBins) bin = adcBins-1;
			value = bin;
		}
		value += baseline_*adcBins;
		if(jitter_ != 0) 
			value += (-jitter_ + 2*G4UniformRand()*jitter_)*adcBins;
			
		if(value <= 65535) // Pulse is not saturated.
			pulseArray[i] = (unsigned short)value;
		else{ // Pulse is saturated.
			pulseIsSaturated = true;
			pulseArray[i] = 65535;
		}
	}
	isDigitized = true;
}

/// Integrate the baseline corrected trace for QDC in the range [start_, stop_] and return the result.
double pmtResponse::integratePulse(const size_t &start_/*=0*/, const size_t &stop_/*=0*/){
	if(pulseLength == 0 || !pulseArray) return -9999;
	size_t stop = (stop_ == 0?pulseLength:stop_);

	// Check for start index greater than stop index.
	if(start_+1 >= stop) return -9999;

	double qdc = 0.0;
	for(size_t i = start_+1; i < stop; i++){ // Integrate using trapezoidal rule.
		qdc += 0.5*(pulseArray[i-1] + pulseArray[i]) - baseline;
	}

	return qdc;
}

/// Integrate the baseline corrected trace for QDC in the range [maxIndex-start_, maxIndex+stop_] and return the result.
double pmtResponse::integratePulseFromMaximum(const short &start_/*=5*/, const short &stop_/*=10*/){
	if(maximum <= 0 && findMaximum() <= 0) return -9999;
	size_t low = (maxIndex > start_ ? maxIndex-start_ : 0);
	return this->integratePulse(low, maxIndex+stop_);
}

/// Perform traditional CFD analysis on the waveform.
double pmtResponse::analyzeCFD(const double &F_/*=0.5*/, const size_t &D_/*=1*/, const size_t &L_/*=1*/){
	if(pulseLength == 0 || !pulseArray) return -9999;
	double cfdMinimum = 9999;
	size_t cfdMinIndex = 0;
	
	double phase = -9999;
	double *cfdvals = new double[pulseLength];

	// Compute the cfd waveform.
	for(size_t cfdIndex = 0; cfdIndex < pulseLength; ++cfdIndex){
		cfdvals[cfdIndex] = 0.0;
		if(cfdIndex >= L_ + D_ - 1){
			for(size_t i = 0; i < L_; i++)
				cfdvals[cfdIndex] += F_ * (pulseArray[cfdIndex - i]-baseline) - (pulseArray[cfdIndex - i - D_]-baseline);
		}
		if(cfdvals[cfdIndex] < cfdMinimum){
			cfdMinimum = cfdvals[cfdIndex];
			cfdMinIndex = cfdIndex;
		}
	}

	// Find the zero-crossing.
	if(cfdMinIndex > 0){
		// Find the zero-crossing.
		for(size_t cfdIndex = cfdMinIndex-1; cfdIndex > 0; cfdIndex--){
			if(cfdvals[cfdIndex] >= 0.0 && cfdvals[cfdIndex+1] < 0.0){
				phase = cfdIndex - cfdvals[cfdIndex]/(cfdvals[cfdIndex+1]-cfdvals[cfdIndex]);
				break;
			}
		}
	}
	
	delete[] cfdvals;

	return phase*ADC_CLOCK_TICK-traceDelay;
}


/// Perform polynomial CFD analysis on the waveform.
double pmtResponse::analyzePolyCFD(const double &F_/*=0.5*/){
	if(pulseLength == 0 || !pulseArray) return -9999;
	if(maximum <= 0 && findMaximum() <= 0) return -9999;

	double threshold = F_*maximum + baseline;

	double phase = -9999;
	for(unsigned short cfdIndex = maxIndex; cfdIndex > 0; cfdIndex--){
		if(pulseArray[cfdIndex-1] < threshold && pulseArray[cfdIndex] >= threshold){
			calculateP2(cfdIndex-1, &pulseArray[cfdIndex-1], &cfdPar[4]);
			
			// Calculate the phase of the trace.
			if(cfdPar[6] != 0)
				phase = (-cfdPar[5]+std::sqrt(cfdPar[5]*cfdPar[5] - 4*cfdPar[6]*(cfdPar[4] - threshold)))/(2*cfdPar[6]);
			else
				phase = (threshold-cfdPar[4])/cfdPar[5];

			break;
		}
	}

	return phase*ADC_CLOCK_TICK-traceDelay;
}

void pmtResponse::copyTrace(unsigned short *arr, const size_t &len){
	if(!isDigitized) this->digitize();
	size_t stop = (len <= pulseLength ? len : pulseLength);
	for(size_t i = 0; i < stop; i++){
		arr[i] = pulseArray[i];
	}
}

/// Copy the digitized trace into a vector.
void pmtResponse::copyTrace(std::vector<unsigned short> &vec){
	vec.clear();
	vec.reserve(pulseLength);
	for(size_t i = 0; i < pulseLength; i++){
		vec.push_back(pulseArray[i]);
	}
}

void pmtResponse::clear(){
	for(size_t i = 0; i < 7; i++){
		cfdPar[i] = 0;
	}

	maximumTime = -9999;
	maximum = -9999;
	baseline = -9999;

	minimumArrivalTime = 1E6;

	arrivalTimes.clear();
	photonWeights.clear();

	maxIndex = 0;
	
	isDigitized = false;
	
	for(size_t i = 0; i < pulseLength; i++){
		rawPulse[i] = 0;
		pulseArray[i] = 0;
	}
}

void pmtResponse::print(){
	std::cout << " pmtResponse-\n";
	std::cout << "  risetime    " << risetime << " ns" << std::endl;
	std::cout << "  falltime    " << falltime << " ns" <<  std::endl;
	std::cout << "  amplitude   " << amplitude << std::endl;
	std::cout << "  peakOffset  " << peakOffset << " ns" <<  std::endl;
	std::cout << "  peakMaximum " << peakMaximum << std::endl;
	std::cout << "  gain        " << gain << std::endl;
}

void pmtResponse::printRaw(){
	for(size_t i = 0; i < pulseLength; i++){
		std::cout << i << "\t" << rawPulse[i] << std::endl;
	}
}

/** Calculate the parameters for a second order polynomial which passes through 3 points.
  * \param[in]  x0 - Initial x value. Sequential x values are assumed to be x0, x0+1, and x0+2.
  * \param[in]  y  - Pointer to the beginning of the array of unsigned shorts containing the three y values.
  * \param[out] p  - Pointer to the array of doubles for storing the three polynomial parameters.
  * \return The maximum/minimum of the polynomial.
  */
double pmtResponse::calculateP2(const short &x0, unsigned short *y, double *p){
	double x1[3], x2[3];
	for(size_t i = 0; i < 3; i++){
		x1[i] = (x0+i);
		x2[i] = std::pow(x0+i, 2);
	}

	double denom = (x1[1]*x2[2]-x2[1]*x1[2]) - x1[0]*(x2[2]-x2[1]*1) + x2[0]*(x1[2]-x1[1]*1);

	p[0] = (y[0]*(x1[1]*x2[2]-x2[1]*x1[2]) - x1[0]*(y[1]*x2[2]-x2[1]*y[2]) + x2[0]*(y[1]*x1[2]-x1[1]*y[2]))/denom;
	p[1] = ((y[1]*x2[2]-x2[1]*y[2]) - y[0]*(x2[2]-x2[1]*1) + x2[0]*(y[2]-y[1]*1))/denom;
	p[2] = ((x1[1]*y[2]-y[1]*x1[2]) - x1[0]*(y[2]-y[1]*1) + y[0]*(x1[2]-x1[1]*1))/denom;
	
	// Calculate the maximum of the polynomial.
	return (p[0] - p[1]*p[1]/(4*p[2]));
}

/** Calculate the parameters for a third order polynomial which passes through 4 points.
  * \param[in]  x0 - Initial x value. Sequential x values are assumed to be x0, x0+1, x0+2, and x0+3.
  * \param[in]  y  - Pointer to the beginning of the array of unsigned shorts containing the four y values.
  * \param[out] p  - Pointer to the array of doubles for storing the three polynomial parameters.
  * \return The local maximum/minimum of the polynomial.
  */
double pmtResponse::calculateP3(const short &x, unsigned short *y, double *p, double &xmax){
	double x1[4], x2[4], x3[4];
	for(size_t i = 0; i < 4; i++){
		x1[i] = (x+i);
		x2[i] = std::pow(x+i, 2);
		x3[i] = std::pow(x+i, 3);
	}

	double denom = (x1[1]*(x2[2]*x3[3]-x2[3]*x3[2]) - x1[2]*(x2[1]*x3[3]-x2[3]*x3[1]) + x1[3]*(x2[1]*x3[2]-x2[2]*x3[1])) - (x1[0]*(x2[2]*x3[3]-x2[3]*x3[2]) - x1[2]*(x2[0]*x3[3]-x2[3]*x3[0]) + x1[3]*(x2[0]*x3[2]-x2[2]*x3[0])) + (x1[0]*(x2[1]*x3[3]-x2[3]*x3[1]) - x1[1]*(x2[0]*x3[3]-x2[3]*x3[0]) + x1[3]*(x2[0]*x3[1]-x2[1]*x3[0])) - (x1[0]*(x2[1]*x3[2]-x2[2]*x3[1]) - x1[1]*(x2[0]*x3[2]-x2[2]*x3[0]) + x1[2]*(x2[0]*x3[1]-x2[1]*x3[0]));

	p[0] = (y[0]*(x1[1]*(x2[2]*x3[3]-x2[3]*x3[2]) - x1[2]*(x2[1]*x3[3]-x2[3]*x3[1]) + x1[3]*(x2[1]*x3[2]-x2[2]*x3[1])) - y[1]*(x1[0]*(x2[2]*x3[3]-x2[3]*x3[2]) - x1[2]*(x2[0]*x3[3]-x2[3]*x3[0]) + x1[3]*(x2[0]*x3[2]-x2[2]*x3[0])) + y[2]*(x1[0]*(x2[1]*x3[3]-x2[3]*x3[1]) - x1[1]*(x2[0]*x3[3]-x2[3]*x3[0]) + x1[3]*(x2[0]*x3[1]-x2[1]*x3[0])) - y[3]*(x1[0]*(x2[1]*x3[2]-x2[2]*x3[1]) - x1[1]*(x2[0]*x3[2]-x2[2]*x3[0]) + x1[2]*(x2[0]*x3[1]-x2[1]*x3[0]))) / denom;
	p[1] = ((y[1]*(x2[2]*x3[3]-x2[3]*x3[2]) - y[2]*(x2[1]*x3[3]-x2[3]*x3[1]) + y[3]*(x2[1]*x3[2]-x2[2]*x3[1])) - (y[0]*(x2[2]*x3[3]-x2[3]*x3[2]) - y[2]*(x2[0]*x3[3]-x2[3]*x3[0]) + y[3]*(x2[0]*x3[2]-x2[2]*x3[0])) + (y[0]*(x2[1]*x3[3]-x2[3]*x3[1]) - y[1]*(x2[0]*x3[3]-x2[3]*x3[0]) + y[3]*(x2[0]*x3[1]-x2[1]*x3[0])) - (y[0]*(x2[1]*x3[2]-x2[2]*x3[1]) - y[1]*(x2[0]*x3[2]-x2[2]*x3[0]) + y[2]*(x2[0]*x3[1]-x2[1]*x3[0]))) / denom;
	p[2] = ((x1[1]*(y[2]*x3[3]-y[3]*x3[2]) - x1[2]*(y[1]*x3[3]-y[3]*x3[1]) + x1[3]*(y[1]*x3[2]-y[2]*x3[1])) - (x1[0]*(y[2]*x3[3]-y[3]*x3[2]) - x1[2]*(y[0]*x3[3]-y[3]*x3[0]) + x1[3]*(y[0]*x3[2]-y[2]*x3[0])) + (x1[0]*(y[1]*x3[3]-y[3]*x3[1]) - x1[1]*(y[0]*x3[3]-y[3]*x3[0]) + x1[3]*(y[0]*x3[1]-y[1]*x3[0])) - (x1[0]*(y[1]*x3[2]-y[2]*x3[1]) - x1[1]*(y[0]*x3[2]-y[2]*x3[0]) + x1[2]*(y[0]*x3[1]-y[1]*x3[0]))) / denom;
	p[3] = ((x1[1]*(x2[2]*y[3]-x2[3]*y[2]) - x1[2]*(x2[1]*y[3]-x2[3]*y[1]) + x1[3]*(x2[1]*y[2]-x2[2]*y[1])) - (x1[0]*(x2[2]*y[3]-x2[3]*y[2]) - x1[2]*(x2[0]*y[3]-x2[3]*y[0]) + x1[3]*(x2[0]*y[2]-x2[2]*y[0])) + (x1[0]*(x2[1]*y[3]-x2[3]*y[1]) - x1[1]*(x2[0]*y[3]-x2[3]*y[0]) + x1[3]*(x2[0]*y[1]-x2[1]*y[0])) - (x1[0]*(x2[1]*y[2]-x2[2]*y[1]) - x1[1]*(x2[0]*y[2]-x2[2]*y[0]) + x1[2]*(x2[0]*y[1]-x2[1]*y[0]))) / denom;

	if(p[3] == 0){
		// Handle the case of p[3] == 0.
		return (p[0] - p[1]*p[1]/(4*p[2]));
	}

	// Calculate the maximum of the polynomial.
	double xmax1 = (-2*p[2]+std::sqrt(4*p[2]*p[2]-12*p[3]*p[1]))/(6*p[3]);
	double xmax2 = (-2*p[2]-std::sqrt(4*p[2]*p[2]-12*p[3]*p[1]))/(6*p[3]);

	if((2*p[2]+6*p[3]*xmax1) < 0) // The second derivative is negative (i.e. this is a maximum).
		xmax = xmax1;
	else
		xmax = xmax2;
	
	return (p[0] + p[1]*xmax + p[2]*xmax*xmax + p[3]*xmax*xmax*xmax);
}

void pmtResponse::update(){
	amplitude = 1/(falltime-risetime);
	peakOffset = std::log(risetime/falltime)/((1/falltime)-(1/risetime));
	peakMaximum = eval(peakOffset);
}

double pmtResponse::eval(const double &t, const double &dt/*=0*/){
	if(t-dt <= 0) return 0;
	return gain*amplitude*(std::exp(-(t-dt)/falltime)-std::exp(-(t-dt)/risetime));
}

double pmtResponse::findMaximum(){
	if(pulseLength == 0 || !pulseArray) return -9999;

	// Find the baseline.
	double tempbaseline = 0.0;
	size_t sample_size = floor(traceDelay/ADC_CLOCK_TICK);
	sample_size = (sample_size <= pulseLength ? 15:pulseLength);
	for(size_t i = 0; i < sample_size; i++){
		tempbaseline += pulseArray[i];
	}
	tempbaseline = tempbaseline/sample_size;
	baseline = double(tempbaseline);

	// Find the maximum ADC value and the maximum bin.
	unsigned short maxADC = 0;
	for(size_t i = 0; i < pulseLength; i++){
		if(pulseArray[i] - baseline > maxADC){ 
			maxADC = pulseArray[i] - baseline;
			maxIndex = i;
		}
	}

	// Find the pulse maximum by fitting with a third order polynomial.
	if(pulseArray[maxIndex-1] >= pulseArray[maxIndex+1]) // Favor the left side of the pulse.
		maximum = calculateP3(maxIndex-2, &pulseArray[maxIndex-2], cfdPar, maximumTime) - baseline;
	else // Favor the right side of the pulse.
		maximum = calculateP3(maxIndex-1, &pulseArray[maxIndex-1], cfdPar, maximumTime) - baseline;

	return maximum;
}
