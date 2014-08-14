# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(combine)

SET(INCLUDES ${ROOT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/include")

INCLUDE_DIRECTORIES(${INCLUDES})

add_executable(combine src/combine.cpp )

TARGET_LINK_LIBRARIES(combine Chef "-L${ROOT_LIBRARY_DIR}"  Core Cint Tree RIO Net MathCore Thread FTGL ${Boost_LIBRARIES})

INSTALL(TARGETS combine RUNTIME DESTINATION ${BINDIR} LIBRARY DESTINATION ${LIBDIR} ARCHIVE DESTINATION ${LIBDIR})



