/*=============================================================================
#     FileName: capture_engine.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Taihua He
#        Email: hetaihua@ict.ac.com
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-01-16 10:40:42
#      History:
=============================================================================*/

#include "capture_engine.h"
#include <stdlib.h>
#include <string.h>

namespace capture {

int PcapEngine::openLiveDevice(const std::string& device) {
    char err_buffer[ERR_BUF_SIZE];
    is_live_device_ = true;
    pcap_handle_ = pcap_open_live(device.c_str(), 65536, 1, 1000, err_buffer);
    if (NULL == pcap_handle_) {
       // TODO: print error log 
       fprintf(stderr, "Error happens in pcap opening: %s\n", err_buffer);
       return -1;
    }
    return 0;
}

int PcapEngine::openOfflineTrace(const std::string& trace) {
    char err_buffer[ERR_BUF_SIZE];
    is_live_device_ = true;
    pcap_handle_ = pcap_open_offline(trace.c_str(), err_buffer);
    if (NULL == pcap_handle_) {
       // TODO: print error log 
       fprintf(stderr, "Error happens in pcap opening: %s\n", err_buffer);
       return -1;
    }
    return 0;
}

bool PcapEngine::getNextPacket(PacketSlice& packet) {
    if (NULL == pcap_handle_) {
       fprintf(stderr, "No device was opened!\n");
       return false;
    }

    struct pcap_pkthdr header;
    const unsigned char* data = pcap_next(pcap_handle_, &header);
    /*
    if (data != NULL && header.caplen != 0) {
        packet.data = new unsigned char[header.caplen];
        memcpy( packet.data, data, header.caplen );
    }
    */

    packet.data = const_cast<unsigned char*>(data);
    packet.timestamp = header.ts;
    packet.length = header.len;
    packet.cap_length = header.caplen;

    return data != NULL && packet.cap_length != 0;
}

int PcapEngine::setFilter(const std::string& filter) {
    if (NULL == pcap_handle_) {
       fprintf(stderr, "No device was opened!\n");
       return -1;
    }

    struct bpf_program bpf;
    pcap_if_t *alldevs = NULL;
    pcap_if_t *d = NULL;
    char err_buffer[ERR_BUF_SIZE];
    bpf_u_int32 mask;		/* The netmask of our sniffing device */
    bpf_u_int32 net;		/* The IP of our sniffing device */

    if (pcap_lookupnet(device_.c_str(), &net, &mask, err_buffer) == -1) {
        fprintf(stderr, "Can't get netmask for device %s\n", device_.c_str());
        net = 0;
        mask = 0;
    }

    if (pcap_compile(pcap_handle_, &bpf, filter.c_str(), 1, mask) < 0) {
        fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n");
        return -1;
    }

    if (pcap_setfilter(pcap_handle_, &bpf) < 0) {
        fprintf(stderr,"\nError setting the filter.\n");
        return -1;
    }

    return 0;
}

void PcapEngine::resetFilter() {
    setFilter("");
}

}
