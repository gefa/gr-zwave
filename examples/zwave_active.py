#!/usr/bin/env python3
import os,sys
from subprocess import Popen, PIPE
import signal,time


import socket

# from scapy.data import MTU
# from scapy.packet import *
# from scapy.fields import *
# from scapy.supersocket import SuperSocket
# from scapy import sendrecv
# from scapy import main
from scapy.layers.dot15d4 import * #Dot15d4FCS, Dot15d4Cmd
# from scapy.modules.gnuradio import *

#from appdirs import user_data_dir
import socket
import struct
import atexit
import os
import sys
import time
from datetime import datetime
import errno

import random


import atexit
import errno
import os
import random
import signal
#from appdirs import user_data_dir
import socket
import struct
import sys
import time
from distutils.version import StrictVersion
from threading import Timer
from struct import pack, unpack

global name, exp

from collections import defaultdict
import time
import pyshark
from Zwave import *
import signal
from functools import partial
import zmq, pmt

# PCAP Header constants
PCAP_MAGIC = 0xA1B2C3D4
PCAP_MAJOR = 2
PCAP_MINOR = 4
PCAP_ZONE = 0
PCAP_SIG = 0
PCAP_SNAPLEN = 0xFFFF
PCAP_NETWORK = 256

# print("argv0 "+sys.argv[0])
# print("argv1 "+sys.argv[1])
#print("argv3 "+sys.argv[3])
try:
    scan_time = int(sys.argv[1])
except:
    #error("Scan time?")
    print("Assuming 1s scan time")
    scan_time = 60*5+16

def open_pcap(filename, mode='overwrite', proto='ble'):
    if mode == 'append' and os.path.exists(filename):
        pcap_fd = open(filename, 'ab')
        return pcap_fd

    pcap_fd = open(filename, 'ab')

    # Write PCAP file header
    if proto is 'lora':
        pcap_fd.write(
            pack(
                '<LHHLLLL',
                PCAP_MAGIC,
                PCAP_MAJOR,
                PCAP_MINOR,
                PCAP_ZONE,
                PCAP_SIG,
                PCAP_SNAPLEN,
                270,
            )
        )
    elif proto is 'zig':
        pcap_fd.write(
            pack(
                '<LHHLLLL',
                PCAP_MAGIC,
                PCAP_MAJOR,
                PCAP_MINOR,
                PCAP_ZONE,
                PCAP_SIG,
                PCAP_SNAPLEN,
                195,#195,#230,#215,#195,#283,
            )
        )  
    else:
        pcap_fd.write(
            pack(
                '<LHHLLLL',
                PCAP_MAGIC,
                PCAP_MAJOR,
                PCAP_MINOR,
                PCAP_ZONE,
                PCAP_SIG,
                PCAP_SNAPLEN,
                PCAP_NETWORK,
            )
        )
    return pcap_fd


# Write BLE packet to PCAP fd
def write_pcap(fd, ble_channel, ble_access_address, ble_data, proto='ble'):
    now = time.time()
    sec = int(now)
    usec = int((now - sec) * 1000000)
    ble_len = 11 #int(len(ble_data) + 14)
    ble_flags = 0x3C37

    # Write PCAP packet header
    if proto is "zig":
        ble_len = int(len(ble_data) + 0)
        fd.write(
            pack(
                '<LLLL',
                sec,
                usec,
                ble_len,
                ble_len,
                # ble_channel,
                # 0xFF,
                # 0xFF,
                # 0x00,
                # ble_access_address,
                # ble_flags,
                # ble_access_address,
            )
        )
    else:
        fd.write(
            pack(
                '<LLLLBBBBLHL',
                sec,
                usec,
                ble_len,
                ble_len,
                ble_channel,
                0xFF,
                0xFF,
                0x00,
                ble_access_address,
                ble_flags,
                ble_access_address,
            )
        )

    # Write BLE packet
    # for pkt in ble_data:
    #     fd.write(pkt)
    fd.write(ble_data)
    fd.flush()

from time import sleep
from contextlib import contextmanager
@contextmanager
def socketcontext(*args, **kwargs):
    s = socket.socket(*args, **kwargs)
    try:
        yield s
    finally:
        s.close()

ip = '127.0.0.1'
port = 52001
burst_n = 5
burst_t = 0.02

def sendburst(data, burst=burst_n, timeout=burst_t):
    for _ in range(burst):
        with socketcontext(socket.AF_INET, socket.SOCK_DGRAM) as s:
            s.sendto(data, (ip, port))
            sleep(timeout)
# Association Request is part of the Command Frame
b = Dot15d4FCS()/Dot15d4Cmd(cmd_id = 1)/Dot15d4CmdAssocReq() # AssocReq()
#b.fcf_srcaddrmode = 3 # Long addressing mode
#b.fcf_destaddrmode = 3
b.fcf_pending = 0
b.fcf_ackreq = 0
b.seqnum = 150  
b.dest_panid = 0x7022 #0x7022 # PAN to which to associate
#b.dest_addr = 0x0000 #0x00155f0040b02960 #0x6029b040005f1500 # coordinatoraddress
# Source PAN Identifier shall contain the broadcast PAN ID
#b.src_panid = 0xFFFF
b.src_addr = 0xCAFEBABECAFEBABE
b.allocate_address = 1
#b.cmd_id = 1 #command ID 1is the Association Request
# malformed packet - cant figure out how to create proper association request

BEACON_REQ = b'\x03\x08\xf2\xff\xff\xff\xff\x07\xcd\xe1' 
# source https://www.binarytides.com/receive-full-data-with-the-recv-socket-function-in-python/
def recv_timeout(the_socket,timeout=1,extend_if_busy=False):
    #make socket non blocking
    the_socket.setblocking(0)
    
    #total data partwise in an array
    total_data=[];
    data='';
    
    #beginning time
    begin=time.time()
    while 1:
        #if you got some data, then break after timeout
        if time.time()-begin > timeout: # total_data and 
            break
        
        # #if you got no data at all, wait a little longer, twice the timeout
        # elif time.time()-begin > timeout*2:
        #     break
        
        #recv something
        try:
            data = the_socket.recv(8192)
            if data:
#                write_pcap(pcap_zig, current_ble_chan, ble_access_address, data,proto='zig')
                total_data.append(data)
                print(str(data))
                #change the beginning time for measurement
                if extend_if_busy:
                  begin=time.time()
            else:
                #sleep for sometime to indicate a gap
                time.sleep(0.1)
        except:
            pass
    
    #join all parts to make final string
    return total_data #''.join(total_data)

# #get reply and print
# print recv_timeout(s)

# #Close the socket
# s.close()

if __name__ == '__main__':

    top_block_cls=Zwave
    tb = top_block_cls()
    tb.start()

    # time.sleep(1) #?
    #Dot15d4FCS()/Dot15d4Cmd()/Dot15d4CmdAssocReq() #
    
    PROBES = [ \

#        b'\x03\x08\xf2\xff\xff\xff\xff\x07\xcd\xe1',

# zwave broadcast ??
        # b'\x01\x00\x00\x00\x00\x00\x00\x00\xdf\x5c\x57\xe7\x01\x05\x01\x16\xff\x20\x00\xfa\x40\x00\x00\x00\x00\x01\x22\x01\x00\x98',
        # b'\x01\x00\x00\x00\x00\x00\x00\x00\xdf\x5c\x57\xe7\x01\x05\x02\x16\xff\x20\x00\xfa\x40\x00\x00\x00\x00\x01\x22\x01\x00\x9b',
b'\x01\x00\x00\x00\x00\x00\x00\x00\xdf\x5c\x57\xe7\x02\x41\x04\x0d\x06\x9f\x01\x0d\x13'

    ]
    for probe in PROBES:
        print(probe)
    # PKT = b'0x55'
    #soctx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    #soctx.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    socrx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    socrx.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    socrx.settimeout(1)
    #soctx.settimeout(0)
    #wireshark = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    #wireshark.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # Bind the socket to the port
    #txmt_address = ('127.0.0.1', 52001)
    server_address = ('127.0.0.1', 52002)
    socrx.bind(server_address)
    #soctx.bind(txmt_address)
    #soctx.connect(txmt_address)
    #soctx.connect(txmt_address)

    # socket_str = 'tcp://127.0.0.1:5557'
    # context = zmq.Context()
    # receiver = context.socket(zmq.PULL)
    # receiver.connect(socket_str)

    current_ble_chan=11
    ble_access_address=0xbeefbabe
    BUSY = set()
    channel = 11
    START=11
    STEP=1
    STOP=26
    STEPS = [0]#[x for x in range(START,STOP+STEP,STEP)]
    #STEPS.remove(20)
    #STEPS = [11] # 23 24 25 26
    print(STEPS)
    idx = 0
    #os.system("whoami")
    # os.system("rm zwave_act_scan.pcap") # delete previous file
    # sub = Popen(["tshark","-f",'not icmp and not tcp','-i','lo','-w','zwave_act_scan.pcap'])
#    pcap_zig = open_pcap("zig_act_scan.pcap",mode='overwrite',proto='zig')
    #while(time.time() < t_end_cap):
    #for probe in PROBES:
    for step in STEPS:
        # freq = 1000000 * (2400 + 5 * (STEPS[idx] - 10)) 
        # tb.uhd_usrp_source_0.set_center_freq(freq)
        # #print(dir(tb))
        # tb.uhd_usrp_sink_0.set_center_freq(freq)
        # #pcap_zig = open_pcap("zig_act{}_scan.pcap".format(STEPS[idx]),mode='overwrite',proto='zig')
        # print("CURRENT STEP "+str(STEPS[idx]))
        # Channel = STEPS[idx]

        #tb.uhd_usrp_source_0.set_center_freq(freq)
        for probe in PROBES:
          sendburst(bytes(probe), burst=1)
        #write_pcap(pcap_zig, current_ble_chan, ble_access_address, bytes(probe),proto='zig')

        data = recv_timeout(socrx,timeout=3,extend_if_busy=True)


        #pcap_zig.close()
        idx = idx + 1
        if idx >= len(STEPS):
            idx = 0



    #sub.terminate()
   # os.system("kill -9 {}".format(sub.pid))

    try:
        input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()

    #sockettx.close()
    #soctx.close()
    socrx.close()
