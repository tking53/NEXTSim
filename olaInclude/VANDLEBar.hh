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
// $Id: VANDLEBar.hh 17.10.2016 A Fijalkowska $
//
/// \file VANDLEBar.hh
/// \brief Definition of the VANDLEBar class
//
#ifndef VANDLEBar_H
#define VANDLEBar_H 1

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "MaterialsManager.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "PMTSD.hh"
#include "ScintillatorSD.hh"


class VANDLEBar
{
  public:

    VANDLEBar(G4int size);
    virtual ~VANDLEBar();

    virtual void ConstructSDandField();
    void Place(G4RotationMatrix *pRot, 
               const G4ThreeVector &tlate, 
               const G4String &pName, 
               G4LogicalVolume *pMotherLogical, 
               G4int pCopyNo);
   void Place(const G4Transform3D& transform3D, 
              const G4String &pName, 
              G4LogicalVolume *pMotherLogical, 
              G4int pCopyNo);
    G4LogicalVolume* GetModuleLogical() {return moduleLogic;}            
    G4LogicalVolume* GetScintilatorLogical() {return barLogic;}
    G4LogicalVolume* GetPMTLogical() {return photocathLogic;}
    
    G4double GetWidth() {return barWidth;}
	G4double GetHeight() {return barHeight;}
	G4double GetLength() {return barLength;}
	
	
  private:
  
	void SetBasicSizes();
	void SetSmallBarSizes();
	void SetMediumBarSizes();
	void SetLargeBarSizes();
	                     
	void MakeModuleLogic(G4double barX, 
	                     G4double barY, 
	                     G4double barZ, 
                         G4double wrappThickness, 
                         G4double PMTRadiusVal,
                         G4double PMTGlassThicknessVal, 
                         G4double PMTLen);
                                
                                
	void MakePlasticBar(G4double barX, G4double barY, G4double barZ);

	void MakeWrapping(G4double barX, 
	                  G4double barY, 
	                  G4double barZ, 
	                  G4double airThickness, 
	                  G4double wrappingThickness);
	                  
	void MakeWrapping(G4double barX, 
	                  G4double barY, 
                      G4double barZ, 
                      G4double wrappThick); 
                                      
    void MakeAirLayer(G4double barX, 
                      G4double barY, 
                      G4double barZ, 
                      G4double airThickness); 
    void MakePMTsGlass(G4double radius, 
                       G4double thickness, 
                       G4double barX, 
                       G4double barY);
    void MakePMTPhotocathode(G4double radius, 
                             G4double thickness, 
                             G4double barX, 
                             G4double barY);
    void PlacePMTPhotocathode(G4double glassThickness, 
                              G4double photocathThickness);        
    void PlacePMTGlass(G4double barZ, G4double thickness);
    
    void MakePMTShell(G4double thickness, G4double outerRad, G4double length);
    void PlacePMTShell(G4double barZ, G4double length, G4double foilThickness);
    
    void SetOpticalSurfacesProperties();
    
    MaterialsManager* materialsManager;
    
    G4double barWidth;
	G4double barHeight;
	G4double barLength;
	G4double reflectiveFoilThickness;
	G4double airThickness;

	G4double PMTRadius;
	G4double PMTActiveRadius;
	G4double PMTLength;
	G4double PMTGlassThickness;
	G4double PMTShellThickness;
	G4double PMTPhotocathodeThickness;
	
	G4LogicalVolume* moduleLogic;
	G4LogicalVolume* barLogic;
	G4LogicalVolume* PMTLogic;
	G4LogicalVolume* wrappingLogic;
	G4LogicalVolume* airLayerLogic;
	G4LogicalVolume* glassLogic;
	G4LogicalVolume* photocathLogic;

    static PMTSD* pmtSD;
    static ScintillatorSD* scintillatorSD;
    
};

#endif							 
