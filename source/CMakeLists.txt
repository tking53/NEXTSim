#Enable support for GDML.
if(GEANT4_GDML)
	add_definitions(-DUSE_GDML)
endif(GEANT4_GDML)

#Enable multi-threading support..
if(GEANT4_MT)
	add_definitions(-DUSE_MULTITHREAD)
endif(GEANT4_MT)

#Set the scan sources that we will make a lib out of.
set(NextSimCoreSources nDetRunAction.cc nDetActionInitialization.cc nDetEventAction.cc nDetSteppingAction.cc nDetTrackingAction.cc nDetStackingAction.cc
                       messengerHandler.cc centerOfMass.cc pmtResponse.cc cmcalc.cc photonCounter.cc nistDatabase.cc)

set(NextSimOutputSources nDetMasterOutputFile.cc nDetMasterOutputFileMessenger.cc nDetDataPack.cc)
set(NextSimDetectorSources nDetMaterials.cc nDetMaterialsMessenger.cc nDetConstruction.cc nDetConstructionMessenger.cc nDetWorld.cc nDetWorldMessenger.cc
                           nDetWorldObject.cc nDetDetector.cc nDetDetectorMessenger.cc nDetDetectorLayer.cc gdmlSolid.cc nDetDynamicMaterial.cc)
set(NextSimGeneratorSources nDetParticleSource.cc nDetParticleSourceMessenger.cc)
set(NextSimPeripheralSources optionHandler.cc termColors.cc)

#Add the sources to the library.
add_library(NextSimCoreObj OBJECT ${NextSimCoreSources})
add_library(NextSimOutputObj OBJECT ${NextSimOutputSources})
add_library(NextSimDetectorObj OBJECT ${NextSimDetectorSources})
add_library(NextSimGeneratorObj OBJECT ${NextSimGeneratorSources})
add_library(NextSimPeripheralObj OBJECT ${NextSimPeripheralSources})

#Generate a static library.
add_library(NextSimCore STATIC $<TARGET_OBJECTS:NextSimCoreObj>)
add_library(NextSimOutput STATIC $<TARGET_OBJECTS:NextSimOutputObj>)
add_library(NextSimDetector STATIC $<TARGET_OBJECTS:NextSimDetectorObj>)
add_library(NextSimGenerator STATIC $<TARGET_OBJECTS:NextSimGeneratorObj>)
add_library(NextSimPeripheral STATIC $<TARGET_OBJECTS:NextSimPeripheralObj>)

#Link the NextSim output library with the root dictionary
target_link_libraries(NextSimOutput ${DICTIONARY_NAME})

#Build simpleScan executable.
add_executable(nextSim nextSim.cc)
target_link_libraries(nextSim NextSimOutput NextSimDetector NextSimGenerator NextSimPeripheral NextSimCore ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
install(TARGETS nextSim DESTINATION bin)
