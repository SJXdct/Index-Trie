#include "record.h"
#include <cstdio>
#include <arpa/inet.h>

using libtrace::IPV4HEADER;
using libtrace::IPV6HEADER;

Record::Record( PACKET* pkt ): packet( pkt ) {
	assert( pkt );
	tv = trace_get_timeval( packet );
	file_offset = trace_get_capture_length( packet );;
}

void Record::set_file_name(std::string s ){
	file_name = s;
}

Ipv4Record::Ipv4Record( PACKET* pkt ): Record( pkt ) {
	uint16_t ethertype;
	uint32_t remaining;
	IPV4HEADER *ipheader = (IPV4HEADER*)trace_get_layer3( packet , &ethertype, &remaining );
	assert( ethertype == TRACE_ETHERTYPE_IP );
	assert( ipheader );
	srcip = ipheader->ip_src;
	dstip = ipheader->ip_dst;
	srcport = trace_get_source_port(packet); 
 	dstport = trace_get_destination_port( packet );
	void *tmp = trace_get_transport(packet, &proto, &remaining );
	if( tmp == NULL || remaining == 0 ) proto = 0;
}

void Ipv4Record::write2file(FILE * fp){
	uint32_t sip = get_srcip();
	uint32_t dip = get_dstip();
	fprintf( fp, "%u,%u,%u,%u,%u,%u,%u,%u,",\
			BYTE1(sip),BYTE2(sip),BYTE3(sip),BYTE4(sip),BYTE1(dip),BYTE2(dip),BYTE3(dip),BYTE4(dip));
	fprintf( fp, "%u,%u,%u\n",get_sport(),get_dport(),get_proto() );
}

uint32_t Ipv4Record::hash() {
	return srcip.s_addr ^ dstip.s_addr ^ srcport ^ dstport ^ proto;
}

/*
std::string Ipv4Record::get_sql_string() {
	char str[512];
	uint32_t src = ntohl(srcip.s_addr);
	uint32_t dst = ntohl(dstip.s_addr);

	snprintf(str, 512, "(%ld,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d)",\
			tv.tv_sec,BYTE1(src),BYTE2(src),BYTE3(src),BYTE4(src),\
			BYTE1(dst),BYTE2(dst),BYTE3(dst),BYTE4(dst),\
			srcport, dstport, proto, file_name.c_str(), packets_begin, packets_end );
	return  (std::string)str;
}
*/

bool Ipv4Record::equals( Record *rec ) {
	return *this == *(Ipv4Record *)rec;
}

void Ipv4Record::display() {
	std::cout<<*this<<"\n";
}


Ipv6Record::Ipv6Record( PACKET* pkt ): Record( pkt ) {
	uint16_t ethertype;
	uint32_t remaining;
	IPV6HEADER* ipv6 = (IPV6HEADER*)trace_get_layer3( packet , &ethertype, &remaining );
	assert( ipv6 );
	assert( ethertype == TRACE_ETHERTYPE_IPV6 );
	dstip6 = ipv6->ip_dst;
	srcip6 = ipv6->ip_src;
	proto = ipv6->nxt;
	srcport = trace_get_source_port(packet); 
 	dstport = trace_get_destination_port( packet );
}

uint32_t Ipv6Record::hash() {
	uint32_t ret = 0;
	for( int i = 0 ; i < 4; ++i ) {
		ret ^= srcip6.__in6_u.__u6_addr32[i];
		ret ^= dstip6.__in6_u.__u6_addr32[i];
	}
	return ret ^ srcport ^ dstport ^ proto;
}

bool Ipv6Record::equals( Record *rec ) {
	return *this == *(Ipv6Record *)rec;
}

void Ipv6Record::display() {
	std::cout<<*this<<"\n";
}


std::string Ipv6Record::get_sql_string() {
	/*
	char str[512];
	uint16_t *src = srcip6.__in6_u.__u6_addr16;
	uint16_t *dst = dstip6.__in6_u.__u6_addr16;

	snprintf(str, 512, "(%ld,%d,%d,%d,%d,%d,%d,%d,%d,\
			%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d,%d)",\
			tv.tv_sec,src[0],src[1],src[2],src[3],src[4],src[5],src[6],src[7],\
			dst[0],dst[1],dst[2],dst[3],dst[4],dst[5],dst[6],dst[7],\
			srcport, dstport, proto, file_name.c_str(), packets_begin, packets_end );
	return  (std::string)str;
	*/
	return "";
}

