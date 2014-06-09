# - Find LibEv
# This module finds an installed libev package.
#
# It sets the following variables:
#  LIBEV_FOUND       - Set to false, or undefined, if libev isn't found.
#  LIBEV_INCLUDE_DIR - The libev include directory.
#  LIBEV_LIBRARY     - The libev library to link against.

FIND_PATH(LIBEV_INCLUDE_DIR ev.h)
FIND_LIBRARY(LIBEV_LIBRARY NAMES ev)

IF (LIBEV_INCLUDE_DIR AND LIBEV_LIBRARY)
	SET(LIBEV_FOUND TRUE)
ENDIF (LIBEV_INCLUDE_DIR AND LIBEV_LIBRARY)

IF (LIBEV_FOUND)

	# show which libev was found only if not quiet
	IF (NOT LIBEV_FIND_QUIETLY)
		MESSAGE(STATUS "Found libev: ${LIBEV_LIBRARY}")
	ENDIF (NOT LIBEV_FIND_QUIETLY)

ELSE (LIBEV_FOUND)

	# fatal error if libev is required but not found
	IF (LIBEV_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find libev")
	ENDIF (LIBEV_FIND_REQUIRED)

ENDIF (LIBEV_FOUND)