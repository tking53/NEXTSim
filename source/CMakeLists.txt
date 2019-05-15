#Enable support for GDML.
if(GEANT4_GDML)
	add_definitions(-DUSE_GDML)
endif(GEANT4_GDML)

#Enable multi-threading support..
if(GEANT4_MT)
	add_definitions(-DUSE_MULTITHREAD)
endif(GEANT4_MT)

#Set the scan sources that we will make a lib out of.
set(NextSimSources centerOfMass.cc nDetConstruction.cc nDetActionInitialization.cc nDetEventAction.cc nDetRunActionMessenger.cc 
                   nDetMasterOutputFile.cc nDetMasterOutputFileMessenger.cc nDetSteppingAction.cc ParticleSource.cc ParticleSourceMessenger.cc 
                   pmtResponse.cc messengerHandler.cc nDetConstructionMessenger.cc nDetRunAction.cc nDetStackingAction.cc cmcalc.cc 
                   nDetTrackingAction.cc optionHandler.cc photonCounter.cc gdmlSolid.cc nistDatabase.cc nDetDataPack.cc termColors.cc)

#Add the sources to the library.
add_library(NextSimObjects OBJECT ${NextSimSources})

#Generate a static library.
add_library(NextSimStatic STATIC $<TARGET_OBJECTS:NextSimObjects>)

#Build simpleScan executable.
add_executable(nextSim nextSim.cc)
target_link_libraries(nextSim NextSimStatic ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
install(TARGETS nextSim DESTINATION bin)
