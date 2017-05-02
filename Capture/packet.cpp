#include "packet.h"
#include "../log_file.h"

#include <cstdio>
#include <arpa/inet.h>

namespace capture {

uint32_t PacketSlice::write2file(FILE* fp) const {
    if (NULL == fp ) {
        return 0;
    }
    uint64_t time = get_time_usec();
    fwrite( &time, sizeof(time), 1, fp);
    fwrite( &cap_length, sizeof(cap_length), 1, fp);
    fwrite( data, cap_length, 1, fp);
    return sizeof(time) + sizeof(cap_length) + cap_length;
}

void PacketSlice::readFromFile(FILE* fp) {
    if (NULL == fp ) {
        return ;
    }
    uint64_t time;
    fread( &time, sizeof(time), 1, fp);
    timestamp.tv_sec = time/1000000;
    timestamp.tv_usec = time%1000000;
    fread( &cap_length, sizeof(cap_length), 1, fp);
    data = new unsigned char[cap_length];
    fread( data, cap_length, 1, fp);
}

bool PacketSlice::equalsTo( const Ipv4Record& record ) {
    return  Ipv4Record(*this) == record;
}

// TODO: write ipv4 record by binary writing mode
uint32_t Ipv4Record::write2file(FILE * fp) const{
	//uint32_t sip = get_srcip();
	//uint32_t dip = get_dstip();
	//fprintf( fp, "%u,%u,%u,%u,%u,%u,%u,%u,",\
			//BYTE1(sip),BYTE2(sip),BYTE3(sip),BYTE4(sip),BYTE1(dip),BYTE2(dip),BYTE3(dip),BYTE4(dip));
	//fprintf( fp, "%u,%u,%u\n",get_sport(),get_dport(),get_proto() );
    //// TODO: should be fixed
    fwrite( &start_time, sizeof(uint32_t), 1, fp);
    fwrite( &srcip, sizeof(struct in_addr), 1, fp);
    fwrite( &dstip, sizeof(struct in_addr), 1, fp);
    fwrite( &srcport, sizeof(uint16_t), 1, fp);
    fwrite( &dstport, sizeof(uint16_t), 1, fp);
    fwrite( &proto, sizeof(uint8_t), 1, fp);

    return sizeof(struct in_addr) * 2 + 9;
}

uint32_t Ipv4Record::hashCode() const {
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

bool Ipv4Record::equals( PacketSlice *rec ) {
	return *this == *(Ipv4Record *)rec;
}

void Ipv4Record::display() {
	std::cout<<*this<<"\n";
}


Ipv6Record::Ipv6Record( PacketSlice* pkt ) {
}

uint32_t Ipv6Record::hashCode() {
	uint32_t ret = 0;
	for( int i = 0 ; i < 4; ++i ) {
		ret ^= srcip6.__in6_u.__u6_addr32[i];
		ret ^= dstip6.__in6_u.__u6_addr32[i];
	}
	return ret ^ srcport ^ dstport ^ proto;
}

bool Ipv6Record::equals(PacketSlice *rec ) {
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

uint32_t PacketsItem::write2file(FILE* fp) const {
    if (NULL == fp ) {
        return 0;
    } 
    size_t packets_count = this->size();
    uint32_t total_bytes = sizeof(packets_count);
    fwrite( &packets_count, sizeof(packets_count), 1, fp);
    for (std::list<PacketSlice*>::const_iterator it = packets_.begin();
            it != packets_.end(); ++it ) {
        total_bytes += (*it)->write2file( fp );
    }
    return total_bytes;
}

void PacketsItem::readFromFile(FILE* fp) {
    if (NULL == fp ) {
        return ;
    } 

    size_t packets_count = 0;
    fread( &packets_count, sizeof(packets_count), 1, fp);
    if (0 == packets_count ) {
        return ;
    }
    PacketSlice* packet = new PacketSlice();
    packet->readFromFile( fp );
    
    five_tuple_.getFiveTuples(*packet);
    packets_.push_back( packet );
    for (int i = 1; i < packets_count; ++i ) {
        packet = new PacketSlice();
        packet->readFromFile( fp );
        packets_.push_back( packet );
    }
}

void PacketsItem::printInfo() const {
    if ( packets_.empty() ) {
        //fprintf( stdout, "The item is empty!\n");
    } else {
        //std::cout << five_tuple_ << std::endl;
        LogFile::getCaptureLog()->infoStream() << five_tuple_ ;
        uint64_t start_time = five_tuple_.get_time_usec(), end_time;
        uint32_t total_bytes = 0;

        for (std::list<PacketSlice*>::const_iterator it = packets_.begin();
                it != packets_.end(); ++it ) {
            total_bytes += (*it)->cap_length;
            end_time = (*it)->get_time_usec();
        }
        LogFile::getCaptureLog()->info("[%llu, %llu, TotalPackets=%d, TotalBytes=%d]\n",
                start_time, end_time, packets_.size(), total_bytes ); 
        //fflush( stdout );
    }
}


bool PacketsItem::needToExport(uint32_t timeout) const {
    if ( packets_.empty() ) {
        return false;
    }
    struct timeval now;
    gettimeofday( &now, NULL );

    return (now.tv_sec - timeout > getLastPacketTime()) ;
    //return (now.tv_sec - timeout > getLastPacketTime()) || packets_.size() >= 20; 
}

// end for namespace
}
