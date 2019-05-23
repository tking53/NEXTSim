#ifndef CMCALC_HPP
#define CMCALC_HPP

#include "ParticleSource.hh"

/*! \class Particle
 *  \brief Class used to store information about a classical particle.
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 23, 2019
*/

class Particle{
  public:
	double v[2]; ///< Velocity of the particle for the first and second solution (in m/s)
	double E[2]; ///< Energy of the particle for the first and second solution (in MeV)
	double labAngle[2]; ///< Angle of the particle in the lab frame for the first and second solution (in radians)
	double comAngle[2]; ///< Angle of the particle in the center-of-mass frame for the first and second solution (in radians)

	bool inverseKin; ///< Flag indicating that this reaction is in inverse kinematics
	
	/** Default constructor
	  */
	Particle() : inverseKin(false) { }

	/** Destructor
	  */	
	~Particle(){ }

	/** Compute particle energies and center-of-mass angles for a given lab angle
	  * @param thetaLab_ The lab angle of the trajectory of the particle (in radians)
	  * @param Vcm_ The center-of-mass velocity of the system (in m/s)
	  * @param partVcm_ The velocity of the particle in the center-of-mass frame (in m/s)
	  * @param mass_ The mass of the particle (in MeV/c^2)
	  * @return True if the input lab angle is physical and return false otherwise
	  */	
	bool calculate(const double &thetaLab_, const double &Vcm_, const double &partVcm_, const double &mass_);

	/** Compute particle energies and lab angles for a given center-of-mass angle
	  * @param thetaLab_ The center-of-mass angle of the trajectory of the particle (in radians)
	  * @param Vcm_ The center-of-mass velocity of the system (in m/s)
	  * @param partVcm_ The velocity of the particle in the center-of-mass frame (in m/s)
	  * @param mass_ The mass of the particle (in MeV/c^2)
	  * @return True
	  */
	bool calculateCOM(const double &thetaCOM_, const double &Vcm_, const double &partVcm_, const double &mass_);
	
	/** Get a string containing kinematics information for the particle
	  * @param bothSolutions Flag indicating that both the first and second solutions should be added to the output
	  * @return A string containing the CoM angle, lab angle, energy, and velocity of the particle
	  */
	std::string print(bool bothSolutions=true);
};

/*! \class Reaction
 *  \brief Class used to simulate a classical two-body kinematics reaction.
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 23, 2019
*/

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
	
	/** Compute necessary energy and velocity variables
	  */
	void Calculate();
	
  public:
 	/** Default constructor
	  */
	Reaction() : Source(), Mbeam(0.0), Mtarg(0.0), Mrecoil(0.0), Meject(0.0), Vcm(0.0), recoilVcm(0.0), 
	             ejectVcm(0.0), EXeject(0.0), EXrecoil(0.0), Ebeam(0.0), Ecm(0.0), Qgs(0.0) { }

	/** Destructor
	  */
	~Reaction(){ }

	/** Read reaction parameters from an input file or stdin
	  * @param fname_ Path to ascii file containing reaction parameters. If NULL, the user is prompted for input via stdin
	  * @return True if parameters are set correctly and return false otherwise
	  */
	bool Read(const char *fname_=NULL);

	/** Get a pointer to the recoil particle
	  */
	Particle *GetRecoil(){ return &recoilPart; }

	/** Get a pointer to the ejectile particle
	  */
	Particle *GetEjectile(){ return &ejectPart; }

	/** Get the incoming beam particle mass (in MeV/c^2)
	  */
	double GetBeamMass(){ return Mbeam; }

	/** Get the incoming target particle mass (in MeV/c^2)
	  */
	double GetTargetMass(){ return Mtarg; }

	/** Get the outgoing recoil particle mass (in MeV/c^2)
	  */
	double GetRecoilMass(){ return Mrecoil; }

	/** Get the outgoing ejectile particle mass (in MeV/c^2)
	  */
	double GetEjectileMass(){ return Meject; }

	/** Get the excitation energy of the recoil particle (in MeV)
	  */
	double GetRecoilExcitation(){ return EXrecoil; }

	/** Get the excitation energy of the ejectile particle (in MeV)
	  */
	double GetEjectileExcitation(){ return EXeject; }

	/** Get the energy of the incoming beam particle (in MeV)
	  */
	double GetBeamEnergy(){ return Ebeam; }

	/** Get the center-of-mass energy of the reaction (in MeV)
	  */
	double GetComEnergy(){ return Ecm; }

	/** Get the ground state Q-value of the reaction (in MeV)
	  */
	double GetQgs(){ return Qgs; }

	/** Get the final state Q-value of the reaction (in MeV)
	  */
	double GetQfinal(){ return Qfinal; }

	/** Get the maximum angle of the recoil particle in the lab frame (in degrees)
	  */
	double GetRecoilMaxAngle(){ return recoilMaxAngle; }

	/** Get the maximum angle of the ejectile particle in the lab frame (in degrees)
	  */
	double GetEjectileMaxAngle(){ return ejectMaxAngle; }

	/** Get the beam energy threshold (in MeV)
	  * If the incoming beam particle energy is below this energy the reaction will not occur
	  */
	double GetThresholdEnergy(){ return (-Qfinal*(Mbeam+Mtarg)/Mtarg); }

	/** Check if the incoming beam particle energy is above threshold (in MeV)
	  * @return True if the beam energy is above the energy threshold and return false otherwise
	  */
	bool IsAboveThreshold(){ return ((Ecm + Qfinal >= 0.0) ? true : false); }

	/** Check if the reaction is in inverse kinematics i.e. the target is more massive than the beam
	  */
	bool IsNormalKinematics(){ return ((Mtarg >= Mbeam ) ? true : false); }

	/** Set the mass of the incoming beam particle
	  * @param Z_ Number of protons 
	  * @param A_ Total number of nucleons
	  * @param BE_A_ Binding energy per nucleon (in MeV)
	  * @return The mass of the beam particle (in MeV/c^2)
	  */
	double SetBeam(const int &Z_, const int &A_, const double &BE_A_=0.0);

	/** Set the mass of the incoming target particle
	  * @param Z_ Number of protons 
	  * @param A_ Total number of nucleons
	  * @param BE_A_ Binding energy per nucleon (in MeV)
	  * @return The mass of the target particle (in MeV/c^2)
	  */
	double SetTarget(const int &Z_, const int &A_, const double &BE_A_=0.0);

	/** Set the mass of the outgoing recoil particle
	  * @param Z_ Number of protons 
	  * @param A_ Total number of nucleons
	  * @param BE_A_ Binding energy per nucleon (in MeV)
	  * @return The mass of the recoil particle (in MeV/c^2)
	  */
	double SetRecoil(const int &Z_, const int &A_, const double &BE_A_=0.0);

	/** Set the mass of the outgoing ejectile particle
	  * @param Z_ Number of protons 
	  * @param A_ Total number of nucleons
	  * @param BE_A_ Binding energy per nucleon (in MeV)
	  * @return The mass of the ejectile particle (in MeV/c^2)
	  */
	double SetEjectile(const int &Z_, const int &A_, const double &BE_A_=0.0);

	/** Set the excitation energy of the recoil particle (in MeV)
	  */
	double SetRecoilEx(const double &ex_);

	/** Set the excitation energy of the ejectile particle (in MeV)
	  */
	double SetEjectileEx(const double &ex_);

	/** Set the incoming beam energy (in MeV)
	  */
	double SetEbeam(const double &E_);

	/** Set the lab angle of the recoil and ejectile (in radians) and update all energies
	  */
	void SetLabAngle(const double &thetaLab_);

	/** Set the center-of-mass angle between the recoil and ejectile (in radians) and update all energies
	  */
	void SetComAngle(const double &thetaCom_);

	/** Print reaction information to stdout
	  */
	void Print();
	
	/** Get the ejectile energy for a given lab angle
	  * @param theta The lab angle of outgoing ejectile particle (in radians)
	  * @return The energy of the ejectile particle (in MeV)
	  */
	virtual double sample(const double &theta);
};

#endif
