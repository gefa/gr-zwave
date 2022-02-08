/* -*- c++ -*- */
/*
 * Copyright 2021 gr-zwave author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZWAVE_PREAMBLE_IMPL_H
#define INCLUDED_ZWAVE_PREAMBLE_IMPL_H

#include <zwave/preamble.h>

namespace gr {
namespace zwave {

class preamble_impl : public preamble
{
private:
    // Nothing to declare in this block.
        char preamble[256];
public:
    preamble_impl();
    ~preamble_impl();

      void general_work(pmt::pmt_t msg);
};

} // namespace zwave
} // namespace gr

#endif /* INCLUDED_ZWAVE_PREAMBLE_IMPL_H */
