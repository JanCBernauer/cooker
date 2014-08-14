# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(MagneticField)

SET(INCLUDES ${Geant4_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/include" ${ROOT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${INCLUDES})

add_library(MagneticField STATIC src/Spline_Field.cpp src/spline_interp.cpp src/Old_Magnetic_Field.cpp src/NoInterp_Magnetic_Field.cpp src/segGen.cc)

#download (magnetfieldbins
#http://mit.edu/~bernauer/www/MagneticField/cooker_v1.deriv.bin bin/cooker_v1.deriv.bin)
#download (magnetfieldbinv2
#http://mit.edu/~bernauer/www/MagneticField/cooker_v2.bin bin/cooker_v2.bin)

#download (magnetfieldbinsm
#http://mit.edu/~bernauer/www/MagneticField/cooker_v1m.deriv.bin bin/cooker_v1m.deriv.bin)

#add_dependencies(MagneticField magnetfieldbins magnetfieldbinsm magnetfieldbinv2)

#INSTALL (FILES 
#${CMAKE_CURRENT_BINARY_DIR}/bin/cooker_v1.deriv.bin
#DESTINATION
#"../shared/MagneticField/")

#INSTALL (FILES 
#${CMAKE_CURRENT_BINARY_DIR}/bin/cooker_v1m.deriv.bin
#DESTINATION
#"../shared/MagneticField/")

#INSTALL (FILES 
#${CMAKE_CURRENT_BINARY_DIR}/bin/cooker_v2.bin
#DESTINATION
#"../shared/MagneticField/")
