//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: MaterialsManager.hh 21.10.2016 A Fijalkowska $
//
/// \file MaterialsManager.hh
/// \brief Definition of the MaterialsManager class. This class is singleton
///  Every materials and its properties should to be defined here.
//
#ifndef MaterialsManager_H
#define MaterialsManager_H 1

#include "G4Material.hh"


class MaterialsManager //MaterialsFactory??
{
  public:
	MaterialsManager* GetMaterialsManager();
	G4Material* GetVaccum();
    G4Material* GetAir();
    G4Material* GetBC408();
    G4Material* GetBC408NoLight();
    G4Material* GetTin();
    G4Material* GetBorosilicate();
    G4Material* GetAluminum();
    G4Material* GetNaI();
    G4Material* GetBialkali();
    G4Material* GetGermanium();
    G4Material* GetIron();
    G4Material* GetStainSteel();
    G4Material* GetConcrete();
    G4Material* GetHDPE();
    G4Material* GetLiqNitrogen();
    
  private:
	MaterialsManager();
	static MaterialsManager* s_instance;
	void CleanUp();
	bool useOptical;
	
	G4double atomicMass;
    G4double z;
    G4double density;
    G4int numberElements;
    G4double massFraction;
  
	G4Element* H;
	G4Element* C;
	G4Element* N;
	G4Element* O;
	G4Element* Na;
	G4Element* Al;
	G4Element* Si;
	G4Element* S;
	G4Element* K;
	G4Element* Fe;
	G4Element* Ni;
	G4Element* Ag;
	G4Element* Sn;
	G4Element* Sb;
	G4Element* I;
	G4Element* Cs;
	G4Element* Pb;
	G4Element* B;
	G4Element* Mn;
	G4Element* Cr;
	G4Element* Mg;
	G4Element* Ca;
  	
	G4Material* vaccum;
	G4Material* air;
	G4Material* BC408;
	G4Material* BC408NoLight;
	G4Material* tin;
	G4Material* borosilicate;
	G4Material* aluminum;
	G4Material* NaI;
	G4Material* bialkali;
	G4Material* germanium;
	G4Material* iron;
	G4Material* stainSteel;
	G4Material* concrete;
	G4Material* HDPE;
	G4Material* liqNitrogen;
	
  public:
	static MaterialsManager *GetInstance()
	{
		if (!s_instance)
			s_instance = new MaterialsManager;
		return s_instance;
	}
};

#endif



	
