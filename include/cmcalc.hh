#ifndef CMCALC_HPP
#define CMCALC_HPP

#include "ParticleSource.hh"

class Particle{
  public:
	double v[2]; ///< Velocity of the particle for the first and second solution (in m/s)
	double E[2]; ///< Energy of the particle for the first and second solution (in MeV)
	double labAngle[2]; ///< Angle of the particle in the lab frame for the first and second solution (in radians)
	double comAngle[2]; ///< Angle of the particle in the center-of-mass frame for the first and second solution (in radians)

	bool inverseKin;
	
	Particle() : inverseKin(false) { }
	
	Particle(const double &thetaLab_, const double &Vcm_, const double &partVcm_, const double &mass_);
	
	bool calculate(const double &thetaLab_, const double &Vcm_, const double &partVcm_, const double &mass_);

	bool calculateCOM(const double &thetaCOM_, const double &Vcm_, const double &partVcm_, const double &mass_);
	
	std::string print(bool bothSolutions=true);
};

class Reaction : public Source {
  private:
	double Mbeam; ///< Mass of the beam particle reactant (in MeV/c^2)
	double Mtarg; ///< Mass of the target particle reactant (in MeV/c^2)
	double Mrecoil; ///< Mass of the recoil particle reaction product (in MeV/c^2)
	double Meject; ///< Mass of the ejectile particle reaction product (in MeV/c^2)
	double Vcm; ///< Center-of-mass velocity of the beam and target particles (in m/s)
	double recoilVcm; ///< Velocity of the recoil particle in the centr-of-mass frame (in m/s)
	double ejectVcm; ///< Velocity of the ejectile particle in the centr-of-mass frame (in m/s)
	double EXeject; ///< Excitation of the ejectile particle (in MeV)
	double EXrecoil; ///< Excitation of the recoil particle (in MeV)
	double Ebeam; ///< Energy of the beam particle (in MeV)
	double Ecm; ///< Energy of the reaction in the center-of-mass frame (in MeV)
	double Qgs; ///< Ground state Q-value of the reaction (in MeV)
	
	double recoilMaxAngle; ///< Maximum angle of the recoil particle in the lab frame (in degrees)
	double ejectMaxAngle; ///< Maximum angle of the ejectile particle in the lab frame (in degrees)
	double Qfinal; ///< Q-value of the final reaction state (in MeV)
	
	Particle recoilPart; ///< Recoil reaction product
	Particle ejectPart; ///< Ejectile reaction product
	
	void Calculate();
	
  public:
	Reaction() : Source(), Mbeam(0.0), Mtarg(0.0), Mrecoil(0.0), Meject(0.0), Vcm(0.0), recoilVcm(0.0), 
	             ejectVcm(0.0), EXeject(0.0), EXrecoil(0.0), Ebeam(0.0), Ecm(0.0), Qgs(0.0) {}

	bool Read(const char *fname_=NULL);

	Particle *GetRecoil(){ return &recoilPart; }
	
	Particle *GetEjectile(){ return &ejectPart; }

	double GetBeamMass(){ return Mbeam; }
	
	double GetTargetMass(){ return Mtarg; }
	
	double GetRecoilMass(){ return Mrecoil; }
	
	double GetEjectileMass(){ return Meject; }
	
	double GetRecoilExcitation(){ return EXrecoil; }
	
	double GetEjectileExcitation(){ return EXeject; }
	
	double GetBeamEnergy(){ return Ebeam; }
	
	double GetComEnergy(){ return Ecm; }
	
	double GetQgs(){ return Qgs; }
	
	double GetQfinal(){ return Qfinal; }
	
	double GetRecoilMaxAngle(){ return recoilMaxAngle; }
	
	double GetEjectileMaxAngle(){ return ejectMaxAngle; }
	
	double GetThresholdEnergy(){ return (-Qfinal*(Mbeam+Mtarg)/Mtarg); }
	
	bool IsAboveThreshold(){ return ((Ecm + Qfinal >= 0.0) ? true : false); }

	bool IsNormalKinematics(){ return ((Mtarg >= Mbeam ) ? true : false); }

	double SetBeam(const int &Z_, const int &A_, const double &BE_A_=0.0);

	double SetTarget(const int &Z_, const int &A_, const double &BE_A_=0.0);

	double SetRecoil(const int &Z_, const int &A_, const double &BE_A_=0.0);

	double SetEjectile(const int &Z_, const int &A_, const double &BE_A_=0.0);

	double SetRecoilEx(const double &ex_);

	double SetEjectileEx(const double &ex_);

	double SetEbeam(const double &E_);

	void SetLabAngle(const double &thetaLab_);
	
	void SetComAngle(const double &thetaCom_);
	
	void Print();
	
	virtual double sample(const double &theta);
};

#endif
