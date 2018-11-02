#include <iostream>

#include "centerOfMass.hh"

#define ADC_CLOCK_TICK 4 // ns

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

void centerOfMass::clear(){
	Npts = 0;
	tSum = 0;
	totalMass = 0;
	center = G4ThreeVector();
	t0 = std::numeric_limits<double>::max();	
	for(size_t i = 0; i < 100; i++)
		arrivalTimes[i] = 0;
}

void centerOfMass::addPoint(const G4ThreeVector &v, const double &time, const double &mass/*=1*/){
	totalMass += mass;
	center += mass*v;
	Npts++;
	
	tSum += time;
	if(time < t0) t0 = time;
	
	size_t index = (size_t)floor(time/ADC_CLOCK_TICK);
	if(index < 100)
		arrivalTimes[index]++;
}

void centerOfMass::print(){
	if(!empty()){
		std::cout << "M=" << totalMass << ", c=(" << getCenterX() << ", " << getCenterY() << ", " << getCenterZ() << ")\n";
		std::cout << " t0=" << t0 << ", tAvg=" << tSum/Npts << std::endl;
	}
}
