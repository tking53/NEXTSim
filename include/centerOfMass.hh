#ifndef CENTER_OF_MASS_HPP
#define CENTER_OF_MASS_HPP

#include <limits>
#include <vector>

#include "G4ThreeVector.hh"

#include "pmtResponse.hh"

class G4Step;

/** @class centerOfMass
  * @brief Computes detected photon center-of-mass position in 3d space.
  * @author Cory R. Thornsberry (cthornsb@vols.utk.edu)
  * @date Nov 2, 2018
  * 
  * This class is used to compute the center-of-mass for photons which are detected
  * by an optical photon sensitive surface. The center-of-mass is computed by dividing
  * the sum of all detected photon positions (in 3d space) by the sum of all of their
  * "weights".
  */

class centerOfMass{
  public:
	/** Default constructor
	  */
	centerOfMass() : Ncol(-1), Nrow(-1), Npts(0), NnotDetected(0), totalMass(0), t0(std::numeric_limits<double>::max()), tSum(0), lambdaSum(0),
	                 activeWidth(0), activeHeight(0), pixelWidth(0), pixelHeight(0), center(0, 0, 0), response() { }

	/** Destructor
	  */
	~centerOfMass();

	/** Check whether there are detected photons
	  * @return True if there has been at least one detected photon and return false otherwise
	  */	
	bool empty() const { return (totalMass == 0); }
	
	/** Get the detected photon weight used to compute the weighted center-of-mass position
	  * @return The total weight of all detected optical photons
	  */	
	double getTotalMass() const { return totalMass; }
	
	/** Get the position of the photon center-of-mass for all detected photons
	  * @return The photon center-of-mass position (in mm)
	  */	
	G4ThreeVector getCenter() const;

	/** Get the position of the photon center-of-mass along the X-axis (in mm)
	  */
	double getCenterX() const;

	/** Get the position of the photon center-of-mass along the Y-axis (in mm)
	  */		
	double getCenterY() const;

	/** Get the position of the photon center-of-mass along the Z-axis (in mm)
	  */
	double getCenterZ() const;

	/** Get the anode column and row for a given position on the active surface
	  * @param pos Position on the active surface (in mm)
	  * @param col Anode column
	  * @param row Anode row
	  * @return True if the anode is within the active area and return false otherwise
	  */
	bool getCenterSegment(G4ThreeVector &pos, short &col, short &row) const ;

	/** Get the anode column and row for the photon center-of-mass
	  * @param col Anode column
	  * @param row Anode row
	  * @return True if the anode is within the active area and return false otherwise
	  */
	bool getCenterSegment(short &col, short &row) const ;

	/** Get the ratio of the number of detected photons to the total number
	  */
	double getDetectionEfficiency() const { return (Npts > 0 ? double(Npts)/(Npts+NnotDetected) : -1); }

	/** Get the average wavelength of all detected photons (in nm)
	  */
	double getAvgWavelength() const { return (Npts > 0 ? lambdaSum/Npts : -1); }

	/** Get the average arrival time of all detected photons (in ns)
	  */
	double getAvgArrivalTime() const { return (Npts > 0 ? tSum/Npts : -1); }

	/** Get the minimum detected photon arrival time (in ns)
	  */
	double getMinArrivalTime() const { return (Npts > 0 ? t0 : -1); }

	/** Get the total number of detected photons
	  */
	size_t getNumDetected() const { return Npts; }

	/** Get the number of photons not detected
	  */
	size_t getNumNotDetected() const { return NnotDetected; }

	/** Get the number anode columns for the segmented PMT
	  */
	short getNumColumns() const { return Ncol; }

	/** Get the number of anode rows for the segmented PMT
	  */
	short getNumRows() const { return Nrow; }

	/** Get the width of the optical photon sensitive surface (in mm)
	  */
	double getActiveAreaWidth() const { return activeWidth; }

	/** Get the height of the optical photon sensitive surface (in mm)
	  */
	double getActiveAreaHeight() const { return activeHeight; }
	
	/** Get a pointer to the dynode light pulse response
	  */
	pmtResponse *getPmtResponse(){ return &response; }
	
	/** Get a pointer to the array containing the four Anger Logic output responses
	  */
	pmtResponse *getAnodeResponse(){ return anodeResponse; }
	
	/** Get the four Anger Logic currents {V1, V2, V3, V4}
	  * @param array Array of at least 4 doubles
	  */
	void getAnodeCurrents(double *array) const ;
	
	/** Get the horizontal photon center-of-mass position computed from the Anger Logic currents
	  */
	double getReconstructedX() const ;
	
	/** Get the vertical photon center-of-mass position computed from the Anger Logic currents
	  */
	double getReconstructedY() const ;
	
	/** Get the anode gain matrix
	  */
	void getGainMatrix(std::vector<std::vector<double> > &matrix) const { matrix = gainMatrix; }
	
	/** Get the anode hit count matrix
	  */
	void getCountMatrix(std::vector<std::vector<int> > &matrix) const { matrix = countMatrix; }
	
	/** Set the number of anode columns
	  * @param col_ Number of anode columns
	  * @return The number of columns
	  */
	short setNumColumns(const short &col_);
	
	/** Se the number of anode rows
	  * @param row_ Number of anode rows
	  * @return The number of rows
	  */
	short setNumRows(const short &row_);

	/** Set the active width of the photo-sensitive surface
	  * @param width_ The width of the active photon detection area (in mm)
	  * @return The width of the optical photon sensitive surface (in mm)
	  */
	double setActiveAreaWidth(const double &width_);

	/** Set the active height of the photo-sensitive surface
	  * @param height_ The height of the active photon detection area (in mm)
	  * @return The height of the optical photon sensitive surface (in mm)
	  */	
	double setActiveAreaHeight(const double &height_);

	/** Set a segmented PMT
	  * @param col_ Number of anode columns
	  * @param row_ Number of anode rows
	  * @param width_ The width of the active photon detection area (in mm)
	  * @param height_ The height of the active photon detection area (in mm)
	  */	
	void setSegmentedPmt(const short &col_, const short &row_, const double &width_, const double &height_);
	
	/** Load the anode quantum efficiency spectrum from a root file
	  * @param fname Path to file containing anode quantum efficiency spectrum
	  * @return True if the quantum efficiency is loaded correctly and return false otherwise
	  */
	bool loadSpectralResponse(const char *fname);
	
	/** Load the anode gains from an ascii file
	  * @param fname Path to file containing a matrix of anode gain percentages
	  * @return True if the number of percentages read is greater than or equal to the number of anodes and return false otherwise
	  */	
	bool loadGainMatrix(const char *fname);

	/** Copy the anode quantum efficiency spectrum from another centerOfMass object
	  * @param other Pointer to a centerOfMass object from which to copy the anode quantum efficiency spectrum
	  */
	void copySpectralResponse(centerOfMass *other);

	/** Copy the anode gain matrix from another centerOfMass object
	  * @param other Pointer to a centerOfMass object from which to copy the matrix of anode gain percentages
	  */	
	void copyGainMatrix(centerOfMass *other);
	
	/** Clear all detected photons
	  */
	void clear();

	/** Add a photon to the center-of-mass distribution
	  * @param energy Energy of the photon (in MeV)
	  * @param time Time-of-arrival of the photon (in ns)
	  * @param position Position of the detection point (in mm)
	  * @param mass The weight of the detection used for the weighted average
	  * @return True
	  */	
	bool addPoint(const double &energy, const double &time, const G4ThreeVector &position, const double &mass=1);

	/** Print the number of detected photons for each anode to stdout
	  */
	void printCounts() const ;

	/** Print debugging information to stdout
	  */
	void print() const ;
  
  private:
	short Ncol; ///< Number of PMT anode columns
	short Nrow; ///< Number of PMT anode rows
	
	size_t Npts; ///< Number of photons added to the weighted average
	size_t NnotDetected; ///< Number of photons not detected by the sensitive surface
	
	double totalMass; ///< Total photon weight for the weighted average
	double t0; ///< Photon minimum time-of-arrival
	double tSum; ///< Sum of all photon arrival times
	double lambdaSum; ///< Sum of all photon wavelengths
	
	double activeWidth; ///< Width of the active anode area (in mm)
	double activeHeight; ///< Height of the active anode area (in mm)
	
	double pixelWidth; ///< Width of each individual PMT anode
	double pixelHeight; ///< Height of each individual PMT anode
	
	double anodeCurrent[4]; ///< Sum of the current at each Anger Logic readout
	
	G4ThreeVector center; ///< 3d sum of all photon detection points
	
	pmtResponse response; ///< Light pulse response of the dynode
	
	pmtResponse anodeResponse[4]; // Light pulse response of the four Anger Logic readouts
	
	std::vector<std::vector<double> > gainMatrix; ///< Matrix containing the gain of each PSPMT anode (in percent)
	std::vector<std::vector<int> > countMatrix; ///< Matrix containing the number of photon counts of each PSPMT anode
	
	/** Increment the anode hit count matrix at position (x, y)
	  * @param x Anode column
	  * @param y Anode row
	  */
	void increment(const int &x, const int &y);

	/** Get the gain of anode at position (x, y)
	  * @param x Anode column
	  * @param y Anode row
	  * @return The gain of anode (x, y) if it is defined, else 0
	  */	
	double getGain(const int &x, const int &y);
	
	/** Get a pointer to the Anger Logic current array for anode at position (x, y)
	  * @param x Anode column
	  * @param y Anode row
	  * @return Pointer to array containing Anger Logic currents for anode at position (x, y) if it exists, else NULL
	  */	
	double *getCurrent(const int &x, const int &y);
};

#endif
