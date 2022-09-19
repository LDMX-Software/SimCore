# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(
  SimCore
  VERSION 2.0.0
  DESCRIPTION "Core classes needed to run the Simulator."
  LANGUAGES C CXX)

find_package(fire REQUIRED)
if (NOT TARGET Geant4::Interface) 
  find_package(Geant4 REQUIRED gdml ui_all vis_all)
  set(G4_DEF_TEMP "")
  foreach(def ${Geant4_DEFINITIONS})
    string(REPLACE "-D" "" def ${def})
    list(APPEND G4_DEF_TEMP ${def})
  endforeach()
  add_library(Geant4::Interface INTERFACE IMPORTED GLOBAL)
  set_target_properties(Geant4::Interface PROPERTIES
    INTERFACE_LINK_LIBRARIES "${Geant4_LIBRARIES}"
    INTERFACE_COMPILE_OPTIONS "${Geant4_Flags}"
    INTERFACE_COMPILE_DEFINITIONS "${G4_DEF_TEMP}"
    INTERFACE_INCLUDE_DIRECTORIES "${Geant4_INCLUDE_DIRS}")
  unset(G4_DEF_TEMP)
  message(STATUS "Geant4 ${Geant4_VERSION} found")
endif()

add_library(SimCore_Event SHARED
  src/SimCore/Event/SimCalorimeterHit.cxx
  src/SimCore/Event/SimTrackerHit.cxx
  src/SimCore/Event/SimParticle.cxx)
target_link_libraries(SimCore_Event PUBLIC fire::io)
if (fire_USE_ROOT)
  target_include_directories(SimCore_Event PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>")
  root_generate_dictionary(SimCore_EventDict
    include/SimCore/Event/SimCalorimeterHit.h
    include/SimCore/Event/SimTrackerHit.h
    include/SimCore/Event/SimParticle.h
    LINKDEF include/SimCore/Event/LinkDef.h
    MODULE SimCore_Event)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libSimCore_Event_rdict.pcm DESTINATION lib)
endif()

# reading LHE files
add_library(SimCore_LHE SHARED
  src/SimCore/LHE/LHEEvent.cxx
  src/SimCore/LHE/LHEParticle.cxx
  src/SimCore/LHE/LHEReader.cxx)
target_link_libraries(SimCore_LHE PUBLIC Geant4::Interface fire::exception)
target_include_directories(SimCore_LHE PUBLIC include)

# we are a Geant4 User library
add_library(SimCore_G4User SHARED
  src/SimCore/G4User/EventAction.cxx
  src/SimCore/G4User/PrimaryGeneratorAction.cxx
  src/SimCore/G4User/RunAction.cxx
  src/SimCore/G4User/StackingAction.cxx
  src/SimCore/G4User/SteppingAction.cxx
  src/SimCore/G4User/TrackingAction.cxx)
target_link_libraries(SimCore_G4User PUBLIC Geant4::Interface fire::framework)

# special custom dark brem
add_library(SimCore_DarkBrem SHARED
  src/SimCore/DarkBrem/APrimePhysics.cxx
  src/SimCore/DarkBrem/DarkBremVertexLibraryModel.cxx
  src/SimCore/DarkBrem/G4APrime.cxx
  src/SimCore/DarkBrem/G4eDarkBremsstrahlung.cxx)
target_link_libraries(SimCore_DarkBrem PUBLIC Geant4::Interface fire::configure fire::framework ROOT::Physics)

# simulator process and its supporting classes
add_library(SimCore_SimCore SHARED
  src/SimCore/Geo/AuxInfoReader.cxx
  src/SimCore/Geo/GDMLParser.cxx
  src/SimCore/Geo/ParserFactory.cxx
  src/SimCore/DetectorConstruction.cxx
  src/SimCore/G4Session.cxx
  src/SimCore/GammaPhysics.cxx
  src/SimCore/MagneticFieldMap3D.cxx
  src/SimCore/ParallelWorld.cxx
  src/SimCore/PrimaryGenerator.cxx
  src/SimCore/RunManager.cxx
  src/SimCore/SensitiveDetector.cxx
  src/SimCore/Simulator.cxx
  src/SimCore/TrackMap.cxx
  src/SimCore/UserAction.cxx
  src/SimCore/UserEventInformation.cxx
  src/SimCore/UserRegionInformation.cxx
  src/SimCore/UserTrackInformation.cxx
  src/SimCore/XsecBiasingOperator.cxx)
target_link_libraries(SimCore_SimCore PUBLIC fire::framework DetDescr::DetDescr
                                             SimCore_Event SimCore_DarkBrem SimCore_G4User)


add_library(SimCore_BiasOperators SHARED
  src/SimCore/BiasOperators/DarkBrem.cxx
  src/SimCore/BiasOperators/ElectroNuclear.cxx
  src/SimCore/BiasOperators/GammaToMuPair.cxx
  src/SimCore/BiasOperators/K0LongInelastic.cxx
  src/SimCore/BiasOperators/NeutronInelastic.cxx
  src/SimCore/BiasOperators/PhotoNuclear.cxx)
target_link_libraries(SimCore_BiasOperators PUBLIC SimCore_SimCore)

add_library(SimCore_Generators SHARED
  src/SimCore/Generators/GeneralParticleSource.cxx
  src/SimCore/Generators/LHEPrimaryGenerator.cxx
  src/SimCore/Generators/MultiParticleGunPrimaryGenerator.cxx
  src/SimCore/Generators/ParticleGun.cxx
  src/SimCore/Generators/RootCompleteReSim.cxx
  src/SimCore/Generators/RootSimFromEcalSP.cxx)
target_link_libraries(SimCore_Generators PUBLIC SimCore_SimCore SimCore_LHE)

add_library(SimCore_SDs SHARED
  src/SimCore/SDs/EcalSD.cxx
  src/SimCore/SDs/HcalSD.cxx
  src/SimCore/SDs/ScoringPlaneSD.cxx
  src/SimCore/SDs/TrackerSD.cxx
  src/SimCore/SDs/TrackerSD.cxx)
target_link_libraries(SimCore_SDs PUBLIC SimCore_SimCore)

# Set some target properties
set_target_properties(
  SimCore_LHE SimCore_G4User SimCore_SimCore SimCore_DarkBrem
  SimCore_BiasOperators SimCore_Generators SimCore_SDs
                      PROPERTIES CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO)
install(TARGETS
  SimCore_LHE SimCore_G4User SimCore_SimCore SimCore_DarkBrem
  SimCore_BiasOperators SimCore_Generators SimCore_SDs
  LIBRARY DESTINATION lib
  INCLUDES DESTINATION include)

# Unpack the example dark brem vertex library (or libraries)
file(GLOB vertex_libraries data/*.tar.gz)

option(INSTALL_EXAMPLE_DB_LIBRARIES 
  "Unpack and install the example dark brem event libraries." ON)
if(INSTALL_EXAMPLE_DB_LIBRARIES)
  # set installation location of dark brem vertex libraries
  #   also used when configuring the makePath.py module
  set(DB_VERTEX_LIB_INSTALL ${CMAKE_INSTALL_PREFIX}/data/db_vertex_libraries)
  if(NOT EXISTS ${DB_VERTEX_LIB_INSTALL})
    #assume new build ==> unpack vertex libraries
    message(STATUS "Unpacking example dark brem vertex libraries.")
    file(MAKE_DIRECTORY ${DB_VERTEX_LIB_INSTALL})
    foreach(library_path ${vertex_libraries})
      execute_process(COMMAND 
        tar -C ${DB_VERTEX_LIB_INSTALL} --skip-old-files -zxf ${library_path})
    endforeach()
  endif()

  # Add compiled tests
  configure_file(${PROJECT_SOURCE_DIR}/test/DarkBremProcessTest.cxx.in 
    ${PROJECT_SOURCE_DIR}/test/DarkBremProcessTest.cxx)
  #setup_test(dependencies SimCore::SimCore Geant4::Interface)

  # Configure the executable so it knows the installation path of the db vertex libraries
  configure_file(${PROJECT_SOURCE_DIR}/src/SimCore/DarkBrem/print_dark_brem_xsec_table.cxx.in
    ${CMAKE_CURRENT_BINARY_DIR}/bin/print_dark_brem_xsec_table.cxx)
  add_executable(print-dark-brem-xsec-table 
    ${CMAKE_CURRENT_BINARY_DIR}/bin/print_dark_brem_xsec_table.cxx)

  # Link to the Framework library
  target_link_libraries(print-dark-brem-xsec-table 
    PRIVATE Geant4::Interface 
    SimCore::SimCore 
    Framework::Framework)

  # Install the fire executable
  install(TARGETS print-dark-brem-xsec-table 
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

#setup_python(package_name ${PYTHON_PACKAGE_NAME}/SimCore)

# add visualization executable
add_executable(g4-vis src/SimCore/g4_vis.cxx)
target_link_libraries(g4-vis PRIVATE Geant4::Interface SimCore::SimCore)
install(TARGETS g4-vis DESTINATION bin)
