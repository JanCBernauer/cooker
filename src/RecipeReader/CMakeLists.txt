# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(RecipeReader)

SET(INCLUDES ${Boost_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/include")

INCLUDE_DIRECTORIES(${INCLUDES})
# Set output path for libraries and executables within build directory (makes linking easier...)

add_library(RecipeReader src/RecipeReader.cpp )
TARGET_LINK_LIBRARIES(RecipeReader  xqilla xerces-c)
