# find LibEvent
# an event notification library (http://libevent.org/)
#
# Usage: 
# LIBEVENT_INCLUDE_DIRS, where to find LibEvent headers
# LIBEVENT_LIBRARIES, LibEvent libraries
# Libevent_FOUND, If false, do not try to use libevent

set(LIBEVENT_ROOT CACHE PATH "Root directory of libevent installation")
set(LibEvent_EXTRA_PREFIXES /usr/local /opt/local "$ENV{LIBEVENT_ROOT}" ${LIBEVENT_ROOT})
foreach(prefix ${LibEvent_EXTRA_PREFIXES})
  list(APPEND LibEvent_INCLUDE_PATHS "${prefix}/include")
  list(APPEND LibEvent_LIBRARIES_PATHS "${prefix}/lib")
  list(APPEND LibEvent_LIBRARIES_PATHS "{prefix}")
endforeach()

find_path(LIBEVENT_INCLUDE_DIRS event2 PATHS ${LibEvent_INCLUDE_PATHS})

if(WIN32)
  find_library(LIBEVENT_LIBRARY_CORE NAMES libevent_core.lib PATHS ${LibEvent_LIBRARIES_PATHS})
  find_library(LIBEVENT_LIBRARY_EXTRA NAMES libevent_extras.lib PATHS ${LibEvent_LIBRARIES_PATHS})
elseif(UNIX)
  find_library(LIBEVENT_LIBRARY_CORE NAMES libevent_core.a PATHS ${LibEvent_LIBRARIES_PATHS})
  find_library(LIBEVENT_LIBRARY_EXTRA NAMES libevent_extras.a PATHS ${LibEvent_LIBRARIES_PATHS})
  find_library(LIBEVENT_LIBRARY_PTHREADS NAMES libevent_pthreads.a PATHS ${LibEvent_LIBRARIES_PATHS})
endif()

if (LIBEVENT_LIBRARY_CORE AND LIBEVENT_LIBRARY_EXTRA AND LIBEVENT_INCLUDE_DIRS)
  set(Libevent_FOUND TRUE)
  list(APPEND LIBEVENT_LIBRARIES ${LIBEVENT_LIBRARY_CORE})
  list(APPEND LIBEVENT_LIBRARIES ${LIBEVENT_LIBRARY_EXTRA})
  if(UNIX)
    list(APPEND LIBEVENT_LIBRARIES ${LIBEVENT_LIBRARY_PTHREADS})
  endif()
else ()
  set(Libevent_FOUND FALSE)
endif ()

if (Libevent_FOUND)
  if (NOT Libevent_FIND_QUIETLY)
    message(STATUS "Found libevent: ${LIBEVENT_LIBRARIES}")
  endif ()
else ()
  if (LibEvent_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find libevent.")
  endif ()
  message(STATUS "libevent NOT found.")
endif ()

mark_as_advanced(
    LIBEVENT_LIBRARIES
    LIBEVENT_INCLUDE_DIRS
  )
