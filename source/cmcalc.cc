#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>

#include "Randomize.hh"

#include "cmcalc.hh"

const double c = 2.99792458E8; // m/s
const double cSquare = 8.9875517873E16; // m^2/s^2
const double pi = 3.141592654;
const double twoPi = 6.28318530718;

const double proton_RME = 938.272046; // MeV/c^2
const double neutron_RME = 939.565378; // MeV/c^2
const double mev2amu = 1.0/931.494061; // (amu*c^2)/MeV
const double mev2kg = 1.783E-30; // (kg*c^2)/MeV

Particle::Particle(const double &thetaLab_, const double &Vcm_, const double &partVcm_, const double &mass_){
	calculate(thetaLab_, Vcm_, partVcm_, mass_);
}

bool Particle::calculate(const double &thetaLab_, const double &Vcm_, const double &partVcm_, const double &mass_){
	labAngle[0] = thetaLab_;
	labAngle[1] = thetaLab_;

	if(partVcm_ < Vcm_ && (thetaLab_ > std::asin(partVcm_/Vcm_))){
		v[0] = -1; v[1] = -1;
		E[0] = -1; E[1] = -1;
		comAngle[0] = -1;
		comAngle[1] = -1;
	
		return false;
	}

	double sinTheta = std::sin(thetaLab_);
	double cosTheta = std::cos(thetaLab_);
	double tempArg = std::sqrt(partVcm_*partVcm_ - Vcm_*Vcm_*sinTheta*sinTheta);

	// Calculate the velocity for the first solution.
	v[0] = Vcm_ * cosTheta + tempArg;
	E[0] = 0.5 * mass_ * v[0]*v[0] / cSquare;
	comAngle[0] = std::atan2(sinTheta, cosTheta - Vcm_/v[0]);
	
	if(partVcm_ <= Vcm_){
		v[1] = Vcm_ * cosTheta - tempArg;
		E[1] = 0.5 * mass_ * v[1]*v[1] / cSquare;
		comAngle[1] = std::atan2(sinTheta, cosTheta - Vcm_/v[1]);
	}
	else{
		v[1] = -1;
		E[1] = -1;
		comAngle[1] = -1;
	}

	// Correct the CoM angle for inverse kinematics.
	if(inverseKin) {
		comAngle[0] = 180 - comAngle[0];
		if(comAngle[1] >= 0) comAngle[1] = 180 - comAngle[1];
	}
	
	return true;
}

bool Particle::calculateCOM(const double &thetaCOM_, const double &Vcm_, const double &partVcm_, const double &mass_){
	comAngle[0] = thetaCOM_;
	comAngle[1] = -1;

	double sinTheta = std::sin(thetaCOM_);
	double cosTheta = std::cos(thetaCOM_);
	double tempArg = 2*Vcm_*partVcm_*std::cos(thetaCOM_);

	// Calculate velocity and energy solutions.
	v[0] = std::sqrt(Vcm_*Vcm_ + partVcm_*partVcm_ + tempArg);
	v[1] = -1;
	E[0] = 0.5 * mass_ * v[0]*v[0] / cSquare;
	E[1] = -1;

	// Calculate the lab angles for each solution.
	labAngle[0] = std::abs(std::atan2(sinTheta, Vcm_/partVcm_ + cosTheta));
	labAngle[1] = -1;
	
	// Correct the CoM angle for inverse kinematics.
	if(inverseKin) comAngle[0] = 180 - comAngle[0];

	return true;
}

std::string Particle::print(bool bothSolutions/*=true*/){
	std::stringstream stream;
	if(v[0] >= 0.0) stream << comAngle[0] << "\t" << labAngle[0] << "\t" << E[0] << "\t" << v[0];
	else stream << "undefined\tundefined\tundefined\tundefined";
	if(bothSolutions){
		if(v[1] >= 0.0) stream << "\t" << comAngle[1] << "\t" << labAngle[1] << "\t" << E[1] << "\t" << v[1];
		else stream << "\tundefined\tundefined\tundefined\tundefined";
	}
	return stream.str();
}

void Reaction::Calculate(){
	// Must calculate these first. Other formulas rely on their values.
	Ecm = (Mtarg / (Mbeam + Mtarg))*Ebeam;
	Qgs = Mbeam + Mtarg - Meject - Mrecoil;
	Qfinal = (Qgs - EXrecoil - EXeject);
	
	// The following formulas rely on Ecm and Qgs.
	Vcm = (c / (Mbeam + Mtarg)) * std::sqrt(2 * Mbeam * Ebeam);
	recoilVcm = Vcm / std::sqrt((Mbeam / Mtarg) * (Mrecoil / Meject) * (Ecm / (Ecm + Qfinal)));
	ejectVcm = Vcm / std::sqrt((Mbeam / Mtarg) * (Meject / Mrecoil) * (Ecm / (Ecm + Qfinal)));
	
	recoilMaxAngle = recoilVcm > Vcm ? 180.0 : std::asin(recoilVcm/Vcm)*180/pi;
	ejectMaxAngle = ejectVcm > Vcm ? 180.0 : std::asin(ejectVcm/Vcm)*180/pi;
}

bool Reaction::Read(const char *fname_/*=NULL*/){
	std::string userInput;
	if(fname_ != NULL){
		std::ifstream configFile(fname_);
		if(!configFile.good()){
			std::cout << " Error: Failed to load input configuration file.\n";
			return false;
		}
		int Z, A;
		double BEA, state;
		configFile >> Z >> A >> BEA;
		SetBeam(Z, A, BEA);
		configFile >> Z >> A >> BEA;
		SetTarget(Z, A, BEA);
		configFile >> Z >> A >> BEA >> state;
		SetRecoil(Z, A, BEA);
		SetRecoilEx(state);
		configFile >> Z >> A >> BEA >> state;
		SetEjectile(Z, A, BEA);
		SetEjectileEx(state);
		configFile >> state;
		SetEbeam(state);
	}
	else{
		// Save the configuration file.
		bool saveToFile = false;
		std::ofstream outConfig;
		std::cout << " Manual reaction configuration mode.\n";
		std::cout << " Save when finished? (y/n) "; std::cin >> userInput;
		if(userInput == "y" || userInput == "Y" || userInput == "yes"){
			std::cout << " Enter config filename: "; std::cin >> userInput;
			outConfig.open(userInput.c_str());
			saveToFile = true;
		}
	
		int Z, A;
		double BEA, state;
		std::cout << " Enter beam Z, A, and BE/A: "; std::cin >> Z >> A >> BEA;
		if(saveToFile) outConfig << Z << "\t" << A << "\t" << BEA << "\n";
		SetBeam(Z, A, BEA);
		std::cout << " Enter target Z, A, and BE/A: "; std::cin >> Z >> A >> BEA;
		if(saveToFile) outConfig << Z << "\t" << A << "\t" << BEA << "\n";
		SetTarget(Z, A, BEA);
		std::cout << " Enter recoil Z, A, BE/A, and excitation: "; std::cin >> Z >> A >> BEA >> state;
		if(saveToFile) outConfig << Z << "\t" << A << "\t" << BEA << "\t" << state << "\n";
		SetRecoil(Z, A, BEA);
		SetRecoilEx(state);
		std::cout << " Enter ejectile Z, A, BE/A, and excitation: "; std::cin >> Z >> A >> BEA >> state;
		if(saveToFile) outConfig << Z << "\t" << A << "\t" << BEA << "\t" << state << "\n";
		SetEjectile(Z, A, BEA);
		SetEjectileEx(state);
		std::cout << " Enter beam energy: "; std::cin >> state;
		if(saveToFile) outConfig << state << "\n";
		SetEbeam(state);
		std::cout << std::endl;
		outConfig.close();
		std::cout << "  Manual setup complete!";
		if(saveToFile) 
			std::cout << " Wrote reaction information to file " << userInput;
		std::cout << std::endl;
	}

	return true;
}

double Reaction::SetBeam(const int &Z_, const int &A_, const double &BE_A_/*=0.0*/){
	Mbeam = Z_*proton_RME + (A_-Z_)*neutron_RME - BE_A_*A_;	
	if(Mtarg < Mbeam){
		ejectPart.inverseKin = true;
		recoilPart.inverseKin = true;
	}
	else{
		ejectPart.inverseKin = false;
		recoilPart.inverseKin = false;
	}
	return Mbeam;
}

double Reaction::SetTarget(const int &Z_, const int &A_, const double &BE_A_/*=0.0*/){
	Mtarg = Z_*proton_RME + (A_-Z_)*neutron_RME - BE_A_*A_;
	if(Mtarg < Mbeam){
		ejectPart.inverseKin = true;
		recoilPart.inverseKin = true;
	}
	else{
		ejectPart.inverseKin = false;
		recoilPart.inverseKin = false;
	}
	return Mtarg;
}

double Reaction::SetRecoil(const int &Z_, const int &A_, const double &BE_A_/*=0.0*/){
	return (Mrecoil = Z_*proton_RME + (A_-Z_)*neutron_RME - BE_A_*A_);
}

double Reaction::SetEjectile(const int &Z_, const int &A_, const double &BE_A_/*=0.0*/){
	return (Meject = Z_*proton_RME + (A_-Z_)*neutron_RME - BE_A_*A_);
}

double Reaction::SetRecoilEx(const double &ex_){
	return (EXrecoil = ex_);
}

double Reaction::SetEjectileEx(const double &ex_){
	return (EXeject = ex_);
}

double Reaction::SetEbeam(const double &E_){
	Ebeam = E_;
	Calculate();
	return Ebeam;
}

// Set the lab angle in radians.
void Reaction::SetLabAngle(const double &thetaLab_){
	recoilPart.calculate(thetaLab_, Vcm, recoilVcm, Mrecoil);
	ejectPart.calculate(thetaLab_, Vcm, ejectVcm, Meject);
}

// Set the center-of-mass angle in radians.
void Reaction::SetComAngle(const double &thetaCom_){
	recoilPart.calculateCOM(thetaCom_, Vcm, recoilVcm, Mrecoil);
	ejectPart.calculateCOM(thetaCom_, Vcm, ejectVcm, Meject);
}

void Reaction::Print(){
	Particle tempParticle1;
	Particle tempParticle2;

	std::cout << " Reaction Information:\n";
	std::cout << "  Beam Mass (MeV/c^2)       = " << Mbeam << std::endl;
	std::cout << "  Beam Energy (MeV)         = " << Ebeam << std::endl;
	std::cout << "  Beam Velocity (m/s)       = " << (c * std::sqrt(2 * Ebeam / Mbeam)) << std::endl;
	std::cout << "  Target Mass (MeV/c^2)     = " << Mtarg << std::endl;
	
	tempParticle1.calculateCOM(0, Vcm, recoilVcm, Mrecoil);
	tempParticle2.calculateCOM(180, Vcm, recoilVcm, Mrecoil);
	std::cout << "  Recoil Mass (MeV/c^2)     = " << Mrecoil << std::endl;
	std::cout << "  Recoil Excitation (MeV)   = " << EXrecoil << std::endl;
	std::cout << "  Recoil Max Angle (deg)    = " << recoilMaxAngle << std::endl;
	std::cout << "  Recoil Max Energy (MeV)   = " << std::max(std::max(tempParticle1.E[0], tempParticle1.E[1]), std::max(tempParticle2.E[0], tempParticle2.E[1])) << std::endl;
	std::cout << "  Recoil Min Energy (MeV)   = " << std::min(std::min(tempParticle1.E[0], tempParticle1.E[1]), std::min(tempParticle2.E[0], tempParticle2.E[1])) << std::endl;
	
	tempParticle1.calculateCOM(0, Vcm, ejectVcm, Meject);
	tempParticle2.calculateCOM(180, Vcm, ejectVcm, Meject);
	std::cout << "  Ejectile Mass (MeV/c^2)   = " << Meject << std::endl;
	std::cout << "  Ejectile Excitation (MeV) = " << EXeject << std::endl;
	std::cout << "  Ejectile Max Angle (deg)  = " << ejectMaxAngle << std::endl;
	std::cout << "  Ejectile Max Energy (MeV) = " << std::max(std::max(tempParticle1.E[0], tempParticle1.E[1]), std::max(tempParticle2.E[0], tempParticle2.E[1])) << std::endl;
	std::cout << "  Ejectile Min Energy (MeV) = " << std::min(std::min(tempParticle1.E[0], tempParticle1.E[1]), std::min(tempParticle2.E[0], tempParticle2.E[1])) << std::endl;
	
	std::cout << "\n  Threshold (MeV)            = " << (Ecm + Qgs) << std::endl;
	std::cout << "  Beam CM Velocity (m/s)     = " << Vcm << std::endl;
	std::cout << "  Recoil CM Velocity (m/s)   = " << recoilVcm << std::endl;
	std::cout << "  Ejectile CM Velocity (m/s) = " << ejectVcm << std::endl;
	std::cout << "  CM Energy (MeV)            = " << Ecm << std::endl;
	std::cout << "  G.S. Q-value (MeV)         = " << Qgs << std::endl;
	std::cout << "  Final Q-value (MeV)        = " << Qfinal << std::endl;
	
	if(Mtarg < Mbeam)
		std::cout << "\n  Inverse Kinematics\n";
	else 
		std::cout << "\n  Normal Kinematics\n";
	
	if(Ecm + Qfinal > 0.0) std::cout << "  Above Threshold\n";
	else std::cout << "  Below Threshold\n";
}

// Sample the ejectile energy distribution (in radians). 
double Reaction::sample(const double &theta){
	this->SetLabAngle(theta);
	if(ejectPart.v[1] >= 0.0) // Multiple solutions.
		return (G4UniformRand() >= 0.5 ? ejectPart.E[1] : ejectPart.E[0]);
	return ejectPart.E[0];
}
