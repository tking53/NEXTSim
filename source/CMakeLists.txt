#Enable support for GDML.
if(GEANT4_GDML)
	add_definitions(-DUSE_GDML)
endif(GEANT4_GDML)

#Set the scan sources that we will make a lib out of.
set(NextSimSources centerOfMass.cc nDetConstruction.cc nDetEventAction.cc nDetRunActionMessenger.cc 
                   nDetSteppingAction.cc ParticleSource.cc pmtResponse.cc messengerHandler.cc  
                   nDetConstructionMessenger.cc nDetRunAction.cc nDetStackingAction.cc 
                   nDetTrackingAction.cc optionHandler.cc photonCounter.cc gdmlSolid.cc nistDatabase.cc)

#Add the sources to the library.
add_library(NextSimObjects OBJECT ${NextSimSources})

#Generate a static library.
add_library(NextSimStatic STATIC $<TARGET_OBJECTS:NextSimObjects>)

#Build simpleScan executable.
add_executable(nextSim nextSim.cc)
target_link_libraries(nextSim NextSimStatic ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
install(TARGETS nextSim DESTINATION bin)
