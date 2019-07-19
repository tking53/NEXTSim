#ifndef NDET_DETECTOR_LAYER_HH
#define NDET_DETECTOR_LAYER_HH

#include <string>

#include "globals.hh"

#include "gdmlSolid.hh"

class nDetDetector;

/** @class nDetDetectorLayer
  * @brief Component layers which are added to detector assemblies
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class nDetDetectorLayer{
  public:
	/** Default constructor
	  */
	nDetDetectorLayer() : argStr(), size(), nReqArgs(0), nUserArgs(0) { }

	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	nDetDetectorLayer(const G4String &arg_) : argStr(arg_), size(), nReqArgs(0), nUserArgs(0) { }

	/** Function pointer constructor
	  * @param arg_ Argument string from the input macro
	  * @param nargs_ The number of arguments which are required from the user input string
	  */	
	nDetDetectorLayer(const G4String &arg_, const int &nargs_) : argStr(arg_), size(), nReqArgs(nargs_), nUserArgs(0) { }

	/** Destructor
	  */
	virtual ~nDetDetectorLayer(){ }

	/** Get the size of the layer along the X-axis (in mm)
	  */
	G4double getSizeX() const { return size.getX(); }
	
	/** Get the size of the layer along the Y-axis (in mm)
	  */
	G4double getSizeY() const { return size.getY(); }

	/** Get the size of the layer along the Z-axis (in mm)
	  */
	G4double getSizeZ() const { return size.getZ(); }

	/** Return the argument string from the input macro
	  */
	G4String dump() const { return argStr; }

	/** Get the number of arguments expected from the user input string
	  */
	unsigned int getNumRequiredArgs() const { return nReqArgs; }

	/** Get the number of arguments passed to decodeString()
	  */
	unsigned int getNumSuppliedArgs() const { return nUserArgs; }

	/** Read a user input string and decode the relevant values
	  */
	virtual bool decodeString() = 0;
	
	/** Construct the layer volume and place it into a detector
	  */
	virtual void construct(nDetDetector *obj) = 0;

	/** Return a string containing proper input string syntax
	  */
	virtual std::string syntaxStr() const = 0;

  protected:
	G4String argStr; ///< Argument string from the macro supplied by the user

	G4ThreeVector size; ///< X, Y, and Z dimensions of the volume

	unsigned int nReqArgs; ///< The number of required user arguments which are expected
	unsigned int nUserArgs; ///< The number of user arguments passed to decodeString()
};

/** @class greaseLayer
  * @brief Optical grease component layer which is added to a detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class greaseLayer : public nDetDetectorLayer {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	greaseLayer(const G4String &arg_) : nDetDetectorLayer(arg_, 2), x(0), y(0), thickness(0) { }

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
	bool decodeString();

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

class diffuserLayer : public nDetDetectorLayer {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	diffuserLayer(const G4String &arg_) : nDetDetectorLayer(arg_, 3), x(0), y(0), thickness(0), matName("G4_SILICON_DIOXIDE") { }

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
	bool decodeString();

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
	G4String matName; ///< Name of the Geant NIST database material
};

/** @class lightGuideLayer
  * @brief Light-guide component layer which is added to a detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class lightGuideLayer : public nDetDetectorLayer {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	lightGuideLayer(const G4String &arg_) : nDetDetectorLayer(arg_, 5), x1(0), x2(0), y1(0), y2(0), thickness(0), matName("G4_SILICON_DIOXIDE") { }

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
	bool decodeString();

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
	G4String matName; ///< Name of the Geant NIST database material
};

/** @class gdmlLayer
  * @brief GDML model which is added to the world at a user-specified location
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class gdmlLayer : public nDetDetectorLayer {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	gdmlLayer(const G4String &arg_) : nDetDetectorLayer(arg_, 8) { }

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
	bool decodeString();

	/** Load the GDML model and place it into a detector
	  * @param obj A pointer to the detector where the model will be placed
	  */
	void construct(nDetDetector *obj);

	/** Return a string containing proper input string syntax
	  */
	std::string syntaxStr() const ;

  private:
	G4String filename; ///< Path to the input GDML file
	G4String material; ///< NIST database name of the material to use for the model
	G4ThreeVector position; ///< X, Y, and Z position of the center of the model (all in mm)
	G4ThreeVector rotation; ///< Rotation about the X, Y, and Z axes (all in degrees)

	gdmlSolid solid; ///< Solid model loaded from an external gdml file
};

/** @class gdmlLightGuideLayer
  * @brief GDML model component which is added to a detector assembly
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date July 18, 2019
  */

class gdmlLightGuideLayer : public nDetDetectorLayer {
  public:
	/** User input constructor
	  * @param arg_ Argument string from the input macro
	  */
	gdmlLightGuideLayer(const G4String &arg_) : nDetDetectorLayer(arg_, 5) { }

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
	bool decodeString();

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
	G4String material; ///< NIST database name of the material to use for the model
	G4ThreeVector rotation; ///< Rotation about the X, Y, and Z axes (all in degrees)
  
	gdmlSolid solid; ///< Solid model loaded from an external gdml file
};

#endif
