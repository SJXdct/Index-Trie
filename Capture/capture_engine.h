/*=============================================================================
#     FileName: capture_engine.h
#     Copyright: All rights reserved.
#         Desc:  Copyright (c) 2013-2014 Institute of Computing Technology, Chinese Academy of Sciences. 
#       Author: Taihua He
#        Email: hetaihua@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-01-16 09:58:58
#      History:
=============================================================================*/

#ifndef CAPTURE_ENGINE_H_
#define CAPTURE_ENGINE_H_

#include <string>
#include <pcap.h>
#include "packet.h"

namespace capture {

#define ERR_BUF_SIZE 128

class CaptureEngine {
public:
    explicit CaptureEngine(std::string &name): capture_name_(name) {
    }

    ~CaptureEngine() {
    }

    virtual int  openLiveDevice(const std::string& device) = 0;

    virtual int  openOfflineTrace(const std::string& trace) = 0;

    virtual bool getNextPacket(PacketSlice& packet) = 0;

    virtual int  setFilter(const std::string& filter) = 0;

    virtual void resetFilter() = 0;

    bool isLiveDivice() const {
        return is_live_device_;
    }

protected:
    std::string capture_name_;
    bool        is_live_device_;
};


class PcapEngine: public CaptureEngine {
public:
    explicit PcapEngine(std::string name): CaptureEngine(name) {
        pcap_handle_ = NULL;
        is_live_device_ = false;
    }

    ~PcapEngine() {
       pcap_close(pcap_handle_);
    }

    virtual int  openLiveDevice(const std::string& device);

    virtual int  openOfflineTrace(const std::string& trace);

    virtual bool getNextPacket(PacketSlice& packet);

    virtual int  setFilter(const std::string& filter); 

    virtual void resetFilter();

private:
    pcap_t *pcap_handle_;
    std::string device_;
};


}
#endif
