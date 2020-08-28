
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
    if (stand_alone_build)
        # Pull in the cmake module as an external package.
    else()
        message(FATAL_ERROR "Project specific build modules have not been loaded.")
    endif()
endif()

# Configure Geant4
setup_geant4_target()

# Search and configure ROOT
find_package(ROOT CONFIG REQUIRED)

# Unpack the example dark brem vertex library (or libraries)
file(GLOB vertex_libraries data/*.tar.gz)

# set installation location of dark brem vertex libraries
#   also used when configuring the makePath.py.in module
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
