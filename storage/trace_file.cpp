/*=============================================================================
#     FileName: trace_file.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-13 19:18:40
#      History:
=============================================================================*/

#include "trace_file.h"
#include "../config_parser.h"

#include <sstream>

namespace storage {

uint32_t FileManager::fileID_ = 0;
pthread_mutex_t FileManager::mutex_ = PTHREAD_MUTEX_INITIALIZER;


std::string FileManager::getTraceFileName(const std::string& storage_name) {
   std::stringstream ss;
   std::string path = ConfigParser::getDirectoryName();
   if (path.back() == '/') path.erase(path.end()-1);
   ss << path << "/data/" ;
   ss << storage_name << "_trace_" << fileID_ << ".dat";
   return ss.str();
}

std::string FileManager::getSipIndexName(const std::string& storage_name) {
   std::stringstream ss;
   std::string path = ConfigParser::getDirectoryName();
   if (path.back() == '/') path.erase(path.end()-1);
   ss << path << "/data/" ;
   ss << storage_name << "_sip_" << fileID_ << ".idx";
   return ss.str();
}

std::string FileManager::getDipIndexName(const std::string& storage_name) {
   std::stringstream ss;
   std::string path = ConfigParser::getDirectoryName();
   if (path.back() == '/') path.erase(path.end()-1);
   ss << path << "/data/" ;
   ss << storage_name << "_dip_" << fileID_ << ".idx";
   return ss.str();
}

std::string FileManager::getSportIndexName(const std::string& storage_name) {
   std::stringstream ss;
   std::string path = ConfigParser::getDirectoryName();
   if (path.back() == '/') path.erase(path.end()-1);
   ss << path << "/data/" ;
   ss << storage_name << "_sport_" << fileID_ << ".idx";
   return ss.str();
}

std::string FileManager::getDportIndexName(const std::string& storage_name) {
   std::stringstream ss;
   std::string path = ConfigParser::getDirectoryName();
   if (path.back() == '/') path.erase(path.end()-1);
   ss << path << "/data/" ;
   ss << storage_name << "_dport_" << fileID_ << ".idx";
   return ss.str();
}

std::string FileManager::getProtoIndexName(const std::string& storage_name) {
   std::stringstream ss;
   std::string path = ConfigParser::getDirectoryName();
   if (path.back() == '/') path.erase(path.end()-1);
   ss << path << "/data/" ;
   ss << storage_name << "_proto_" << fileID_ << ".idx";
   return ss.str();
}

std::string FileManager::getTimeIndexName(const std::string& storage_name) {
   std::stringstream ss;
   std::string path = ConfigParser::getDirectoryName();
   if (path.back() == '/') path.erase(path.end()-1);
   ss << path << "/data/" ;
   ss << storage_name << "_time_" << fileID_ << ".idx";
   return ss.str();
}

uint64_t TraceFile::writePacketsItem(const PacketsItem& item) {
    uint64_t offset = ftell( file_handle_ );
    total_bytes_ += item.write2file( file_handle_ );
    return offset;
}

uint64_t TraceFile::writeFlowRecord(const Ipv4Record& item) {
    item.write2file( file_handle_);
    uint32_t timestamp = item.get_time();
    if ( total_flows_ == 0 ) start_time = timestamp;
    start_time = std::min( start_time, timestamp );
    end_time = std::max( end_time, timestamp );
    return ++total_flows_;
}

//end for namespace 
}
