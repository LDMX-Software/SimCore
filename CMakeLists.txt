
# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(SimCore VERSION 2.1.0
                DESCRIPTION "Core classes needed to run the Simulator."
                LANGUAGES CXX
)

# Check if the project specifc macros have been loaded. If not, and the 
# stand alone build option has been given, clone the project locally and 
# include them.  Otherwise, error out.
if (NOT build_macros_found)

    message(STATUS "Downloading CMake modules.")

    # Set the URL from which to download the modules from
    set(url https://github.com/LDMX-Software/cmake/archive/master.zip)

    # Download the file 
    file(DOWNLOAD ${url} ${CMAKE_INSTALL_PREFIX}/external/master.zip STATUS status)

    # If the file fails to download, throw a fatal error and exit
    list(GET status 0 error)
    if(error)
        message(FATAL_ERROR "Could not download ${url}")
    endif()

    # Extract the archive
    execute_process(COMMAND unzip master.zip
                    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/external)

    # Load additional macro commands 
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/external/cmake-master)

    # Load the BuildMacros module. If loaded correctly, the variable 
    # 'build_macros_found' will be set to the path of the module.  Otherwise, it
    # is set to NOTFOUND.
    include(BuildMacros)

endif()

# Configure the Geant4 target
setup_geant4_target()

# Add the fire-interactive executable
add_executable(fire-interactive ${PROJECT_SOURCE_DIR}/src/fire_interactive.cxx)

# Link to the Geant4 library
target_link_libraries(fire-interactive PRIVATE Geant4::Interface)

# Install the fire executable
install(TARGETS fire-interactive DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# If only building the interactive application, skip processing the rest of the
# configuration. 
option(INTERACTIVE_ONLY "Only build the interactive application." OFF)
if (INTERACTIVE_ONLY)
    return()
endif()

# Setup the library 
setup_library(name SimCore 
              dependencies Geant4::Interface DARK::Framework DARK::DetDescr
              python_install_path ${PYTHON_INSTALL_PREFIX}
)

# Set some target properties
set_target_properties(SimCore PROPERTIES 
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

