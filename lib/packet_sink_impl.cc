/* -*- c++ -*- */
/*
 * Copyright 2013 Airbus DS CyberSecurity.
 * Authors: Jean-Michel Picod, Arnaud Lebrun, Jonathan Christofer Demay
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "packet_sink_impl.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <gnuradio/blocks/count_bits.h>
#include <iostream>

#define ZWAVE 0x01 //Select ZWAVE

#define verbose
 #define CRC_printing

namespace gr {
  namespace zwave {

    packet_sink::sptr
    packet_sink::make()
    {
      return gnuradio::get_initial_sptr
        (new packet_sink_impl());
    }

    //"CRC" computing :
void packet_sink_impl::crc_computing(void)
{
    if(frame_struct.length >= 7){
        frame_struct.CRC= 0xFF;
        int iter;
        std::cout  << "CRC_rx : " << std::hex <<  frame_struct.frame[frame_struct.length-1]  << std::endl;
        for(iter=0; iter<(frame_struct.length-1); iter++){
            frame_struct.CRC ^= frame_struct.frame[iter];
        }
    }
    else
    {
        //FIXME : Must return an error.........
    }
}

    //Constructor
    packet_sink_impl::packet_sink_impl()
      : gr::block("packet_sink",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(0, 0, 0))
{
    state = PREAMBLE_SEARCH;
    data_shift = 0;
    frame_shift_reg = 0x00000000;
    frame_struct.CRC= 0xFF;
    message_port_register_out(pmt::mp("out"));
}
    //Destructor
    packet_sink_impl::~packet_sink_impl()
    {
    }

int packet_sink_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
{
const unsigned char *inbuf = (const unsigned char*)input_items[0];
int ninput = ninput_items[0];
int count=0;

while(count < ninput){
    switch(state){
        case PREAMBLE_SEARCH :      //Looking for preamble if found go to next state
            while (count < ninput) {
                //update the shift register
                if(inbuf[count++])  frame_shift_reg = frame_shift_reg << 1;
				else frame_shift_reg = (frame_shift_reg << 1)  | 1 ;
                //looking for preamble in the first byte
                if((frame_shift_reg & 0x00FFFFFF) == 0x005555F0){
                 #ifdef verbose
                    std::cout << "Preamble found : "<< std::hex <<  frame_shift_reg << std::endl;
                 #endif
                 frame_shift = 16;
                 state = HEADER_READING;
                 break;
                 }
            }
        break;
/*
        case SYNC_SEARCH :      // Looking for access addr
            while(count < ninput){
                //update the shift register
                if(inbuf[count++])  frame_shift_reg = (frame_shift_reg << 1) | 1;
				else frame_shift_reg = frame_shift_reg << 1;
				frame_shift++;
                //wait for 40 shift to have the acces addr in frame_shift_reg
                if(frame_shift == 16){
                        #ifdef verbose
                            std::cout << "Looking for sync : "<< std::hex <<  frame_shift_reg << std::endl;
                        #endif
                    //looking for a match in the acces addr table
                    if((frame_shift_reg & 0x000000FF) == 0x000000F0){ //return 1 if found
                        state = HEADER_READING;
                        #ifdef verbose
                            std::cout << "Sync found : "<< std::hex <<  frame_shift_reg << std::endl;
                        #endif
                        break;
                    }
                    else{
                        state=PREAMBLE_SEARCH;

                        break;
                    }
                }
            }
        break;
*/
        case HEADER_READING :
            while(count < ninput){
                //update the shift register
                if(inbuf[count++])  frame_shift_reg = frame_shift_reg << 1;
				else frame_shift_reg = (frame_shift_reg << 1)  | 1 ;
				frame_shift++;
				//Complete header data
                if(frame_shift == 48){
                    //C'est vraiment pas terrible ...............
                    frame_struct.home_id = frame_shift_reg;
                    #ifdef verbose
                    std::cout << "Home ID : "<< std::hex <<  frame_struct.home_id << std::endl;
                    #endif
                    frame_struct.frame[0] = (frame_struct.home_id & 0xFF000000) >> 24;
                    frame_struct.frame[1] = (frame_struct.home_id & 0x00FF0000) >> 16;
                    frame_struct.frame[2] = (frame_struct.home_id & 0x0000FF00) >> 8;
                    frame_struct.frame[3] = frame_struct.home_id & 0x000000FF ;
                }
                else if(frame_shift == 56){
                     frame_struct.device_id = frame_shift_reg & 0x000000FF;
                    #ifdef verbose
                    std::cout << "Device ID : "<< std::hex <<  frame_struct.device_id << std::endl;
                    #endif
                     frame_struct.frame[4] = frame_struct.device_id;
                }
                else if(frame_shift == 72){
                     frame_struct.frame_ctrl = frame_shift_reg & 0x0000FFFF;
                     frame_struct.frame[5] = (frame_struct.frame_ctrl & 0x0000FF00) >> 8;
                     frame_struct.frame[6] = frame_struct.frame_ctrl & 0x000000FF;
                     #ifdef verbose
                     std::cout << "Frame ctrl : "<< std::hex <<  frame_struct.frame_ctrl << std::endl;
                     #endif
                }
                else if(frame_shift == 80){
                     frame_struct.length = frame_shift_reg & 0x000000FF;
                    frame_struct.frame[7] = frame_struct.length;
                    #ifdef verbose
                    std::cout << "Length : "<< std::hex <<  frame_struct.length << std::endl;
                    #endif
                }
                else if(frame_shift == 88){
                     frame_struct.dest_id = frame_shift_reg & 0x000000FF;
                     #ifdef verbose
                     std::cout << "Dest ID : "<< std::hex <<  frame_struct.dest_id << std::endl;
                     #endif
                     frame_struct.frame[8] = frame_struct.dest_id;
                     data_shift=0;
                     if(frame_struct.length > 8 && frame_struct.length <= 54){  //FIXME : Bad value
                        state = READ_DATA;
                        break;
                    }
                    else
                    {
                        //error back to search preamble
                        state = PREAMBLE_SEARCH;
                        break;
                    }
                }
            }
        break;

        case READ_DATA :
             while(count < ninput){
                //update the shift register
                if(inbuf[count++])  frame_shift_reg = frame_shift_reg << 1;
				else frame_shift_reg = (frame_shift_reg << 1)  | 1 ;
				frame_shift++;
                data_shift++;
                if( (data_shift%8) == 0){ //every 8bits we store a byte
                    frame_struct.frame[8+ (data_shift/8) ] = frame_shift_reg & 0x000000FF;
                    if((data_shift/8) == (frame_struct.length)){

                        #ifdef CRC_printing
                            crc_computing();
                            std::cout  << "CRC_debug : " << std::hex <<  frame_struct.CRC  << std::endl;
                        #endif

                        pmt::pmt_t meta = pmt::make_dict();
                        buf[0] = ZWAVE;
                        buf[1] = 0x00; //Unused
                        buf[2] = 0x00; //Unused
                        buf[3] = 0x00; //Unused
                        buf[4] = 0x00; //Unused
                        buf[5] = 0x00; //Unused
                        buf[6] = 0x00; //Unused
                        buf[7] = 0x00; //Unused
                        std::memcpy(buf+8, frame_struct.frame , (frame_struct.length ) );
                        pmt::pmt_t payload = pmt::make_blob(buf, (frame_struct.length+8 ));
                        // std::memcpy(buf+0, frame_struct.frame , (frame_struct.length ) );
                        // pmt::pmt_t payload = pmt::make_blob(buf, (frame_struct.length+0 ));
                        message_port_pub(pmt::mp("out"), pmt::cons(meta, payload));

                    }
                    else if ((data_shift/8) == (frame_struct.length+1)){
                        // print entire packet, including received crc
                        printf("Packet: ");
                        for(int iter=0; iter<(frame_struct.length+1-1); iter++){
                            //std::cout  << "Packet : " << std::hex <<  frame_struct.frame[iter]  << std::endl;
                            printf("%02X ",frame_struct.frame[iter]);
                            //frame_struct.CRC ^= frame_struct.frame[iter];
                        }
                        printf("%02X ",frame_struct.CRC);
                        printf("\n");
                        if (frame_struct.CRC == frame_struct.frame[frame_struct.length-1]){
                            printf("CRC OK\n");
                        }else{
                            printf("CRC BAD\n");
                        }

                        data_shift=0;
                        state = PREAMBLE_SEARCH;
#ifdef verbose_state
    cout << "state = 0" << endl;
#endif
                        break;
                    }
                }
             }
        break;
    }

}

consume(0, ninput_items[0]);
// Tell runtime system how many output items we produced => 0 cause we use  PDU.
return 0;
}




  } /* namespace Zwave */
} /* namespace gr */

