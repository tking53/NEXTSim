/** @file nDetDetectorTypes.hh
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu
  * @date September 25, 2019
  * @brief Contains all pre-defined detector construction types
  */

#ifndef NDET_DETECTOR_TYPE_HH
#define NDET_DETECTOR_TYPE_HH

#include "G4String.hh"
#include "G4ThreeVector.hh"

#include "nDetDetector.hh"

class nDetConstruction;
class nDetMaterials;

namespace nDetDetectorTypes{

/** Get a pointer to a new detector
  * @param geom The name of the detector type
  * @return A pointer to the new nDetDetector instance
  */
nDetDetector* getDetectorType(const G4String &geom);

/** Get a pointer to a new detector
  * @param geom The name of the detector type
  * @param detector Pointer to the detector construction handler
  * @param matptr Pointer to the materials handler
  * @return A pointer to the new nDetDetector instance
  */
nDetDetector* getDetectorType(const G4String &geom, nDetConstruction *detector, nDetMaterials *matptr);

/** @class nextModuleType
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date September 25, 2019
  * @brief Segmented detector module
  */

class nextModuleType : public nDetDetector {
public:
	/** Default constructor
	  */
	nextModuleType() : nDetDetector() { }

	/** Detector constructor
	  * @param detector Pointer to a nDetConstruction object where the current detector is defined
	  * @param matptr Pointer to the Geant materials handler class which will be used for detector construction
	  */
	nextModuleType(nDetConstruction *detector, nDetMaterials *matptr) : nDetDetector(detector, matptr) { }

	/** Destructor
	  */
	~nextModuleType(){ }

	/** Prepare for the detector volume to be built. In this method, the user should set the maximum
	  * size constraints of the body of the detector so that the detector handler knows how large to
	  * make its bounding assembly volume.
	  */
	virtual void prepareToBuild();

	/** Build a segmented detector module
	  *
	  * The detector will have @a fNumColumns columns (horizontal) and @a fNumRows rows (vertical). If @a fWrappingThickness is
	  * greater than zero, the user selected wrapping material will be applied between all segments and around the outside
	  * of the detector.
	  */
	virtual void buildDetector();

protected:
};

/** @class ellipticalType
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date September 25, 2019
  * @brief Single-segment elliptical detector (football)
  */

class ellipticalType : public nDetDetector {
public:
	/** Default constructor
	  */
	ellipticalType() : nDetDetector() { }

	/** Detector constructor
	  * @param detector Pointer to a nDetConstruction object where the current detector is defined
	  * @param matptr Pointer to the Geant materials handler class which will be used for detector construction
	  */
	ellipticalType(nDetConstruction *detector, nDetMaterials *matptr) : nDetDetector(detector, matptr) { }

	/** Destructor
	  */
	~ellipticalType(){ }

	/** Prepare for the detector volume to be built. In this method, the user should set the maximum
	  * size constraints of the body of the detector so that the detector handler knows how large to
	  * make its bounding assembly volume.
	  */
	virtual void prepareToBuild();

	/** Build a single-segment elliptical detector (football)
	  * 
	  * The parameter @a fDetectorLength is used as the length of the central rectangular body while @a fTrapezoidLength is
	  * used for the length of the trapezoids on either side. The total detector length is equal to 2*fTrapezoidLength+fDetectorLength.
	  * The internal trapezoid angle is equal to @a fTrapezoidAngle
	  */	
	virtual void buildDetector();

protected:
};

/** @class rectangularType
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date September 25, 2019
  * @brief Single-segment rectangular detector
  */

class rectangularType : public nDetDetector {
public:
	/** Default constructor
	  */
	rectangularType() : nDetDetector() { }

	/** Detector constructor
	  * @param detector Pointer to a nDetConstruction object where the current detector is defined
	  * @param matptr Pointer to the Geant materials handler class which will be used for detector construction
	  */
	rectangularType(nDetConstruction *detector, nDetMaterials *matptr) : nDetDetector(detector, matptr) { }

	/** Destructor
	  */
	~rectangularType(){ }

	/** Prepare for the detector volume to be built. In this method, the user should set the maximum
	  * size constraints of the body of the detector so that the detector handler knows how large to
	  * make its bounding assembly volume.
	  */
	virtual void prepareToBuild();

	/** Build a single-segment rectangular detector
	  */
	virtual void buildDetector();

protected:
};

/** @class cylindricalType
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date September 25, 2019
  * @brief Single-segment cylindrical detectector
  */

class cylindricalType : public nDetDetector {
public:
	/** Default constructor
	  */
	cylindricalType() : nDetDetector() { }

	/** Detector constructor
	  * @param detector Pointer to a nDetConstruction object where the current detector is defined
	  * @param matptr Pointer to the Geant materials handler class which will be used for detector construction
	  */
	cylindricalType(nDetConstruction *detector, nDetMaterials *matptr) : nDetDetector(detector, matptr) { }

	/** Destructor
	  */
	~cylindricalType(){ }

	/** Prepare for the detector volume to be built. In this method, the user should set the maximum
	  * size constraints of the body of the detector so that the detector handler knows how large to
	  * make its bounding assembly volume.
	  */
	virtual void prepareToBuild();

	/** Build a single-segment cylindrical detectector
	  *
	  * The parameter @a fDetectorWidth is used as the diameter of the cylindrical body, @a fDetectorheight is not used
	  */
	virtual void buildDetector();

protected:
};

} // namespace nDetDetectorTypes

#endif
