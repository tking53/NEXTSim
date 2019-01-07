#include <iostream>
#include <fstream>

#include "G4Step.hh"

#include "centerOfMass.hh"
#include "vertilon.hh"

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

double centerOfMass::getCenterY() const {
	return (totalMass > 0 ? (1/totalMass)*center.getY() : 0);
}

double centerOfMass::getCenterZ() const {
	return (totalMass > 0 ? (1/totalMass)*center.getZ() : 0);
}

bool centerOfMass::getCenterSegment(G4ThreeVector &pos, short &col, short &row) const {
	double xpos = (pos.getX()+activeWidth/2)/pixelWidth;
	double ypos = (pos.getY()+activeHeight/2)/pixelHeight;
	
	pos.setX(xpos*(activeWidth/Ncol)-activeWidth/2);
	pos.setY(ypos*(activeHeight/Nrow)-activeHeight/2);
	
	col = (short)floor(xpos);
	row = (short)floor(ypos);
	
	return ((col >= 0 && col < Ncol) && (row >= 0 && row < Nrow));
}

bool centerOfMass::getCenterSegment(short &col, short &row) const {
	G4ThreeVector pos = this->getCenter();
	return this->getCenterSegment(pos, col, row);
}	

void centerOfMass::getAnodeCurrents(double *array) const {
	for(size_t i = 0; i < 4; i++){
		array[i] = anodeCurrent[i];
	}
}

double centerOfMass::getReconstructedX() const {
	return ((anodeCurrent[0]+anodeCurrent[1])-(anodeCurrent[2]+anodeCurrent[3]))/(anodeCurrent[0]+anodeCurrent[1]+anodeCurrent[2]+anodeCurrent[3]);
}

double centerOfMass::getReconstructedY() const {
	return ((anodeCurrent[1]+anodeCurrent[2])-(anodeCurrent[3]+anodeCurrent[0]))/(anodeCurrent[0]+anodeCurrent[1]+anodeCurrent[2]+anodeCurrent[3]);
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
	
	// Setup the anode gain matrix.
	gainMatrix.clear();
	for(short i = 0; i < Ncol; i++){
		gainMatrix.push_back(std::vector<double>(Nrow, 1));
	}
}

bool centerOfMass::loadSpectralResponse(const char *fname){
	return response.loadSpectralResponse(fname);
}

bool centerOfMass::loadGainMatrix(const char *fname){
	if(gainMatrix.empty() || Ncol*Nrow == 0) return false;
	std::ifstream gainFile(fname);
	if(!gainFile.good()) return false;
	
	double readval;
	for(short col = 0; col < Ncol; col++){
		for(short row = 0; row < Nrow; row++){
			gainFile >> readval;
			if(gainFile.eof()){
				gainFile.close();
				return false;
			}
			gainMatrix[col][row] = readval;
		}
	}
	
	gainFile.close();
	
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
	response.clear();
	for(size_t i = 0; i < 4; i++){
		anodeCurrent[i] = 0;
	}
}

bool centerOfMass::addPoint(const G4Step *step, const double &mass/*=1*/){
	if(!step->GetPostStepPoint()){
		std::cout << " WARNING! INVALID POST POINT!\n";
		return false;
	}

	double wavelength = coeff/step->GetTrack()->GetTotalEnergy(); // in nm
	double time = step->GetPostStepPoint()->GetGlobalTime(); // in ns

	if(Ncol < 0 && Nrow < 0){ // Default behavior
		center += mass*step->GetPostStepPoint()->GetPosition();	
		
		// Add the PMT response to the "digitized" trace
		response.addPhoton(time, wavelength);
		
		// Add the "mass" to the others
		totalMass += mass;		
	}
	else{ // Segmented PMT behavior
		short xpos, ypos;
		G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
		if(this->getCenterSegment(pos, xpos, ypos)){
			// Get the gain of this anode.
			double gain = getGain(xpos, ypos);

			// Compute resistor network leakage current.
			const double leakage[3][3] = {{1E-3, 1E-2, 1E-3},
			                              {1E-2, 1.00, 1E-2},
			                              {1E-3, 1E-2, 1E-3}};
			for(short anodeX = -1; anodeX <= 1; anodeX++){
				for(short anodeY = -1; anodeY <= 1; anodeY++){
					double *current = getCurrent(xpos+anodeX, ypos+anodeY);
					if(current){ // Add the anger logic currents to the anode outputs.
						for(size_t i = 0; i < 4; i++){
							anodeCurrent[i] += gain*leakage[anodeX+1][anodeY+1]*current[3-i];
						}
					}
				}
			}
			
			// Add the PMT response to the "digitized" trace
			response.addPhoton(time, wavelength, gain);

			// Add the "mass" to the others weighted by the individual anode gain
			center += pos;
			totalMass += mass;
		}
	}
	
	tSum += time;
	lambdaSum += wavelength;
	if(time < t0) t0 = time;

	Npts++;
	
	return true;
}

void centerOfMass::print(){
	if(!empty()){
		std::cout << "M=" << totalMass << ", c=(" << getCenterX() << ", " << getCenterY() << ", " << getCenterZ() << ")\n";
		std::cout << " t0=" << t0 << ", tAvg=" << tSum/Npts << std::endl;
	}
}

double centerOfMass::getGain(const int &x, const int &y){
	if((x < 0 || x >= Ncol) || (y < 0 || y >= Nrow)) return 0;
	return gainMatrix[x][y]/100;
}

double *centerOfMass::getCurrent(const int &x, const int &y){
	if((x < 0 || x >= 8) || (y < 0 || y >= 8)) return NULL;
	return currents[x][y];
}
