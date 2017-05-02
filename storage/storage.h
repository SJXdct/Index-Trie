/*=============================================================================
#     FileName: storage.h
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-13 17:06:26
#      History:
=============================================================================*/

#ifndef STORAGE_H_
#define STORAGE_H_

#include "../capture/packet.h"
#include "trace_file.h"
#include "index.h"
#include "../mysql_driver.h"
#include "../log_file.h"

#include <string>
#include <pcap.h>

using capture::PacketsItem;
using capture::Ipv4Record;

namespace storage {

// TODO: thread-safe
class Storage {

public:
    explicit Storage( const std::string& name): storage_name_(name) {
        //mysql instance must be created at the most begining;
        mysql = new MysqlDriver();
        if (!mysql->connect()) {
            //fprintf( stderr, "can not connect to database\n" );
            LogFile::getErrorLog()->error("cannot connect to database\n");
        }
        if (!mysql->createTable(name)) {
            LogFile::getErrorLog()->error("cannot create table\n");
            //fprintf( stderr, "can not create table\n" );
        }
        FileManager::resetFileID( mysql->getTotalRows(name) );
        // Trace File must be create before indexes created
        trace_file_handle_ = new TraceFile( name );
        index = new Index( name );
    }

    ~Storage() {
        delete trace_file_handle_;
        delete index;
        delete mysql;
    }

    void storePacketsItem(const PacketsItem& packets);

    void storeFlowRecords(const Ipv4Record& record);

    void loadPacketsItem(PacketsItem& item);

    uint64_t saveMatchedPacketsToFile( const OffsetsResult* result, const char *file_name,
            pcap_dumper_t *dump, uint64_t start_time, uint64_t end_time) const;

    uint64_t saveMatchedFlowsToFile( const OffsetsResult* result, const char *file_name,
            FILE *dump, uint64_t start_time, uint64_t end_time) const;

    uint64_t currentFileSize() const {
        return trace_file_handle_->getTotalBytes();
    }

    uint64_t currentFlowsNumber() const {
        return trace_file_handle_->getTotalFlows();
    }

    std::string getStorageName() const {
        return storage_name_;
    }

    void dumpTraceAndIndex() const ;

    Index* getIndex() const {
        return index;
    }

    uint64_t getStartTimestamp() const {
        return trace_file_handle_->getStartTimestamp();
    }

    bool getMatchedTraces( uint64_t begin, uint64_t end,
            std::vector< std::vector<std::string> >& result ) const {
       return mysql->getMatchedRows( storage_name_, begin, end, result);
    }


    OffsetsResult* getOffsets(const std::string& index_name, INDEXCODE code, 
            uint32_t begin, uint32_t end)const {
        return index->queryFromFile(code, index_name.c_str(), begin, end);
    }

    OffsetsResult* getMemSipOffsets(uint32_t begin, uint32_t end) const {
        return index->queryFromMem(SRCIP, begin, end);
    }

    OffsetsResult* getMemDipOffsets(uint32_t begin, uint32_t end) const {
        return index->queryFromMem(DSTIP, begin, end);
    }

    OffsetsResult* getMemSportOffsets(uint32_t begin, uint32_t end) const {
        return index->queryFromMem(SPORT, begin, end);
    }

    OffsetsResult* getMemDportOffsets(uint32_t begin, uint32_t end) const {
        return index->queryFromMem(DPORT, begin, end);
    }

    OffsetsResult* getMemProtoOffsets(uint32_t begin, uint32_t end) const {
        return index->queryFromMem(PROTO, begin, end);
    }

    OffsetsResult* getMemTimeOffsets(uint32_t begin, uint32_t end) const {
        return index->queryFromMem(TIME, begin, end);
    }


private:
    std::string storage_name_;
    TraceFile*  trace_file_handle_;
    Index*      index;
    MysqlDriver* mysql;
};

// end for namespace 
}
#endif
