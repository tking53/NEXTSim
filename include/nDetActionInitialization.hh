#ifndef NDET_ACTION_INITIALIZATION_HH
#define NDET_ACTION_INITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

/*! \class nDetActionInitialization
 *  \brief Class used to set the user actions for worker and master threads
 *  \author Cory R. Thornsberry (cthornsb@vols.utk.edu)
 *  \date May 31, 2019
*/

class nDetActionInitialization : public G4VUserActionInitialization {
  public:
	/** Default constructor
	  * @param verboseMode Set the verbosity flag
	  */
	nDetActionInitialization(bool verboseMode=false);

	/** Set user actions for the worker threads. Called from G4RunManager::SetUserInitialization()
	  */
	virtual void Build() const ;
	
	/** Set user actions for the master thread. Called from G4MTRunManager::SetUserInitialization()
	  */
	virtual void BuildForMaster() const ;

  private:
	bool verbose; ///< Verbosity flag
};

#endif
