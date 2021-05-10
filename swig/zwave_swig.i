/* -*- c++ -*- */

#define ZWAVE_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "zwave_swig_doc.i"

%{
#include "zwave/preamble.h"
#include "zwave/packet_sink.h"
%}

%include "zwave/preamble.h"
GR_SWIG_BLOCK_MAGIC2(zwave, preamble);
%include "zwave/packet_sink.h"
GR_SWIG_BLOCK_MAGIC2(zwave, packet_sink);
