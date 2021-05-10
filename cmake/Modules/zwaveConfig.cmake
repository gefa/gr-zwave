INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_ZWAVE zwave)

FIND_PATH(
    ZWAVE_INCLUDE_DIRS
    NAMES zwave/api.h
    HINTS $ENV{ZWAVE_DIR}/include
        ${PC_ZWAVE_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    ZWAVE_LIBRARIES
    NAMES gnuradio-zwave
    HINTS $ENV{ZWAVE_DIR}/lib
        ${PC_ZWAVE_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/zwaveTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ZWAVE DEFAULT_MSG ZWAVE_LIBRARIES ZWAVE_INCLUDE_DIRS)
MARK_AS_ADVANCED(ZWAVE_LIBRARIES ZWAVE_INCLUDE_DIRS)
