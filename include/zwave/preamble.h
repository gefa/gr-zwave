/* -*- c++ -*- */
/*
 * Copyright 2021 gr-zwave author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZWAVE_PREAMBLE_H
#define INCLUDED_ZWAVE_PREAMBLE_H

#include <gnuradio/block.h>
#include <zwave/api.h>

namespace gr {
namespace zwave {

/*!
 * \brief <+description of block+>
 * \ingroup zwave
 *
 */
class ZWAVE_API preamble : virtual public gr::block
{
public:
    typedef std::shared_ptr<preamble> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of zwave::preamble.
     *
     * To avoid accidental use of raw pointers, zwave::preamble's
     * constructor is in a private implementation
     * class. zwave::preamble::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace zwave
} // namespace gr

#endif /* INCLUDED_ZWAVE_PREAMBLE_H */
