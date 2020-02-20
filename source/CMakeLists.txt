#Enable support for GDML.
if(GEANT4_GDML)
	add_definitions(-DUSE_GDML)
endif(GEANT4_GDML)

#Enable multi-threading support..
if(GEANT4_MT)
	add_definitions(-DUSE_MULTITHREAD)
endif(GEANT4_MT)

#Check the Geant install version
# A non-insignificant number of Geant method names changed after 10.4, so we need to check
if(Geant4_VERSION VERSION_LESS "10.4")
    # Version 10.3 and below
    message(WARNING "An older version of Geant4 is being used(${Geant4_VERSION}). Compilation errors may be introduced, as many Geant method names were changed in later version.")
    add_definitions(-DGEANT_OLDER_VERSION)
endif()

#Set the scan sources that we will make a lib out of.
set(NextSimCoreSources nDetRunAction.cc nDetActionInitialization.cc nDetEventAction.cc nDetSteppingAction.cc nDetTrackingAction.cc nDetStackingAction.cc
                       messengerHandler.cc centerOfMass.cc pmtResponse.cc cmcalc.cc photonCounter.cc nistDatabase.cc)

set(NextSimOutputSources nDetMasterOutputFile.cc nDetMasterOutputFileMessenger.cc nDetDataPack.cc)
#set(NextSimDetectorSources nDetMaterials.cc nDetMaterialsMessenger.cc nDetConstruction.cc nDetConstructionMessenger.cc nDetWorld.cc nDetWorldMessenger.cc
#                           nDetWorldObject.cc nDetDetector.cc nDetDetectorTypes.cc nDetDetectorMessenger.cc nDetDetectorLayer.cc gdmlSolid.cc nDetDynamicMaterial.cc
#                           )
set(NextSimDetectorSources nDetMaterials.cc nDetMaterialsMessenger.cc nDetConstruction.cc nDetConstructionMessenger.cc nDetWorld.cc nDetWorldMessenger.cc
                           nDetWorldObject.cc nDetDetector.cc nDetDetectorTypes.cc nDetDetectorMessenger.cc nDetDetectorLayer.cc gdmlSolid.cc nDetDynamicMaterial.cc
                           CERNFrame.cc CERNTapeBox.cc MaterialsManager.cc
                           CERNSupport.cc ArgoneFrame.cc
                           CERNFloor.cc 
                           RIKENFloor.cc
                           RIKENSupport.cc
                           Polyhedron.cc IS530_Chamber.cc IS530_Plastic.cc Tape.cc
                           CloverQuadBuchDetector.cc CloverQuadDetector.cc CloverSingleBuchDetector.cc CloverSingleDetector.cc
                           )
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

#Capture the current commit (version) number
set(VERSION_SCRIPT ${TOP_DIRECTORY}/cmake/version.sh)
set(VERSION_HEADER ${TOP_DIRECTORY}/include/version.hh)
set(INTERNAL_VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.hh)
set(GIT_DIR ${TOP_DIRECTORY}/.git)

#Workaround because 'cut' wouldn't work correctly through cmake
execute_process(COMMAND cat ${GIT_DIR}/HEAD OUTPUT_VARIABLE GIT_HEAD_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "ref: " "" GIT_HEAD ${GIT_HEAD_DIR})
set(GIT_HEAD_REF ${GIT_DIR}/${GIT_HEAD})

add_custom_target(GetVersion ALL DEPENDS ${VERSION_HEADER})
add_custom_command(OUTPUT ${INTERNAL_VERSION_HEADER} COMMAND ${VERSION_SCRIPT} > ${INTERNAL_VERSION_HEADER} DEPENDS ${GIT_HEAD_REF} ${VERSION_SCRIPT} VERBATIM)
add_custom_command(OUTPUT ${VERSION_HEADER} COMMAND cp ${INTERNAL_VERSION_HEADER} ${VERSION_HEADER} DEPENDS ${INTERNAL_VERSION_HEADER} VERBATIM)

#Build simpleScan executable.
add_executable(nextSim nextSim.cc)
#target_link_libraries(nextSim NextSimOutput NextSimDetector NextSimGenerator NextSimPeripheral NextSimCore ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
target_link_libraries(nextSim NextSimOutput NextSimDetector NextSimGenerator NextSimPeripheral NextSimCore ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${cadmesh_LIBRARIES})
install(TARGETS nextSim DESTINATION bin)
