# find Log4cplus
#
# Usage: 
# LOG4CPLUS_INCLUDE_DIRS, where to find Log4cplus headers
# LOG4CPLUS_LIBRARIES, Log4cplus libraries
# Log4cplus_FOUND, If false, do not try to use log4cplus

set(Log4cplus_ROOT CACHE PATH "Root directory of log4cplus installation")
set(Log4cplus_EXTRA_PREFIXES /usr/local /opt/local "$ENV{Log4cplus_ROOT}" ${Log4cplus_ROOT})
foreach(prefix ${Log4cplus_EXTRA_PREFIXES})
  list(APPEND Log4cplus_INCLUDE_PATHS "${prefix}/include")  
  if(UNIX)  
    list(APPEND Log4cplus_LIBRARIES_PATHS "${prefix}/lib")
  elseif(WIN32)
    list(APPEND Log4cplus_LIBRARIES_PATHS "${prefix}/msvc10/Win32")
  endif()
endforeach()

find_path(LOG4CPLUS_INCLUDE_DIRS log4cplus PATHS ${Log4cplus_INCLUDE_PATHS})

if(WIN32)
  if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    find_library(LOG4CPLUS_LIBRARY NAMES log4cplusSD.lib PATHS ${Log4cplus_LIBRARIES_PATHS} PATH_SUFFIXES "bin.Debug")
  else()
    find_library(LOG4CPLUS_LIBRARY NAMES log4cplusS.lib PATHS ${Log4cplus_LIBRARIES_PATHS} PATH_SUFFIXES "bin.Release")
  endif()
elseif(UNIX)
  find_library(LOG4CPLUS_LIBRARY NAMES liblog4cplus.a PATHS ${Log4cplus_LIBRARIES_PATHS})
endif()

if (LOG4CPLUS_LIBRARY AND LOG4CPLUS_INCLUDE_DIRS)
  set(Log4cplus_FOUND TRUE)
  list(APPEND LOG4CPLUS_LIBRARIES ${LOG4CPLUS_LIBRARY})
else ()
  set(Log4cplus_FOUND FALSE)
endif ()

if (Log4cplus_FOUND)
  if (NOT Log4cplus_FOUND_QUIETLY)
    message(STATUS "Found log4cplus: ${LOG4CPLUS_LIBRARIES}")
  endif ()
else ()
  if (Log4cplus_FOUND_REQUIRED)
    message(FATAL_ERROR "Could NOT find log4cplus.")
  endif ()
  message(STATUS "log4cplus NOT found.")
endif ()

mark_as_advanced(
    LOG4CPLUS_LIBRARIES
    LOG4CPLUS_INCLUDE_DIRS
  )
