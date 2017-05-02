/*=============================================================================
#     FileName: packet.h
#     Copyright: All rights reserved.
#         Desc:  Copyright (c) 2013-2014 Institute of Computing Technology, Chinese Academy of Sciences. 
#       Author: Taihua He
#        Email: hetaihua@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-01-16 14:33:40
#      History:
=============================================================================*/

#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#ifndef PACKET_H_
#define PACKET_H_

#include <sys/time.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#ifdef linux
#define __FAVOR_BSD
#endif
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <pcap.h>

//#include <stdio.h>
#include <iostream>
#include <sstream>
#include <list>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

namespace capture {

class Ipv4Record;

class PacketSlice {
public:
    unsigned char *data;
    struct timeval timestamp;
    uint16_t length;
    uint16_t cap_length;

public:

    PacketSlice() {
        data = NULL;
        length = 0;
        cap_length = 0;
        gettimeofday(&timestamp, NULL);
    }

    virtual ~PacketSlice() {
        delete data;
    }

    PacketSlice( const PacketSlice& packet) {
        //fprintf(stdout, "PacketSlice construction is called!\n");
        length = packet.length;
        cap_length = packet.cap_length;
        timestamp = packet.timestamp;
        if (cap_length > 0) {
            data = new unsigned char[cap_length];
            memcpy( data, packet.data, cap_length);
        } else data = NULL;
    }
    
    //PacketSlice( unsigned char *_data, struct timeval& tv, int len)
        //: data(_data), timestamp( tv ), length(len) {}

    friend std::ostream& operator<<(std::ostream& out, const PacketSlice& packet) {
        out << "[length = " << packet.length << ", cap_length = " << packet.cap_length
            <<  "] " << std::endl;
        return out;
    }

	timeval get_time_tv() const {
		return timestamp;
	}

	__time_t get_time_second() const {
		return 	timestamp.tv_sec;
	}

    uint64_t get_time_usec() const {
        return 1ll * 1e6 * timestamp.tv_sec + timestamp.tv_usec;
    }

    virtual uint32_t hashCode() const {
        return 0;
    }

    virtual bool equalsTo(const Ipv4Record& record);  

    const unsigned char* getConstData() const {
        return data;
    }

    uint32_t write2file(FILE* fp) const ;

    void readFromFile(FILE* fp);

    void transferToPcapHeader( struct pcap_pkthdr& hdr) const {
        hdr.ts = timestamp;
        hdr.caplen = cap_length;
        //hdr.len = length;
        hdr.len = cap_length;
    }

};

#ifdef linux
struct icmphdr {
	uint8_t type;                /* message type */
	uint8_t code;                /* type sub-code */
	uint16_t checksum;
	union
	{
		struct {
			uint16_t id;
			uint16_t sequence;
		}
		echo;                     /* echo datagram */
		uint32_t   gateway;        /* gateway address */
		struct {
			uint16_t unused;
			uint16_t mtu;
		}
		frag;                     /* path mtu discovery */
	} un;
};
#else
#include <netinet/ip_icmp.h>
#endif

/* locate header positions and structures */

/* this is in net/ethernet.h */
/* #define ETHER_HDR_LEN		sizeof(struct ether_header) */

#define ETHERNET(packet)    ((struct ether_header *)packet)
#define IP(packet)          ((struct ip *)(packet+ETHER_HDR_LEN))


#define IP_HDR_LEN(packet)	(IP(packet)->ip_hl*4)

#define TCP(packet)         ((struct tcphdr *)((char*)IP(packet)+IP_HDR_LEN(packet)))
#define UDP(packet)         ((struct udphdr *)((char*)IP(packet)+IP_HDR_LEN(packet)))


#define  BYTE1(x)	(uint8_t)(((x)&0xff000000)>>24)
#define  BYTE2(x)	(uint8_t)(((x)&0x00ff0000)>>16)
#define  BYTE3(x)	(uint8_t)(((x)&0x0000ff00)>>8)
#define  BYTE4(x)	(uint8_t)((x)&0x000000ff)


class Ipv4Record: public PacketSlice {
private:
	struct in_addr 	srcip;		/** Source Address */
	struct in_addr 	dstip;		/** DST Address */
	uint16_t		proto;		/** protocal ID */
	uint16_t		srcport;	/** src port, 0 for icmp, igmp */
	uint16_t		dstport;	/** dst port, 0 for icmp, igmp */
    uint32_t        start_time;
    uint32_t        end_time;

public:
    Ipv4Record() {}

	Ipv4Record( PacketSlice* pkt ): PacketSlice(*pkt) {
        getFiveTuples( *pkt );
    }

    Ipv4Record( const PacketSlice& pkt ): PacketSlice(pkt) {
        getFiveTuples( pkt );
    }


	~Ipv4Record() {
	}

    static uint32_t getClassSize()  {
        return 2 * sizeof(struct in_addr) + 9;
    }

    void getFiveTuples( const PacketSlice& pkt ) {
        srcip = IP(pkt.getConstData())->ip_src;
        dstip = IP(pkt.getConstData())->ip_dst;
        proto = IP(pkt.getConstData())->ip_p;
        if (proto == IPPROTO_TCP) {
            srcport = TCP(pkt.getConstData())->th_sport;
            dstport = TCP(pkt.getConstData())->th_dport;
        } else if (proto == IPPROTO_UDP) {
            srcport = UDP(pkt.getConstData())->uh_sport;
            dstport = UDP(pkt.getConstData())->uh_dport;
        } else {
            srcport = dstport = 0;
        }
    }

    void readFromLine( const std::string& line) {
        std::string str;
        std::stringstream ss;
        ss << line;
        ss >> str;
        start_time = atoi( str.c_str() );
        ss >> str;
        inet_aton( str.c_str(), &srcip );
        ss >> str;
        inet_aton( str.c_str(), &dstip );
        ss >> srcport;
        ss >> dstport;
        ss >> proto;
    }

    uint32_t get_srcip() const {
		return ntohl( srcip.s_addr );	
	}

	uint32_t get_dstip() const {
        return ntohl( dstip.s_addr );
	}

	uint16_t get_sport() const {
		return ntohs( srcport );
	}

	uint16_t get_dport() const {
		return ntohs( dstport );
	}
	
	uint8_t get_proto() const {
        //printf("proto = %d\n", (uint8_t)proto);
        //std::cout << proto << std::endl;
		return (uint8_t)proto;
	}

    uint32_t get_time() const {
        return start_time;
    }

	uint32_t hashCode() const;

    bool equalsTo( const Ipv4Record& record ) {
        return *this == record;
    }

	bool equals( PacketSlice * );

	uint32_t write2file(FILE *fp) const;

	void display();

	bool operator == ( const Ipv4Record &rec ) const {
		return srcip.s_addr == rec.srcip.s_addr && \
				dstip.s_addr == rec.dstip.s_addr && \
				proto == rec.proto && \
				srcport == rec.srcport && dstport == rec.dstport;
	}

	friend std::ostream& operator << ( std::ostream &out, const Ipv4Record& rec ) {
        out<<  rec.start_time <<", ";
		out<<	inet_ntoa(rec.srcip)<<", " ;
        flush(out);
        out <<	inet_ntoa(rec.dstip)<<", " \
			<<	rec.srcport<<", "\
			<< 	rec.dstport<<", "\
			<< 	(uint8_t)rec.proto <<", "\
			<< 	rec.proto;
		flush(out);
		return out;
	}
};


class Ipv6Record: public PacketSlice {
private:
	struct in6_addr  srcip6;
	struct in6_addr  dstip6;
	uint8_t			proto;		/** next header or protocal ID */
	uint16_t		srcport;	/** src port, 0 for icmp, igmp */
	uint16_t		dstport;	/** dst port, 0 for icmp, igmp */

public:
	Ipv6Record( PacketSlice* pkt );

	~Ipv6Record() {
	}

	uint32_t hashCode();

	std::string get_sql_string();

	bool equals(PacketSlice * );

	void display();

	bool operator == ( const Ipv6Record &rec ) const {
		for( int i = 0 ; i < 4; ++i )
			if( srcip6.__in6_u.__u6_addr32[i] != rec.srcip6.__in6_u.__u6_addr32[i] )
				return false;

		for( int i = 0 ; i < 4;++i )
			if( dstip6.__in6_u.__u6_addr32[i] != rec.dstip6.__in6_u.__u6_addr32[i] )
				return false;

		return	proto == rec.proto && \
				srcport == rec.srcport && dstport == rec.dstport;
	}

	friend std::ostream& operator << ( std::ostream &out, const Ipv6Record& rec ) {
		char buf[128];
		inet_ntop( AF_INET6, (void*)&(rec.srcip6), buf, 128);
		out<< buf << "|";
		inet_ntop( AF_INET6, (void*)&(rec.dstip6), buf, 128);
		out<< buf << "|" \
			<<	rec.srcport<<"|"\
			<< 	rec.dstport<<"|"\
			<< 	(uint8_t)rec.proto;
		return out;
	}

};


struct PacketsItem {
    Ipv4Record five_tuple_;
    std::list<PacketSlice*> packets_;

    PacketsItem() {
    }

    ~PacketsItem() {
        //fprintf(stdout, "PacketsItem Deconstruction is called!\n");
        for (std::list<PacketSlice*>::iterator it = packets_.begin();
                it != packets_.end(); ++it ) {
            delete *it;
        }
    }

    void addPacket( PacketSlice* packet) {
        if (packets_.empty()) {
            five_tuple_.getFiveTuples( *packet );
        }
        packets_.push_back( packet );
    }

    void addPacketInFront( PacketSlice* packet ) {
        if (packets_.empty()) {
            five_tuple_.getFiveTuples( *packet );
        }
        packets_.push_front( packet );
    }

    size_t size() const {
        return packets_.size();
    }

    uint32_t write2file( FILE* fp ) const;

    void readFromFile( FILE* fp) ;

    void printInfo() const;

    bool needToExport(uint32_t timeout = 60) const;

    uint32_t getLastPacketTime() const {
        assert( !packets_.empty() );
        return (packets_.back())->get_time_second();
    }

    void clear() {
        for (std::list<PacketSlice*>::iterator it = packets_.begin();
                it != packets_.end(); ++it ) {
            delete *it;
        }
        packets_.clear();
    }
};


}

#endif
