# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(detectorlib)
# searches for needed packages


EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE ARCHDIR)


SET(INCLUDES 
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIRS}  
"${CMAKE_CURRENT_SOURCE_DIR}/include" 
 "${CMAKE_SOURCE_DIR}/include")

include_directories(${Geant4_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${INCLUDES})

# Set output path for libraries and executables within build directory (makes linking easier...)
SET(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

FILE(GLOB detector_src src/*.cpp)


SET(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_library(detector
	${detector_src} 
)
TARGET_LINK_LIBRARIES(detector "-L${ROOT_LIBRARY_DIR}" Core Cint MathCore MathMore MagneticField)

add_definitions(${Geant4_DEFINITIONS})



