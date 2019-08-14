#ifndef NDET_DETECTOR_LAYER_HH
#define NDET_DETECTOR_LAYER_HH

#include <string>

#include "globals.hh"

#include "gdmlSolid.hh"
#include "nDetWorldObject.hh"

class nDetDetector;

/** @class greaseLayer
  * @brief Optical grease component layer which is added to a detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class greaseLayer : public nDetWorldObject {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	greaseLayer(const G4String &arg_) : nDetWorldObject(arg_, 2), x(0), y(0), thickness(0) { }

	/** Destructor
	  */	
	~greaseLayer(){ }
	
	/** Apply a grease layer to the current detector assembly using dimensions from a space-delimited input string
	  * @note String syntax: <width> <height> [thickness]
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | width     | The width of the grease layer (in mm)
	  * | height    | The height of the grease layer (in mm)
	  * | thickness | The thickness of the grease layer (in mm). If not specified, @a fGreaseThickness is used
	  */
	bool decodeArgs();

	/** Construct the grease layer volume and place it into a detector
	  * @param obj A pointer to the detector where the grease layer will be placed
	  */
	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;

  private:
	G4double x; ///< Width of the diffuser (in mm)
	G4double y; ///< Height of the diffuser (in mm)
	G4double thickness; ///< Thickness of the diffuser (in mm)	
};

/** @class diffuserLayer
  * @brief Light diffuser component layer which is added to a detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class diffuserLayer : public nDetWorldObject {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	diffuserLayer(const G4String &arg_);

	/** Destructor
	  */	
	~diffuserLayer(){ }
	
	/** Apply a straight light diffuser layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note String syntax: <width> <height> <thickness> [material=G4_SILICON_DIOXIDE]
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | width     | The width of the diffuser layer (in mm)
	  * | height    | The height of the diffuser layer (in mm)
	  * | thickness | The thickness of the diffuser layer (in mm)
	  * | material  | Not used
	  */
	bool decodeArgs();

	/** Construct the diffuser layer volume and place it into a detector
	  * @param obj A pointer to the detector where the diffuser will be placed
	  */
	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;

  private:
	G4double x; ///< Width of the diffuser (in mm)
	G4double y; ///< Height of the diffuser (in mm)
	G4double thickness; ///< Thickness of the diffuser (in mm)
};

/** @class lightGuideLayer
  * @brief Light-guide component layer which is added to a detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class lightGuideLayer : public nDetWorldObject {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	lightGuideLayer(const G4String &arg_);

	/** Destructor
	  */
	lightGuideLayer(){ }

	/** Apply a trapezoidal light guide layer (quartz) to the current detector assembly using dimensions from a space-delimited input string
	  * @note String syntax: <width1> <width2> <height1> <height2> <thickness> [material=G4_SILICON_DIOXIDE]
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | width1    | The width of the large side of the trapezoid (in mm)
	  * | width2    | The width of the small side of the trapezoid (in mm)
	  * | height1   | The height of the large side of the trapezoid (in mm)
	  * | height2   | The height of the small side of the trapezoid (in mm)
	  * | thickness | The thickness of the light-guide layer (in mm)
	  * | material  | Not used
	  */
	bool decodeArgs();

	/** Construct the light-guide layer volume and place it into a detector
	  * @param obj A pointer to the detector where the light-guide will be placed
	  */
	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;

  private:
	G4double x1; ///< Width of the large side of the trapezoid (in mm)
	G4double x2; ///< Width of the small side of the trapezoid (in mm)
	G4double y1; ///< Height of the large side of the trapezoid (in mm)
	G4double y2; ///< Height of the small side of the trapezoid (in mm)
	G4double thickness; ///< Thickness of the trapezoid (in mm)
};

/** @class gdmlLayer
  * @brief GDML model which is added to the world at a user-specified location
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class gdmlLayer : public nDetWorldObject {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	gdmlLayer(const G4String &arg_) : nDetWorldObject(arg_, 8) { }

	/** Destructor
	  */	
	~gdmlLayer(){ }

	/** Load a GDML model from a file using parameters from a space-delimited input string and place it into the assembly
	  * @note String syntax: <filename> <posX> <posY> <posZ> <rotX> <rotY> <rotZ> <matString>
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | filename  | Filename of the input GDML file
	  * | posX(Y,Z) | X, Y, and Z position of the center of the model (in mm)
	  * | rotX(Y,Z) | Rotation about the X, Y, and Z axes (in degrees)
	  * | matString | The NIST database name of the material to use for the model
	  * @return A pointer to the gdmlSolid containing the model
	  */
	bool decodeArgs();

	/** Load the GDML model and place it into a detector
	  * @param obj A pointer to the detector where the model will be placed
	  */
	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;

  private:
	G4String filename; ///< Path to the input GDML file

	G4ThreeVector rotVector; ///< The rotation angles about the X, Y, and Z axes (all in degrees)

	gdmlSolid solid; ///< Solid model loaded from an external gdml file
};

/** @class gdmlLightGuideLayer
  * @brief GDML model component which is added to a detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class gdmlLightGuideLayer : public nDetWorldObject {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	gdmlLightGuideLayer(const G4String &arg_) : nDetWorldObject(arg_, 5) { }

	/** Destructor
	  */	
	~gdmlLightGuideLayer(){ }

	/** Load a light guide model from a file using parameters from a space-delimited input string and place it into the assembly
	  * @note String syntax: <filename> <rotX> <rotY> <rotZ> <matString>
	  * | Parameter | Description |
	  * |-----------|-------------|
	  * | filename  | Filename of the input GDML file
	  * | rotX(Y,Z) | Rotation about the X, Y, and Z axes (in degrees)
	  * | matString | The NIST database name of the material to use for the model
	  * @return A pointer to the gdmlSolid containing the model
	  */
	bool decodeArgs();

	/** Load the GDML light-guide model and place it into a detector
	  *
	  * This method performs the same as gdmlLayer::construct() except that it also defines logical border surfaces on all
	  * intersecting faces and automatically places the model into the assembly at the current layer position
	  *
	  * @param obj A pointer to the detector where the light-guide model will be placed
	  */
	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;
	
  private:
	G4String filename; ///< Path to the input GDML file
  
	G4ThreeVector rotVector; ///< The rotation angles about the X, Y, and Z axes (all in degrees)
  
	gdmlSolid solid; ///< Solid model loaded from an external gdml file
};

#endif
