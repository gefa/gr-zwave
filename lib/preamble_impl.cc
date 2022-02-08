/* -*- c++ -*- */
/*
 * Copyright 2021 gr-zwave author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "preamble_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>
#include <gnuradio/block_detail.h>
#define ZWAVE 0x01
#define PREAMBLE_SIZE 25 // <<<< of preamble table size
namespace gr {
namespace zwave {

#pragma message("set the following appropriately and remove this warning")
using input_type = float;
#pragma message("set the following appropriately and remove this warning")
using output_type = float;
preamble::sptr preamble::make() { return gnuradio::make_block_sptr<preamble_impl>(); }


/*
 * The private constructor
 */
preamble_impl::preamble_impl()
    : gr::block("preamble",
                gr::io_signature::make(
                    0 /* min inputs */, 0 /* max inputs */, 0*sizeof(input_type)),
                gr::io_signature::make(
                    0 /* min outputs */, 0 /*max outputs */, 0*sizeof(output_type)))
{
    int jojo=0;
    for(;jojo<PREAMBLE_SIZE;jojo++) preamble[jojo]=0x55;
    preamble[jojo]=0xF0;

    //Queue stuff
    message_port_register_out(pmt::mp("out"));
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), boost::bind(&preamble_impl::general_work, this, _1));
}

/*
 * Our virtual destructor.
 */
preamble_impl::~preamble_impl() {}

//void preamble_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
//{
//#pragma message( \
//    "implement a forecast that fills in how many items on each input you need to produce noutput_items and remove this warning")
//    /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
//}

void preamble_impl::general_work (pmt::pmt_t msg){

	if(pmt::is_eof_object(msg)) {
		message_port_pub(pmt::mp("out"), pmt::PMT_EOF);
		detail().get()->set_done(true);
		return;
	}

	assert(pmt::is_pair(msg));
	pmt::pmt_t blob = pmt::cdr(msg);

	size_t data_len = pmt::blob_length(blob);
	assert(data_len);
	assert(data_len < 256 - 1);
	//Check if Zwave frame
    char temp[256];
    std::memcpy(temp, pmt::blob_data(blob), data_len);
    if(temp[0] == ZWAVE){

    std::memcpy(preamble + 1 + PREAMBLE_SIZE, ((const char*)pmt::blob_data(blob))+8, data_len-8); // blob_data+1 to remove the 2 byte header

    //2 byte added at the end of the packet
    preamble[data_len+1+PREAMBLE_SIZE-8] = 0xAA;


//    for(int toto=0;toto< (PREAMBLE_SIZE+data_len-8+2);toto++)  preamble[toto] ^=  0xff;

	pmt::pmt_t packet = pmt::make_blob(preamble, data_len-8 + 1+1+PREAMBLE_SIZE); //padding of 1 octets

	message_port_pub(pmt::mp("out"), pmt::cons(pmt::PMT_NIL, packet));
	}
    }

} /* namespace zwave */
} /* namespace gr */
