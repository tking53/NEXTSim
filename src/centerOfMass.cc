#include <iostream>

#include "centerOfMass.hh"

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

void centerOfMass::clear(){
	totalMass = 0;
	center = G4ThreeVector();
}

void centerOfMass::addPoint(const G4ThreeVector &v, const double &mass/*=1*/){
	totalMass += mass;
	center += mass*v;
}

void centerOfMass::print(){
	if(!empty())
		std::cout << "M=" << totalMass << ", c=(" << getCenterX() << ", " << getCenterY() << ", " << getCenterZ() << ")\n";
	else
		std::cout << "M=0\n";
}
