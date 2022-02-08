# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT zwave_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT zwave_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################
list(APPEND zwave_python_files
    packet_sink_python.cc
    preamble_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_OOT(zwave
   ../..
   gr::zwave
   "${zwave_python_files}")

install(TARGETS zwave_python DESTINATION ${GR_PYTHON_DIR}/zwave COMPONENT pythonapi)
