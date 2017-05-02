/*=============================================================================
#     FileName: trace_file.h
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-13 18:59:44
#      History:
=============================================================================*/

#ifndef TRACE_FILE_H_
#define TRACE_FILE_H_

#include "../capture/packet.h"

#include <string>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

using capture::PacketsItem;
using capture::Ipv4Record;

namespace storage {

class FileManager {
public:
    static std::string  getTraceFileName( const std::string& storage_name );

    static std::string  getSipIndexName( const std::string& storage_name );

    static std::string  getDipIndexName( const std::string& storage_name );

    static std::string  getSportIndexName( const std::string& storage_name );

    static std::string  getDportIndexName( const std::string& storage_name );

    static std::string  getProtoIndexName( const std::string& storage_name );

    static std::string  getTimeIndexName( const std::string& storage_name );

    static void resetFileID(const uint32_t id) {
        pthread_mutex_lock(&mutex_);
        fileID_ = id;
        pthread_mutex_unlock(&mutex_);
    }

    static void increaseFileID() {
        pthread_mutex_lock(&mutex_);
        ++fileID_;
        pthread_mutex_unlock(&mutex_);
    }

private:
    static uint32_t fileID_;
    static pthread_mutex_t mutex_;
};

class TraceFile {
public:
    TraceFile(const std::string& name ): storage_name_(name) {
        total_bytes_ = 0;
        total_flows_ = 0;
        FileManager::increaseFileID();
        std::string file_name_ = FileManager::getTraceFileName(name);
        start_time = getCurTimestamp();
        end_time = 0;
        file_handle_ = fopen( file_name_.c_str(), "wb");
        if ( file_handle_ == NULL ) {
            fprintf( stderr, "cannot open file %s", file_name_.c_str() );
            assert( false );
        }
    }

    ~TraceFile() {
        fclose( file_handle_ );
    }

    void newFile() {
        fclose( file_handle_ );
        total_bytes_ = 0;
        total_flows_ = 0;
        FileManager::increaseFileID();
        std::string file_name_ = FileManager::getTraceFileName( storage_name_ );
        start_time = getCurTimestamp();
        end_time = 0;
        file_handle_ = fopen( file_name_.c_str(), "wb");
        if ( file_handle_ == NULL ) {
            fprintf( stderr, "cannot open file %s", file_name_.c_str() );
            assert( false );
        }
    }

    uint64_t writePacketsItem( const PacketsItem& item);

    uint64_t writeFlowRecord( const Ipv4Record& item);

    uint64_t getTotalBytes() const {
        return total_bytes_;
    }

    uint64_t getTotalFlows() const {
        return total_flows_;
    }

    uint32_t getStartTimestamp() {
        return start_time;
    }

    uint32_t getCurTimestamp() {
        return (uint32_t)time(NULL);
    }
    
    uint32_t getEndTimestamp() {
        return end_time;
    }


private:
    std::string storage_name_;
    FILE*       file_handle_;
    uint64_t    total_bytes_;
    uint64_t    total_flows_;
    uint32_t    start_time;
    uint32_t    end_time;
};


// end for namespace
}
#endif

