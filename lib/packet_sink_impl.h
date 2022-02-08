/* -*- c++ -*- */
/*
 * Copyright 2021 gr-zwave author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ZWAVE_PACKET_SINK_IMPL_H
#define INCLUDED_ZWAVE_PACKET_SINK_IMPL_H

#include <zwave/packet_sink.h>

#define MAX_PDU_SIZE 47 //frame size not pdu

namespace gr {
namespace zwave {

class packet_sink_impl : public packet_sink
{
private:
      // Nothing to declare in this block.
    	enum {PREAMBLE_SEARCH, SYNC_SEARCH, HEADER_READING, READ_DATA} state;
        unsigned int frame_shift_reg;
        unsigned int frame_shift;
        unsigned int data_shift;
        struct s_frame_struct
        {
            unsigned int home_id;
            unsigned int length;
            unsigned int device_id;
            unsigned int frame_ctrl;
            unsigned int dest_id;
            unsigned char frame[MAX_PDU_SIZE+1];
            unsigned int CRC;
        };
        struct s_frame_struct  frame_struct;
        unsigned char buf[512];

public:
    packet_sink_impl();
    ~packet_sink_impl();
    void crc_computing(void);  
    // Where all the action really happens
    //void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace zwave
} // namespace gr

#endif /* INCLUDED_ZWAVE_PACKET_SINK_IMPL_H */
