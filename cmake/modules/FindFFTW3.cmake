# - Find FFTW3
# Find the native FFTW3 includes and library
#
# FFTW3_INCLUDES - Where to find the fftw3.h header file
# FFTW3_LIBRARIES - List of libraries available in your FFTW3 installation
# FFTW3_LIBRARIES_DIR - Directory where those libraries reside
# FFTW3_FOUND - True if FFTW3 has been successfully found
#

#if (FFTW3_INCLUDES)
  # Already in cache, be silent
#  set (FFTW3_FIND_QUIETLY TRUE)
#endif (FFTW3_INCLUDES)

find_path (FFTW3_INCLUDES fftw3.h)

find_library (FFTW3_LIBRARIES NAMES fftw3)

get_filename_component(FFTW3_LIBRARIES_DIR ${FFTW3_LIBRARIES} PATH)

# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (FFTW3 DEFAULT_MSG FFTW3_LIBRARIES FFTW3_LIBRARIES_DIR FFTW3_INCLUDES)

mark_as_advanced (FFTW3_LIBRARIES FFTW3_LIBRARIES_DIR FFTW_INCLUDES)
