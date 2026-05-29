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

# Try to find ROOT via config package first. If not found, try to infer ROOT_DIR from root-config.
find_package(ROOT REQUIRED COMPONENTS Core Hist MathCore)

if(NOT ROOT_FOUND)
  find_program(ROOT_CONFIG_EXECUTABLE root-config)
  if(ROOT_CONFIG_EXECUTABLE)
    execute_process(
      COMMAND ${ROOT_CONFIG_EXECUTABLE} --prefix
      OUTPUT_VARIABLE ROOT_PREFIX
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(ROOT_PREFIX)
      if(EXISTS "${ROOT_PREFIX}/lib/cmake/ROOT/ROOTConfig.cmake")
        set(ROOT_DIR "${ROOT_PREFIX}/lib/cmake/ROOT" CACHE PATH "Path to ROOTConfig.cmake")
      elseif(EXISTS "${ROOT_PREFIX}/cmake/ROOTConfig.cmake")
        set(ROOT_DIR "${ROOT_PREFIX}/cmake" CACHE PATH "Path to ROOTConfig.cmake")
      endif()
      find_package(ROOT REQUIRED COMPONENTS Core Hist MathCore)
    endif()
  endif()
endif()

if(NOT ROOT_FOUND)
  message(FATAL_ERROR
    "ROOT not found.\n"
    "Set ROOT_DIR to the directory containing ROOTConfig.cmake, or set CMAKE_PREFIX_PATH to your ROOT install prefix.\n"
    "Example:\n"
    "  cmake -S . -B build -DROOT_DIR=/path/to/root/lib/cmake/ROOT\n"
    "or:\n"
    "  cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/root"
  )
endif()

message(STATUS "ROOT found!")
message(STATUS "ROOT version: ${ROOT_VERSION}")
message(STATUS "ROOT include dirs: ${ROOT_INCLUDE_DIRS}")

if(DEFINED ROOT_CXX_STANDARD)
  message(STATUS "Setting C++ standard to match ROOT (C++${ROOT_CXX_STANDARD})")
  set(CMAKE_CXX_STANDARD ${ROOT_CXX_STANDARD})
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
  # Fallback for older ROOT package configs that do not define ROOT_CXX_STANDARD.
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  message(WARNING "ROOT_CXX_STANDARD not defined by ROOT; defaulting to C++17.")
endif()

set(HDF5_PREFER_PARALLEL FALSE)
find_package(HDF5 REQUIRED COMPONENTS C CXX)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${HDF5_INCLUDE_DIRS})

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

target_link_libraries(particleProduction
  ${Geant4_LIBRARIES}
  ${ROOT_LIBRARIES}
  ${HDF5_LIBRARIES}
)

install(TARGETS particleProduction DESTINATION bin)
