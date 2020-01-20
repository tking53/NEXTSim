/* 
 * File:   LoadDecayData.h
 * Author: aleksandra
 *
 * Created on 3 czerwiec 2012, 12:27
 * Class reads ensdf file with decay scheme
 * In the file is a lot of never udes data. Class doesn't read that, all such lines are commented
 */

#ifndef LOADDECAYDATA_H
#define	LOADDECAYDATA_H
#include "Level.hh"
#include "Gamma.hh"
#include <vector>
#include <string>

class LoadDecayData {
public:
    LoadDecayData(const std::string filename);
    //LoadDecayData(const LoadDecayData& orig);
    ~LoadDecayData();
    std::vector<Level*> GetLevels(){return levels_;}
    int GetAtomicMass() {return atomicMass_;}
    int GetAtomicNumber() {return atomicNumber_;}
    double GetQVal() {return qVal_;}
    Level* GetParentLevel() {return parentLevel_;}
    
private:
    Level* FindFinalLevel(Gamma* gamma, Level* parentLevel, std::vector<Level*> &levels);
    std::vector<Level*> levels_;
    Level* parentLevel_;
    int atomicMass_;
    int atomicNumber_;
    double qVal_;

};

#endif	/* LOADDECAYDATA_H */

