# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(TreeSearch)

SET(INCLUDES  ${Boost_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/include")
INCLUDE_DIRECTORIES(${INCLUDES})

# Set output path for libraries and executables within build directory (makes linking easier...)
SET(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

add_executable(PatternPackerULL PatternPackerULL.cc )
add_executable(PatternCheckULL PatternCheckULL.cc )
add_executable(PatternPackerBS70 PatternPackerBS70.cc )
add_executable(PatternCheckBS70 PatternCheckBS70.cc )
add_executable(PatternPackerBS177 PatternPackerBS177.cc )
add_executable(PatternCheckBS177 PatternCheckBS177.cc )
add_executable(PatternPackerBS180 PatternPackerBS180.cc )
add_executable(PatternCheckBS180 PatternCheckBS180.cc )
