/*=============================================================================
#     FileName: storage.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-13 19:37:41
#      History:
=============================================================================*/

#include "storage.h"
#include <sstream>
#include <pcap.h>
#include <stdio.h>
#include "../perfMeasure.h"

using capture::PacketSlice;
using capture::Ipv4Record;

namespace storage {

void Storage::storePacketsItem(const PacketsItem& packets) {
    uint64_t offset = trace_file_handle_->writePacketsItem( packets );
    //std::cout << offset << ":===";
    //packets.printInfo();
    index->addRecord( packets.five_tuple_, offset);
}

void Storage::storeFlowRecords(const Ipv4Record& record) {
    uint64_t flowID = trace_file_handle_->writeFlowRecord( record );
    index->addRecord(record, flowID);
}

void Storage::loadPacketsItem(PacketsItem& item) {

}

uint64_t Storage::saveMatchedFlowsToFile( const OffsetsResult* result, const char *data_file,
        FILE* dump, uint64_t start_time, uint64_t end_time) const {
    FILE* fp = fopen( data_file, "rb");
    if (NULL == fp) {
        LogFile::getErrorLog()->error("cannot open file %s in file %s, line %d\n",
                data_file, __FILE__, __LINE__);
        return 0;
    }

    Ipv4Record record;
    uint64_t count = result->offsets.size();
    uint64_t prev_offset = 0;
    uint32_t record_size = Ipv4Record::getClassSize();

    //printf("offset size = %d\n", result->offsets.size());
    PerfMeasure perfms("readPackets");
    for (int i = 0; i < result->offsets.size(); ++i) {
        uint64_t offset = result->offsets[i] * record_size;
        perfms.start();
        fseek( fp, offset - prev_offset, SEEK_CUR);
        perfms.end();

        if (dump == NULL) {
            continue;
        } else {
            record.readFromFile( fp );
            record.write2file(dump);
        }

        prev_offset = ftell(fp);
    }

    perfms.print_stat();
    fclose(fp);
    return count;
}

uint64_t Storage::saveMatchedPacketsToFile( const OffsetsResult* result, const char *data_file,
        pcap_dumper_t* dump, uint64_t start_time, uint64_t end_time) const {
    FILE* fp = fopen( data_file, "rb");
    if (NULL == fp) {
        LogFile::getErrorLog()->error("cannot open file %s in file %s, line %d\n",
                data_file, __FILE__, __LINE__);
        return 0;
    }

    uint64_t count = 0;
    uint64_t prev_offset = 0;
    PacketsItem pkts;
    struct pcap_pkthdr header;

    printf("offset size = %d\n", result->offsets.size());
    PerfMeasure perfms("readPackets");
    for (int i = 0; i < result->offsets.size(); ++i) {
        uint64_t offset = result->offsets[i];
        perfms.start();
        fseek( fp, offset - prev_offset, SEEK_CUR);
        perfms.end();

        pkts.clear();
        pkts.readFromFile(fp);

        if (dump == NULL) {
            count += pkts.size(); 
        } else {
            for (std::list<PacketSlice*>::iterator it = pkts.packets_.begin();
                    it != pkts.packets_.end(); ++it ) {
                PacketSlice* packet = *it;
                if ( start_time <= packet->get_time_second() &&
                        end_time >= packet->get_time_second() ) {
                    packet->transferToPcapHeader( header );
                    pcap_dump( (uint8_t *)dump, &header, packet->data );
                    ++count;
                } 
            }
        }

        prev_offset = ftell(fp);
    }

    perfms.print_stat();
    fclose(fp);
    return count;
}



void Storage::dumpTraceAndIndex() const {
    std::vector<std::string> columns;
    std::stringstream ss;
    // add columns start_time, end_time;
    ss << trace_file_handle_->getStartTimestamp();
    columns.push_back( ss.str() );
    ss.str("");

    ss << trace_file_handle_->getCurTimestamp();
    columns.push_back( ss.str() );
    ss.str("");

    columns.push_back( FileManager::getTraceFileName(storage_name_) );
    columns.push_back( FileManager::getSipIndexName(storage_name_) );
    columns.push_back( FileManager::getDipIndexName(storage_name_) );
    columns.push_back( FileManager::getSportIndexName(storage_name_) );
    columns.push_back( FileManager::getDportIndexName(storage_name_) );
    columns.push_back( FileManager::getProtoIndexName(storage_name_) );
    columns.push_back( FileManager::getTimeIndexName(storage_name_) );

    ss << trace_file_handle_->getTotalBytes();
    columns.push_back( ss.str() );
    mysql->insertRow( storage_name_, columns );

    trace_file_handle_->newFile();
    // the trace file need to be created before calling setWriteIndexFlag()
    // since newFile() would increase the fileID in FileManager
    index->setWriteIndexFlag();
}


//end for namespace 
}
