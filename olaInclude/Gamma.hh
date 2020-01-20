/* 
 * File:   Gamma.h
 * Author: aleksandra
 *
 * 
 */

#ifndef GAMMA_H
#define	GAMMA_H
#include "G4DataVector.hh"
#include "G4AtomicTransitionManager.hh"
#include "G4DataVector.hh"
#include <vector>
#include <string>

class Level;
struct Event;
class Gamma {
public:

    Gamma(int atomicNumber, double energy = 0, double gammaIntensity = 0, double cumulatedElectronConversionCoefficient = 0, Level* finalLevel = 0L );
    virtual ~Gamma();
    /**
     * Set electron conversion coeficients
     * @param type (KC, LC, MC and KC+, LC+, MC+)
     * @param value 
     */
    void SetShellElectronConvCoef(std::string type, double value);
    /**
     * Set final level after gamma emission
     * @param finalLevel
     */
    void SetFinalLevel(Level* finalLevel){finalLevel_ = finalLevel;}
    
    double GetEnergy(){return energy_;}
    double GetTotalIntensity(){return gammaIntensity_*(1+cumulatedElectronConversionCoefficient_);}
    Level* GetFinalLevel(){return finalLevel_;}
    
    std::vector<Event> FindGammaEvents();
    int FindPrimaryVacancies(int atomicNumber);
private:
	void InitializeShellNumbers();
	bool IsGammaDecay();
	void FindICEvent(std::vector<Event> &decay);
	int FindPrimaryShellIndex();
	bool IsRadiativeTransitionReachableShell(int shellIndex);
	bool IsAugerReachableShell(int shellIndex);
	void AddGammaEvent(std::vector<Event> &decay, double energy);
	void AddElectronEvent(std::vector<Event> &decay, double energy);
	void AddXRaysEvent(std::vector<Event> &decay, int primaryVacancies);
	void AddAugerEvent(std::vector<Event> &decay, int primaryVacancies);
	int FindRandomIndex( const G4DataVector transProb);

	void CatSpaces(std::string &s);
	
    int atomicNumber_;
    double energy_;
    double gammaIntensity_;
    double cumulatedElectronConversionCoefficient_;
    Level* finalLevel_;
    double cummulatedProbability_;

	static const int nuberOfShellIndexes_ = 4;
    double shellElectonConvCoeff_[nuberOfShellIndexes_];
	int shellNumbers_[nuberOfShellIndexes_]; 
    G4AtomicTransitionManager* atomicTransitionManager_;

    
};

#endif	/* GAMMA_H */

