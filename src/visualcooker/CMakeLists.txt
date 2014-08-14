# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(visco)

SET(INCLUDES ${ROOT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/src/include" "${CMAKE_SOURCE_DIR}/include")

INCLUDE_DIRECTORIES(${INCLUDES})

SET(visco_SOURCES src/visualcooker.cpp src/controlwindow.cpp )
SET(visco_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/include/controlwindow.h)

# create root dictionary
SET(LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/src/include/LinkDef.hh)
SET(DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/viscoDict.cpp)
ROOT_GENERATE_DICTIONARY("${visco_HEADERS}" "${LINKDEF}" "${DICTIONARY}" "${INCLUDES}")
LIST(APPEND visco_SOURCES ${DICTIONARY})

add_executable(visco ${visco_SOURCES})

TARGET_LINK_LIBRARIES(visco Chef cookerrawtree "-L${ROOT_LIBRARY_DIR}"  Gui Core Cint
Tree RIO Net MathCore Thread Hist Matrix Rint Gpad Graf FTGL ${Boost_LIBRARIES} )


INSTALL(TARGETS visco RUNTIME DESTINATION ${BINDIR} LIBRARY DESTINATION ${LIBDIR} ARCHIVE DESTINATION ${LIBDIR})

