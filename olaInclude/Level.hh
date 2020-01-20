/* 
 * File:   Level.h
 * Author: aleksandra
 *
 * Created on 5 maj 2012, 16:44
 */

#ifndef LEVEL_H
#define	LEVEL_H
#include <vector>
#include<string>
#include "Decay.hh"
#include "Beta.hh"
class Gamma;
class Level {
public:

    Level(double energy, double betaFeedingFunc, std::vector<Gamma*> &gammaEmitedFromLevel, int atomicNumber);
    Level(double energy, int atomicNumber);
    virtual ~Level();
    void AddGamma(Gamma* gamma);
    /**
     * Set half life of level
     * @param T12 string represent value and unit (F - fs, MS - ms, S - s, M - min, H - h, Y - year)
     */
    void SetHalfLifeTime(std::string T12);
    void SetBetaFeedingFunction (double betaFeedingFun) {betaFeedingFunction_ = betaFeedingFun;}
    void SetMaxBetaEnergy(double maxBetaEnergy) {maxBetaEnergy_ = maxBetaEnergy;}
		void SetBetaDecayProbability(double betaDecayProbability){betaDecayProbability_ = betaDecayProbability;}
		void SetBetaMinus();
		void SetBetaPlus(double bPlInt, double ecInt);
    
    double GetEnergy(){return energy_;}
    double GetBetaFeedingFunction(){return betaFeedingFunction_;}
    double GetMaxBetaEnergy(){return maxBetaEnergy_;}
    double GetT12() {return halfLifeTimeInSeconds_;}
		double GetBetaDecayProbability(){return betaDecayProbability_;}
		
		std::vector<Event> FindBetaEvents(){return beta_->FindBetaEvents();}
		Beta* GetBeta(){return beta_;}
		/**
			* Check level dacay -  IT or Beta
     */
		bool IsBetaDecayFromLevel();
		bool IsIsomerLevel();
    Gamma* RoundGammaEmitedFromLevel();
    double RoundTimeOfDecay();
private:
    double energy_; 
    double betaFeedingFunction_;
    std::vector<Gamma*> gammaEmitedFromLevel_;
    int atomicNumber_;
    
    double maxBetaEnergy_;
    double halfLifeTimeInSeconds_;
		double betaDecayProbability_;//w przypadku izomerow moze byc niezerowe

    std::vector<double> sumOfGammaIntensity_;
    
		Beta* beta_;
};

#endif	/* LEVEL_H */

