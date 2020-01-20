/* 
 * File:   Beta.h
 * Author: aleksandra
 *
 * Created on 5 maj 2012, 16:44
 * Class represent beta transition (beta plus, minus and EC). 
 * The most important method is FindBetaEvents() - it create std::vector<Event>
 * with random beta energy, time of decay and type of emitted particle
 */

#ifndef BETA_H
#define	BETA_H
#include <vector>
#include<string>
#include <fstream>
#include "G4DataVector.hh"
#include "G4ParticleDefinition.hh"
#include "FermiDistribution.hh"
#include "G4AtomicTransitionManager.hh"
struct Event;
class Beta {
public:

	Beta(double maxEnergy, int atomicNumber, int eCharge);
	virtual ~Beta();

	double GetMaxBetaEnergy(){return maxBetaEnergy_;}
	std::vector<Event> FindBetaEvents();
	void SetECCoef(std::string type, double value);
	void SetBetaPlusECInt(double betaPlusInt, double ECInt){
	if(maxBetaEnergy_ < 0)
		betaPlusInt_ =0;
	else
		betaPlusInt_ = betaPlusInt;
		ECInt_ = ECInt;
	}
private:
	void CatSpaces(std::string &s);
	int FindPrimaryVacancies();
	int FindRandomIndex( const G4DataVector transProb);
		
	double maxBetaEnergy_;
	int atomicNumber_;
	int eCharge_;
    
	double betaPlusInt_;
	double ECInt_;
	G4ParticleDefinition* decayType_;
	FermiDistribution *betaEnergyDistribution_;
	double ECCoeff_[4];
	bool ifECCoef_;
	G4AtomicTransitionManager* atomicTransitionManager_;
		//std::ofstream betyEn_;
};

#endif	/* Beta_H */

