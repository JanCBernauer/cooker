# - Try to find CLHEP library
#
# Once done this will define
#
#   CLHEP_FOUND - system has the required CLHEP library
#   CLHEP_INCLUDE_DIRS - the CLHEP include directory
#   CLHEP_LIBRARIES - the libraries needed to use CLHEP
#   CLHEP_VERSION   - Version of CLHEP
#
# Usage of this module:
#
#   FIND_PACKAGE( CLHEP )
#

# Find the clhep-config program

find_program( CLHEP_CONFIG_EXECUTABLE NAMES clhep-config )
    
set( CLHEP_LIBRARIES )
set( CLHEP_INCLUDE_DIRS )
set( CLHEP_VERSION_OK )

   message(STATUS "Found CLHEP executable: ${CLHEP_CONFIG_EXECUTABLE}")

if ( CLHEP_CONFIG_EXECUTABLE )
  
   execute_process( COMMAND ${CLHEP_CONFIG_EXECUTABLE} --version
                    OUTPUT_VARIABLE CLHEP_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE )

   message(STATUS "Found CLHEP version: ${CLHEP_VERSION}")

   # Use clhep-config to find libs and includes

   execute_process( COMMAND ${CLHEP_CONFIG_EXECUTABLE} --libs
                    OUTPUT_VARIABLE CLHEP_CONFIG_LIBS
                    OUTPUT_STRIP_TRAILING_WHITESPACE )

   execute_process( COMMAND ${CLHEP_CONFIG_EXECUTABLE} --include
                    OUTPUT_VARIABLE CLHEP_CONFIG_INCLUDE_DIR
                    OUTPUT_STRIP_TRAILING_WHITESPACE )

   string ( REGEX REPLACE "(^| )-I" " "
            CLHEP_CONFIG_INCLUDE_DIR_STRIP "${CLHEP_CONFIG_INCLUDE_DIR}" )	      
   string ( REGEX REPLACE "\"" " "
            CLHEP_CONFIG_INCLUDE_DIR_STRIP2 "${CLHEP_CONFIG_INCLUDE_DIR_STRIP}" )	      
   string ( REGEX REPLACE "-lCLHEP[0-9.-]*" "-lCLHEP"
            CLHEP_CONFIG_LIBS_STRIP "${CLHEP_CONFIG_LIBS}" )	      

   if ( CLHEP_CONFIG_LIBS AND CLHEP_CONFIG_INCLUDE_DIR )

      # Resolve the includes and libraries

      SET( CLHEP_INCLUDE_DIRS ${CLHEP_CONFIG_INCLUDE_DIR_STRIP2} )

      SET ( CLHEP_LIBRARIES ${CLHEP_CONFIG_LIBS_STRIP} )

      message( STATUS "CLHEP Include directory: ${CLHEP_INCLUDE_DIRS}" )
      message( STATUS "CLHEP Include directory: ${CLHEP_LIBRARIES}" )

   endif ()

endif ()
