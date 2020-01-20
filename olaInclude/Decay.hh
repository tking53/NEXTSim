/* 
 * File:   Decay.h
 * Author: aleksandra
 *
 * Created on 1 maj 2012, 14:54
 * Class Decay represent all decay, with beta and gamma transitions. 
 * Method Execute() creates std::vector<Event> which is read by MTASPrimaryGeneratorAction
 */

#ifndef DECAY_H
#define	DECAY_H
#include "G4ParticleDefinition.hh"
#include <string>
#include <vector>
typedef std::vector<double> vecDouble;
class Level;
struct Event
{
	float  energy, time;
	G4ParticleDefinition* type;
	Event(float energyVal, float timeVal, G4ParticleDefinition* typeVal):
	energy(energyVal), time(timeVal), type(typeVal){}
};

class Decay
{
public:
    Decay(int atomicMass, int atomicNumber, double qVal, std::vector<Level*> levels, Level* parentLevel);
    Decay(const std::string& ensdfFilename);
    virtual ~Decay();
    std::vector<Event> Execute();
    Level* RoundPrimaryLevel();
private:

    int atomicMass_;
    int atomicNumber_;
    double qVal_;
    std::vector<Level*> levels_;
    Level* parentLevel_;
    std::vector<double> totalBetaFeedingFunction_;
    static Level* isomerLevel_;
	int nrOfIsLevelsHits;
		void SetBetaFeedingDistribution();
		std::vector<Event> BFeedingExecute();
		//std::vector<Event> NoBFeedingExecute();
};

#endif	/* DECAY_H */

