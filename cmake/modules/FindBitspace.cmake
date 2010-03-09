# - Try to find libbitspace
# Once done this will define
#
#  BITSPACE_FOUND - system has libbitspace
#  BITSPACE_INCLUDE_DIR - the libbitspace include directory
#  BITSPACE_LIBRARY - Link this to use libbitspace
#

# Copyright (C) 2010, Casey Link <unnamedrambler@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(BITSPACE_INCLUDE_DIR AND BITSPACE_LIBRARY)

  # in cache already
   set(BITSPACE_FOUND TRUE)

else(BITSPACE_INCLUDE_DIR AND BITSPACE_LIBRARY)
  find_path(BITSPACE_INCLUDE_DIR NAMES ws.h
    HINTS
    /home/ramblurr/src/bitspace/libbitspace
  )
  find_library(BITSPACE_LIBRARY NAMES bitspace
    HINTS
    /home/ramblurr/src/bitspace/libbitspace
  )
  if(BITSPACE_INCLUDE_DIR AND BITSPACE_LIBRARY)
    set(BITSPACE_FOUND TRUE)
    message(STATUS "Found libbitspace: ${BITSPACE_INCLUDE_DIR}, ${BITSPACE_LIBRARY}")
  else(BITSPACE_INCLUDE_DIR AND BITSPACE_LIBRARY)
    set(BITSPACE_FOUND FALSE)
    if (Bitspace_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find required package LibBitspace")
    endif(Bitspace_FIND_REQUIRED)
  endif(BITSPACE_INCLUDE_DIR AND BITSPACE_LIBRARY)
endif(BITSPACE_INCLUDE_DIR AND BITSPACE_LIBRARY)
