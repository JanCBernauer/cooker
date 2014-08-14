# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.3 FATAL_ERROR)
# Needed for cmake 2.6
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set project name
PROJECT(alacarte)

FIND_PACKAGE(Qt4 REQUIRED)

SET(INCLUDES ${Boost_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/src/include" "${CMAKE_SOURCE_DIR}/include")

INCLUDE_DIRECTORIES(${INCLUDES})

SET(alacarte_SOURCES src/alacarte.cpp src/database.cpp
src/mainwindow.cpp src/checkpivotmodel.cpp)
SET(alacarte_HEADERS src/include/mainwindow.h)

QT4_WRAP_CPP(alacarte_HEADERS_MOC ${alacarte_HEADERS})

SET(QT_USE_QTSQL TRUE)

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


add_executable(alacarte ${alacarte_SOURCES} ${alacarte_HEADERS_MOC})

TARGET_LINK_LIBRARIES(alacarte ${Boost_LIBRARIES} ${QT_LIBRARIES} )

#download (alacartelib
#http://web.mit.edu/bernauer/www/alacarte/rundata.db db/rundata.db)

#add_dependencies(alacarte alacartelib)

#INSTALL (FILES 
#${CMAKE_CURRENT_BINARY_DIR}/db/rundata.db
#DESTINATION
#"../shared/alacarte/")



INSTALL(TARGETS alacarte RUNTIME DESTINATION ${BINDIR} LIBRARY DESTINATION ${LIBDIR} ARCHIVE DESTINATION ${LIBDIR})

