/*=============================================================================
#     FileName: query.h
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-17 16:54:29
#      History:
=============================================================================*/

#ifndef QUERY_H_
#define QUERY_H_


#include "../storage/storage.h"

#include <string>
#include <stdint.h>
#include <vector>

using namespace storage;

//namespace query {

enum QUERYTYPE {
    HELP = 0,
    SAVE ,
    COUNT ,
    SHOW ,
};

struct QueryRequest{
    std::string sip_index_file;
    std::string dip_index_file;
    std::string sport_index_file;
    std::string dport_index_file;
    std::string proto_index_file;
    std::string time_index_file;
    std::string data_file_name;
    std::string save_file_name;
    uint64_t    start_time;
    uint64_t    end_time;
    QUERYTYPE   query_type;
};

class Query {
public:
    QueryRequest*  query_request;

    Query(const storage::Storage* storage_ ): storage(storage_) {
        query_request = new QueryRequest();
    }

    ~Query() {
        if (query_request) delete query_request;
    }

    void processCommand(const char *command );

    OffsetsResult* getOffsets(const std::string& index_name, INDEXCODE code, 
            uint32_t begin, uint32_t end) {
        //fprintf( stdout, "begin = %u, end = %u\n", begin, end);
        return storage->getOffsets(index_name, code, begin, end);
        //return index->queryFromFile(code, index_name.c_str(), begin, end);
    }

    OffsetsResult* getMemSipOffsets(uint32_t begin, uint32_t end) {
        //return index->queryFromMem(SRCIP, begin, end);
        return storage->getMemSipOffsets(begin, end);
    }

    OffsetsResult* getMemDipOffsets(uint32_t begin, uint32_t end) {
        //return index->queryFromMem(DSTIP, begin, end);
        return storage->getMemDipOffsets(begin, end);
    }

    OffsetsResult* getMemSportOffsets(uint32_t begin, uint32_t end) {
        return storage->getMemSportOffsets(begin, end);
        //return index->queryFromMem(SPORT, begin, end);
    }

    OffsetsResult* getMemDportOffsets(uint32_t begin, uint32_t end) {
        return storage->getMemDportOffsets(begin, end);
        //return index->queryFromMem(DPORT, begin, end);
    }

    OffsetsResult* getMemProtoOffsets(uint32_t begin, uint32_t end) {
        return storage->getMemProtoOffsets(begin, end);
        //return index->queryFromMem(PROTO, begin, end);
    }

    OffsetsResult* getMemTimeOffsets(uint32_t begin, uint32_t end) {
        return storage->getMemTimeOffsets(begin, end);
        //return index->queryFromMem(PROTO, begin, end);
    }



private:
    const storage::Storage* storage;
};

//end for namespace 
//}
#endif
