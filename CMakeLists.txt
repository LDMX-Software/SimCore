
# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(SimCore VERSION 2.1.0
                DESCRIPTION "Core classes needed to run the Simulator."
                LANGUAGES CXX
)

# Configure the Geant4 target
setup_geant4_target()

# Setup the library 
setup_library(name SimCore 
              dependencies Geant4::Interface 
                           ROOT::Physics 
                           DARK::Framework DARK::DetDescr
              python_install_path ${PYTHON_INSTALL_PREFIX}
)

# Set some target properties
set_target_properties(SimCore PROPERTIES 
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Add the fire-interactive executable
add_executable(fire-interactive ${PROJECT_SOURCE_DIR}/src/fire_interactive.cxx)

# Link to the Geant4 library
target_link_libraries(fire-interactive PRIVATE Geant4::Interface)

# Install the fire executable
install(TARGETS fire-interactive DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# Search and configure ROOT
find_package(ROOT CONFIG REQUIRED)
