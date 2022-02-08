/* -*- c++ -*- */
/*
 * Copyright 2021 gr-zwave author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZWAVE_PACKET_SINK_H
#define INCLUDED_ZWAVE_PACKET_SINK_H

#include <gnuradio/block.h>
#include <zwave/api.h>

namespace gr {
namespace zwave {

/*!
 * \brief <+description of block+>
 * \ingroup zwave
 *
 */
class ZWAVE_API packet_sink : virtual public gr::block
{
public:
    typedef std::shared_ptr<packet_sink> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of zwave::packet_sink.
     *
     * To avoid accidental use of raw pointers, zwave::packet_sink's
     * constructor is in a private implementation
     * class. zwave::packet_sink::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace zwave
} // namespace gr

#endif /* INCLUDED_ZWAVE_PACKET_SINK_H */
