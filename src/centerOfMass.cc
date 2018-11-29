#include <iostream>

#include "TFile.h"
#include "TGraph.h"

#include "G4Step.hh"
#include "Randomize.hh"

#include "centerOfMass.hh"

#define ADC_CLOCK_TICK 4 // ns

const double coeff = 1.23984193E-3; // hc = Mev * nm

///////////////////////////////////////////////////////////////////////////////
// class TraceProcessor
///////////////////////////////////////////////////////////////////////////////

/// Destructor.
TraceProcessor::~TraceProcessor(){ }

/// Integrate the baseline corrected trace for QDC in the range [start_, stop_] and return the result.
float TraceProcessor::IntegratePulse(unsigned short *pulse, const size_t &len, const size_t &start_/*=0*/, const size_t &stop_/*=0*/){
	if(len == 0 || !pulse) return -9999;
	size_t stop = (stop_ == 0?len:stop_);

	// Check for start index greater than stop index.
	if(start_+1 >= stop) return -9999;

	float qdc = 0.0;
	for(size_t i = start_+1; i < stop; i++){ // Integrate using trapezoidal rule.
		qdc += 0.5*(pulse[i-1] + pulse[i]) - baseline;
	}

	return qdc;
}

/// Integrate the baseline corrected trace for QDC in the range [start_, stop_] and return the result.
float TraceProcessor::IntegratePulse(const size_t &start_/*=0*/, const size_t &stop_/*=0*/){
	return this->IntegratePulse(pulseArray, pulseLength, start_, stop_);
}

/// Integrate the baseline corrected trace for QDC in the range [maxIndex-start_, maxIndex+stop_] and return the result.
float TraceProcessor::IntegratePulseFromMaximum(const short &start_/*=5*/, const short &stop_/*=10*/){
	if(maximum <= 0 && FindMaximum() <= 0) return -9999;
	size_t low = (maxIndex > start_ ? maxIndex-start_ : 0);
	return this->IntegratePulse(pulseArray, pulseLength, low, maxIndex+stop_);
}

/// Perform traditional CFD analysis on the waveform.
float TraceProcessor::AnalyzeCFD(unsigned short *pulse, const size_t &len, const float &F_/*=0.5*/, const size_t &D_/*=1*/, const size_t &L_/*=1*/){
	if(len == 0 || !pulse) return -9999;
	float cfdMinimum = 9999;
	size_t cfdMinIndex = 0;
	
	float phase = -9999;
	float *cfdvals = new float[len];

	// Compute the cfd waveform.
	for(size_t cfdIndex = 0; cfdIndex < len; ++cfdIndex){
		cfdvals[cfdIndex] = 0.0;
		if(cfdIndex >= L_ + D_ - 1){
			for(size_t i = 0; i < L_; i++)
				cfdvals[cfdIndex] += F_ * (pulse[cfdIndex - i]-baseline) - (pulse[cfdIndex - i - D_]-baseline);
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

	return phase*ADC_CLOCK_TICK;
}

/// Perform traditional CFD analysis on the waveform.
float TraceProcessor::AnalyzeCFD(const float &F_/*=0.5*/, const size_t &D_/*=1*/, const size_t &L_/*=1*/){
	return this->AnalyzeCFD(pulseArray, pulseLength, F_, D_, L_);
}

/// Perform polynomial CFD analysis on the waveform.
float TraceProcessor::AnalyzePolyCFD(unsigned short *pulse, const size_t &len, const float &F_/*=0.5*/){
	if(len == 0 || !pulse) return -9999;
	if(maximum <= 0 && FindMaximum() <= 0) return -9999;

	double threshold = F_*maximum + baseline;

	float phase = -9999;
	for(unsigned short cfdIndex = maxIndex; cfdIndex > 0; cfdIndex--){
		if(pulse[cfdIndex-1] < threshold && pulse[cfdIndex] >= threshold){
			calculateP2(cfdIndex-1, &pulse[cfdIndex-1], &cfdPar[4]);
			
			// Calculate the phase of the trace.
			if(cfdPar[6] != 0)
				phase = (-cfdPar[5]+std::sqrt(cfdPar[5]*cfdPar[5] - 4*cfdPar[6]*(cfdPar[4] - threshold)))/(2*cfdPar[6]);
			else
				phase = (threshold-cfdPar[4])/cfdPar[5];

			break;
		}
	}

	return phase*ADC_CLOCK_TICK;
}

/// Perform polynomial CFD analysis on the waveform.
float TraceProcessor::AnalyzePolyCFD(const float &F_/*=0.5*/){
	return this->AnalyzePolyCFD(pulseArray, pulseLength, F_);
}

float TraceProcessor::FindMaximum(){
	if(pulseLength == 0 || !pulseArray) return -9999;

	// Find the baseline.
	double tempbaseline = 0.0;
	size_t sample_size = (15 <= pulseLength ? 15:pulseLength);
	for(size_t i = 0; i < sample_size; i++){
		tempbaseline += pulseArray[i];
	}
	tempbaseline = tempbaseline/sample_size;
	baseline = float(tempbaseline);

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
		maximum = calculateP3(maxIndex-2, &pulseArray[maxIndex-2], cfdPar) - baseline;
	else // Favor the right side of the pulse.
		maximum = calculateP3(maxIndex-1, &pulseArray[maxIndex-1], cfdPar) - baseline;

	return maximum;
}

/** Calculate the parameters for a second order polynomial which passes through 3 points.
  * \param[in]  x0 - Initial x value. Sequential x values are assumed to be x0, x0+1, and x0+2.
  * \param[in]  y  - Pointer to the beginning of the array of unsigned shorts containing the three y values.
  * \param[out] p  - Pointer to the array of doubles for storing the three polynomial parameters.
  * \return The maximum/minimum of the polynomial.
  */
double TraceProcessor::calculateP2(const short &x0, unsigned short *y, double *p){
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
double TraceProcessor::calculateP3(const short &x, unsigned short *y, double *p){
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
		return (p[0] + p[1]*xmax1 + p[2]*xmax1*xmax1 + p[3]*xmax1*xmax1*xmax1);

	return (p[0] + p[1]*xmax2 + p[2]*xmax2*xmax2 + p[3]*xmax2*xmax2*xmax2);
}

///////////////////////////////////////////////////////////////////////////////
// class pmtResponse
///////////////////////////////////////////////////////////////////////////////

void pmtResponse::setRisetime(const double &risetime_){ 
	risetime = risetime_; 
	update();
}

void pmtResponse::setFalltime(const double &falltime_){ 
	falltime = falltime_; 
	update();
}

void pmtResponse::digitize(const double &arrival, double *arr, const size_t &len){
	//double dt = arrival - peakOffset + traceDelay; // Arrival time is the peak of the pulse.
	double dt = arrival + traceDelay; // Arrival time is the leading edge of the pulse.
	if(dt < 0) dt = 0;
	double dy;
	double time = dt;
	size_t index = (size_t)floor(dt/ADC_CLOCK_TICK);
	time += ADC_CLOCK_TICK/2;
	while(index < len){
		dy = eval(time, dt);
		arr[index] += dy;
		time += ADC_CLOCK_TICK;
		index++;
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

void pmtResponse::update(){
	amplitude = 1/(falltime-risetime);
	peakOffset = std::log(risetime/falltime)/((1/falltime)-(1/risetime));
	peakMaximum = eval(peakOffset);
}

double pmtResponse::eval(const double &t, const double &dt/*=0*/){
	if(t-dt <= 0) return 0;
	return gain*amplitude*(std::exp(-(t-dt)/falltime)-std::exp(-(t-dt)/risetime));
}

///////////////////////////////////////////////////////////////////////////////
// class centerOfMass
///////////////////////////////////////////////////////////////////////////////

centerOfMass::~centerOfMass(){
	if(spec) delete spec;
}

G4ThreeVector centerOfMass::getCenter() const {
	return (totalMass > 0 ? (1/totalMass)*center : center);
}

double centerOfMass::getCenterX() const {
	return (totalMass > 0 ? (1/totalMass)*center.getX() : 0);
}

double centerOfMass::getCenterY() const{
	return (totalMass > 0 ? (1/totalMass)*center.getY() : 0);
}

double centerOfMass::getCenterZ() const{
	return (totalMass > 0 ? (1/totalMass)*center.getZ() : 0);
}

void centerOfMass::getArrivalTimes(unsigned short *arr, const size_t &len, const double &baseline/*=0*/, const double &jitter/*=0*/) const {
	const unsigned int adcBins = 65536;
	size_t stop = (len <= 100 ? len : 100);
	for(size_t i = 0; i < stop; i++){
		if(arrivalTimes[i] == 0)
			arr[i] = 0;
		else{
			unsigned int bin = floorl(arrivalTimes[i]);
			if(bin >= adcBins) bin = adcBins-1;
			arr[i] = bin;
		}
		arr[i] += baseline*adcBins;
		if(jitter != 0) 
			arr[i] += (-jitter + 2*G4UniformRand()*jitter)*adcBins;
	}
}

short centerOfMass::setNumColumns(const short &col_){ 
	Ncol = col_;
	pixelWidth = activeWidth / Ncol;
	return Ncol; 
}

short centerOfMass::setNumRows(const short &row_){
	Nrow = row_;
	pixelHeight = activeHeight / Nrow;
	return Nrow; 
}

double centerOfMass::setActiveAreaWidth(const double &width_){
	activeWidth = width_;
	pixelWidth = activeWidth / Ncol;
	return activeWidth;
}

double centerOfMass::setActiveAreaHeight(const double &height_){
	activeHeight = height_;
	pixelHeight = activeHeight / Nrow;
	return activeHeight;
}

void centerOfMass::setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_){
	Ncol = col_;
	Nrow = row_;
	activeWidth = width_;
	activeHeight = height_;
	pixelWidth = activeWidth / Ncol;
	pixelHeight = activeHeight / Nrow;
}

bool centerOfMass::loadSpectralResponse(const char *fname, const char *name/*="spec"*/){
	TFile *f = new TFile(fname, "READ");
	if(!f->IsOpen()) return false;
	TGraph *g1 = (TGraph*)f->Get(name);
	if(!g1){
		f->Close();
		return false;
	}
	if(spec) delete spec;
	spec = (TGraph*)g1->Clone("spec");
	useSpectralResponse = true;
	f->Close();
	return true;
}

void centerOfMass::clear(){
	Npts = 0;
	NnotDetected = 0;
	tSum = 0;
	lambdaSum = 0;
	totalMass = 0;
	center = G4ThreeVector();
	t0 = std::numeric_limits<double>::max();	
	for(size_t i = 0; i < 100; i++)
		arrivalTimes[i] = 0;
}

bool centerOfMass::addPoint(const G4Step *step, const double &mass/*=1*/){
	double wavelength = coeff/step->GetTrack()->GetTotalEnergy(); // in nm

	if(useSpectralResponse && G4UniformRand() > spec->Eval(wavelength)/100){ // Not detected
		NnotDetected++;
		return false;
	}

	totalMass += mass;
	Npts++;
	
	if(Ncol < 0 && Nrow < 0){ // Default behavior
		center += mass*step->GetPostStepPoint()->GetPosition();	
	}
	else{ // Segmented PMT behavior
		G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
		pos.setX(((pos.getX()+activeWidth/2)/pixelWidth)*(activeWidth/Ncol)-activeWidth/2);
		pos.setY(((pos.getY()+activeHeight/2)/pixelHeight)*(activeHeight/Nrow)-activeHeight/2);
		center += mass*pos;
	}
	
	double time = step->GetPostStepPoint()->GetGlobalTime();
	
	tSum += time;
	lambdaSum += wavelength;
	if(time < t0) t0 = time;
	
	// Add the PMT response to the "digitized" trace
	response.digitize(time, arrivalTimes, 100);
	
	return true;
}

void centerOfMass::print(){
	if(!empty()){
		std::cout << "M=" << totalMass << ", c=(" << getCenterX() << ", " << getCenterY() << ", " << getCenterZ() << ")\n";
		std::cout << " t0=" << t0 << ", tAvg=" << tSum/Npts << std::endl;
	}
}
