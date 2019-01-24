#Build NIST database list/search program.
#if(BUILD_TOOLS_NISTLIST)
#	add_executable(nistList nistList.cc)
#	target_link_libraries(nextSim NextSimStatic ${Geant4_LIBRARIES})
#	install(TARGETS nistList DESTINATION bin)
#endif(BUILD_TOOLS_NISTLIST)

#Build nextSim to simpleScan output converter.
if(BUILD_TOOLS_CONVERTER)
	add_executable(nextConvert nextConvert.cc)
	target_link_libraries(nextConvert ${SimpleScan_DICTIONARY} ${ROOT_LIBRARIES})
	install(TARGETS nextConvert DESTINATION bin)
endif(BUILD_TOOLS_CONVERTER)
