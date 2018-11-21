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
#include "ParticleSource.hh"
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
  handler.add(optionExt("tree", required_argument, NULL, 't', "<treename>", "Set the output TTree name (default=\"data\")."));
  handler.add(optionExt("yield", required_argument, NULL, 'Y', "<multiplier>", "Specify the light yield multiplier to use when producing photons (default=1)."));
  handler.add(optionExt("verbose", no_argument, NULL, 'V', "", "Toggle verbose mode."));

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

  double yieldMult = -1;
  if(handler.getOption(4)->active) // Set output TTree name
    yieldMult = strtod(handler.getOption(4)->argument.c_str(), NULL);

  bool verboseMode = false;
  if(handler.getOption(5)->active) // Toggle verbose flag
    verboseMode = true;

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
#endif

  // set mandatory initialization classes
  // Initialize the detector
  nDetConstruction* detector;
  if(yieldMult < 0){ 
    detector = new nDetConstruction();
  }
  else{ // Modify the photon yield of the detector
    std::cout << "neutronDetectorDesign: Setting photon yield multiplier to " << yieldMult << std::endl;
    detector = new nDetConstruction(yieldMult);
  }
  runManager->SetUserInitialization( detector );

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
    runAction->setOutputFilename((prefix+"_m.root").c_str());
  }
  if(!outputTreeName.empty()) runAction->setOutputTreeName(outputTreeName);
  if(verboseMode) runAction->toggleVerboseMode();
  runManager->SetUserAction( runAction );

  G4VUserPrimaryGeneratorAction* primaryGeneratorAction = new ParticleSource(runAction, detector);
  runManager->SetUserAction( primaryGeneratorAction );
  
  nDetEventAction* eventAction = new nDetEventAction(runAction);
  runManager->SetUserAction( eventAction );

  nDetSteppingAction* steppingAction = new nDetSteppingAction(detector, runAction, eventAction);
  runManager->SetUserAction(steppingAction);

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
      // Set root output to a single output file.
      runAction->setPersistentMode(true);

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
