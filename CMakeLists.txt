#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(particleProduction)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
find_package(Geant4 REQUIRED)

# Optional: detect vis support and enable a macro
if(TARGET Geant4::G4vis_management)
  message(STATUS "Visualization support found in Geant4.")
  set(G4UI_USE TRUE)
endif()

#----------------------------------------------------------------------------
# Find ROOT
#
find_package(ROOT REQUIRED COMPONENTS Core Hist MathCore)

if(ROOT_FOUND)
  message(STATUS "ROOT found!")
  message(STATUS "ROOT version: ${ROOT_VERSION}")
  message(STATUS "ROOT include dirs: ${ROOT_INCLUDE_DIRS}")
endif()

# Include ROOT headers
include_directories(${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(particleProduction particleProduction.cc ${sources} ${headers})
target_link_libraries(particleProduction ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(particleProduction DEPENDS particleProduction)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS particleProduction DESTINATION bin)
