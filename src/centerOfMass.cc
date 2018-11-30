#include <iostream>

#include "G4Step.hh"

#include "centerOfMass.hh"

const double coeff = 1.23984193E-3; // hc = Mev * nm

///////////////////////////////////////////////////////////////////////////////
// class centerOfMass
///////////////////////////////////////////////////////////////////////////////

centerOfMass::~centerOfMass(){
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

bool centerOfMass::loadSpectralResponse(const char *fname){
	return response.loadSpectralResponse(fname);
}

void centerOfMass::clear(){
	Npts = 0;
	NnotDetected = 0;
	tSum = 0;
	lambdaSum = 0;
	totalMass = 0;
	center = G4ThreeVector();
	t0 = std::numeric_limits<double>::max();	
	response.clear();
}

bool centerOfMass::addPoint(const G4Step *step, const double &mass/*=1*/){
	double wavelength = coeff/step->GetTrack()->GetTotalEnergy(); // in nm
	double time = step->GetPostStepPoint()->GetGlobalTime(); // in ns

	// Add the PMT response to the "digitized" trace
	response.addPhoton(time, wavelength);

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
	
	tSum += time;
	lambdaSum += wavelength;
	if(time < t0) t0 = time;
	
	return true;
}

void centerOfMass::print(){
	if(!empty()){
		std::cout << "M=" << totalMass << ", c=(" << getCenterX() << ", " << getCenterY() << ", " << getCenterZ() << ")\n";
		std::cout << " t0=" << t0 << ", tAvg=" << tSum/Npts << std::endl;
	}
}
