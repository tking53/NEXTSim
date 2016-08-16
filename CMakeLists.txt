      # (1)
      cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

      project(LithiumFiberDesign)

      #include("/Applications/cern_root/root_v5.34.14/root_v5.34.14_installation/etc/cmake/FindROOT.cmake")

      # (2)
      option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
      if(WITH_GEANT4_UIVIS)
        find_package(Geant4 REQUIRED ui_all vis_all)
      else()
        find_package(Geant4 REQUIRED)
      endif()
      set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /home/kyle/Downloads/root/etc/cmake)
      find_package(ROOT)
      set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
      
      # (3)      
      include(${Geant4_USE_FILE})
      include_directories(${PROJECT_SOURCE_DIR}/include)
      include_directories(${ROOT_INCLUDE_DIR})
      message(status ${ROOT_INCLUDE_DIR})

     # (4)      
      file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
      file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

      # (5)
      add_executable(neutronDetectorDesign neutronDetectorDesign.cc ${sources})
      target_link_libraries(neutronDetectorDesign ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

      # (6)


      # (7)
      install(TARGETS neutronDetectorDesign DESTINATION /home/kyle/geant4.10.02/config/sys/Darwin-g++)
    #  install(TARGETS neutronDetectorDesign DESTINATION /home/kyle/geant4.9.6.p04/config/sys/Darwin-g++)
