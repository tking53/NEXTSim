//
// ********************************************************************
// * The beutron detector program is used to simulate an              *
// * optimize the performance of the scintillation detector in the    * 
// * framework of Geant4.10.00.p01 and ROOT v5.34.14.                 *
// *  by Dr. Xiaodong Zhang on Sept., 2015.                           *
// *                                                                  *
// ********************************************************************
//
// log: nDet.cc,v1.0  Sept., 2015
// --------------------------------------------------------------
//
                                                                         
// c/c++ headers

// Geant4 base classes
#include "G4MTRunManager.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

// using the modular physics list
#include "G4VModularPhysicsList.hh"
//#include "LBE.hh"
#include "QGSP_BERT_HP.hh"

//trying Sergei's physics list
#include "LENSLongPhysicsList.hh"

// selfdefine classes
#include "nDetConstruction.hh"
#include "nDetPrimaryGeneratorAction.hh"
#include "nDetRunAction.hh"
#include "nDetEventAction.hh"
#include "nDetStackingAction.hh"
#include "nDetSteppingAction.hh"
#include "nDetAnalysisManager.hh"
#include "nDetTrackingAction.hh"
#include "nDetPhysicsList.hh"
#include "CfSource.hh"
#include "optionHandler.hh"
//#include "nDetActionInitialization.hh"

#include "G4OpticalPhysics.hh"

#include "Randomize.hh"
#include "time.h"

int main(int argc, char** argv)
{
  optionHandler handler;
  handler.add(optionExt("input", required_argument, NULL, 'i', "<filename>", "Specify an input geant macro."));
  handler.add(optionExt("output", required_argument, NULL, 'o', "<filename>", "Specify the name of the output file."));
  handler.add(optionExt("gui", no_argument, NULL, 'g', "", "Run interactive GUI session."));
  handler.add(optionExt("tree-name", required_argument, NULL, 't', "<treename>", "Set the output TTree name (default=\"theTree\")."));
  handler.add(optionExt("252Cf", no_argument, NULL, 0x0, "", "Use 252Cf energy spectrum (Mannhart)."));
  handler.add(optionExt("spectral", required_argument, NULL, 0x0, "<filename>", "Enable PMT spectral response."));
  handler.add(optionExt("verbose", no_argument, NULL, 'V', "", "Toggle verbose mode."));
  handler.add(optionExt("run-index", required_argument, NULL, 0x0, "<ID>", "Specify starting filename suffix number (default=1)."));

  // Handle user input.
  if(!handler.setup(argc, argv))
    return 1;

  std::string inputFilename;
  if(handler.getOption(0)->active) // Set input filename
    inputFilename = handler.getOption(0)->argument;

  std::string outputFilename;
  if(handler.getOption(1)->active) // Set output filename
    outputFilename = handler.getOption(1)->argument;

  bool batchMode = true;
  if(handler.getOption(2)->active) // Set interactive mode
  	batchMode = false;

  std::string outputTreeName;
  if(handler.getOption(3)->active) // Set output TTree name
    outputTreeName = handler.getOption(3)->argument;

  bool useCaliforniumSpectrum = false;
  if(handler.getOption(4)->active) // Use 252Cf energy spectrum
    useCaliforniumSpectrum = true;

  std::string specResponseFilename;
  if(handler.getOption(5)->active) // Use PMT spectral response
    specResponseFilename = handler.getOption(5)->argument;

  bool verboseMode = false;
  if(handler.getOption(6)->active) // Toggle verbose flag
    verboseMode = true;

  G4int startRunID = -1;
  if(handler.getOption(7)->active) // Specify starting filename prefix number
    startRunID = strtol(handler.getOption(7)->argument.c_str(), NULL, 10);

  if(batchMode && inputFilename.empty()){
  	std::cout << " ERROR: Input macro filename not specified!\n";
  	return 1;
  }

  //make random number seeds different in different runs in Geant4 simulation
  //////////////////////////////////////
  //choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
  //set random seed with system time
  G4long seed = time(NULL);
  CLHEP::HepRandom::setTheSeed(seed);
  //////////////////////////////////////


  // Construct the default run manager
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
  runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
#else
  G4RunManager* runManager = new G4RunManager;
  if(startRunID > 0){
    std::cout << " Setting start run file index to " << startRunID << std::endl;
    runManager->SetRunIDCounter(startRunID-1);
  }
#endif

  // set mandatory initialization classes
  // Initialize the detector
  nDetConstruction* detector = new nDetConstruction;
  runManager->SetUserInitialization( detector );

  std::cout<<"nDetConstruction()-->"<<detector<<std::endl;

/*  // Initialize the physics lists
  // The LBE modular physics list is used in this program
  // For details about this physics list, please reference source code
  // 
  G4VModularPhysicsList* physics = new LBE();
  runManager->SetUserInitialization( physics );
  */
 
/*// Using the VANDLE (Sergei) physics list.
  LENSLongPhysicsList* physicsList = new LENSLongPhysicsList;
  runManager->SetUserInitialization( physicsList );
that didn't work... this is horribly deprecated*/


//Using "high precision" neutron scattering model.  Warning: this will be slow.


  //nDetPhysicsList* physics = (nDetPhysicsList*)new QGSP_BERT_HP();
    //nDetPhysicsList* physics = new nDetPhysicsList();
  G4VModularPhysicsList* physics = new QGSP_BERT_HP();

  G4OpticalPhysics *theOpticalPhysics=new G4OpticalPhysics();
  theOpticalPhysics->SetScintillationByParticleType(true);
  physics->ReplacePhysics(theOpticalPhysics);
  runManager->SetUserInitialization( physics );

#ifdef G4VIS_USE
  // add visulization manager
  G4VisManager *visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  //set optional user action classes

  nDetAnalysisManager *theManager= new nDetAnalysisManager();
  if(!outputFilename.empty()) theManager->setFilename(outputFilename);
  if(!outputTreeName.empty()) theManager->setTreeName(outputTreeName);

  //Geant complains we need to change this for multithreading.  Moving the following to nDetActionInitialization.cc
  nDetRunAction* runAction = new nDetRunAction(detector);
  if(!outputFilename.empty()){
    size_t index = outputFilename.find('.');
    std::string prefix = outputFilename;
    if(index != std::string::npos) prefix = outputFilename.substr(0, index);
    runAction->setFilename((prefix+"_m.root").c_str());
  }
  if(!outputTreeName.empty()) runAction->setTreeName(outputTreeName);
  if(verboseMode) runAction->toggleVerboseMode();
  runManager->SetUserAction( runAction );

  G4VUserPrimaryGeneratorAction* primaryGeneratorAction;
  
  if(!useCaliforniumSpectrum) // Standard particle generator
    primaryGeneratorAction = new nDetPrimaryGeneratorAction(runAction);
  else // 252Cf source spectrum (CRT)
    primaryGeneratorAction = new ParticleSource(runAction, detector);
  runManager->SetUserAction( primaryGeneratorAction );
  
  nDetEventAction* eventAction = new nDetEventAction(runAction);
  runManager->SetUserAction( eventAction );

  nDetSteppingAction* steppingAction = new nDetSteppingAction(detector, runAction, eventAction);
  runManager->SetUserAction(steppingAction);

  if(!specResponseFilename.empty()){ // Load PMT spectral response.
    detector->setPmtSpectralResponse(specResponseFilename.c_str());
  }
  
  nDetStackingAction* stackingAction = new nDetStackingAction(runAction);
  runManager->SetUserAction(stackingAction);

  nDetTrackingAction*  trackingAction = new nDetTrackingAction(runAction);
  runManager->SetUserAction(trackingAction);

  runAction->setActions(stackingAction, trackingAction, steppingAction);

//So we do this instead K Schmitt 6/16/16
//  runManager->SetUserInitialization(new nDetActionInitialization());
//end of Kyle's changes

  // Initialize G4 kernel
  runManager->Initialize();
  
  // get the pointer to the UI manager and set verbosities
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  if (!batchMode){   // Define UI session for interactive mode
#ifdef G4UI_USE
      G4UIExecutive * ui;
      ui = new G4UIExecutive(argc, argv, "");
      UImanager->ApplyCommand("/vis/open OGL");
      UImanager->ApplyCommand("/vis/drawVolume");
      UImanager->ApplyCommand("/vis/scene/add/trajectories");
      UImanager->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 90 0");
      UImanager->ApplyCommand("/vis/viewer/set/viewpointVector -1 1 1");
      UImanager->ApplyCommand("/vis/viewer/refresh");

	  /*G4String command = "/control/execute ";
      command += inputFilename;
      std::cout << "cmd=" << command << std::endl;
      UImanager->ApplyCommand(command);*/
      
      // start interactive session
      ui->SessionStart();      
      delete ui;
#endif
  }
  else{ // Batch mode
    G4String command = "/control/execute ";
    command += inputFilename;
    UImanager->ApplyCommand(command);
  }
 
  // job termination
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;
    //std::cout<<"runManager is deleted"<<std::endl;
  //delete verbosity;
  
  return 0;
}//END of main() function
