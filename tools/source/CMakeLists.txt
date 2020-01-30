#Build NIST database list/search program.
if(BUILD_TOOLS_NISTLIST)
	add_executable(nistList nistList.cc)
	target_link_libraries(nistList NextSimCore NextSimPeripheral ${Geant4_LIBRARIES})
	install(TARGETS nistList DESTINATION bin)
endif(BUILD_TOOLS_NISTLIST)

#Build nextSim to simpleScan output converter.
if(BUILD_TOOLS_CONVERTER)
	add_executable(nextConvert nextConvert.cc)
	target_link_libraries(nextConvert NextSimPeripheral ${DICTIONARY_NAME} ${SimpleScan_DICTIONARY} ${ROOT_LIBRARIES})
	install(TARGETS nextConvert DESTINATION bin)
endif(BUILD_TOOLS_CONVERTER)

if(BUILD_TOOLS_CMDSEARCH)
	add_executable(nextCmdSearch nextCmdSearch.cc)
	target_link_libraries(nextCmdSearch NextSimOutput NextSimDetector NextSimGenerator NextSimPeripheral NextSimCore ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${cadmesh_LIBRARIES})
	install(TARGETS nextCmdSearch DESTINATION bin)
endif(BUILD_TOOLS_CMDSEARCH)

if(BUILD_TOOLS_MACROREADER)
	add_executable(nextMacReader macroReader.cc)
	target_link_libraries(nextMacReader NextSimCore NextSimPeripheral ${ROOT_LIBRARIES})
	install(TARGETS nextMacReader DESTINATION bin)	
endif(BUILD_TOOLS_MACROREADER)
