#Configure and install the module file
configure_file("modulefiles/nextSim" nextSim @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/nextSim DESTINATION share/modulefiles/)
