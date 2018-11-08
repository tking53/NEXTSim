#include <iostream>

#include "TFile.h"
#include "TGraph.h"

#include "G4Step.hh"
#include "Randomize.hh"

#include "centerOfMass.hh"

#define ADC_CLOCK_TICK 4 // ns

const double coeff = 1.23984193E-3; // hc = Mev * nm

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

void centerOfMass::getArrivalTimes(unsigned short *arr, const size_t &len, const size_t &offset/*=0*/){
	size_t stop = (len <= 100 ? len : 100);
	for(size_t i = offset; i < stop; i++){
		arr[i] = arrivalTimes[i];
	}
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
	center += mass*step->GetPostStepPoint()->GetPosition();
	Npts++;
	
	double time = step->GetPostStepPoint()->GetGlobalTime();
	
	tSum += time;
	lambdaSum += wavelength;
	if(time < t0) t0 = time;
	
	size_t index = (size_t)floor(time/ADC_CLOCK_TICK);
	if(index < 100)
		arrivalTimes[index]++;

	return true;
}

void centerOfMass::print(){
	if(!empty()){
		std::cout << "M=" << totalMass << ", c=(" << getCenterX() << ", " << getCenterY() << ", " << getCenterZ() << ")\n";
		std::cout << " t0=" << t0 << ", tAvg=" << tSum/Npts << std::endl;
	}
}
