/*=============================================================================
#     FileName: config_parser.h #     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-20 19:51:04
#      History:
=============================================================================*/

#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include <yaml-cpp/yaml.h>
#include <stdint.h>
#include <fstream>


class ConfigParser {
public:
    static void readConfig(const std::string& config_file);
    
    static uint64_t getMaxTraceFileSize() {
        //std::cout << " max trace file " << std::endl;
        //return node["trace_file_size"].as<uint64_t>();
        return 1;
    }

    static uint16_t getScanIntervalTime() {
        //std::cout << " max trace file " << std::endl;
        //return node["scan_interval_time"].as<uint16_t>();
        return 1;
    }

    static std::string getDirectoryName() {
        //std::cout << " max trace file " << std::endl;
        //return node["directory_name"].as<std::string>();
      //  return "/home/hth/trafficTest/";
        return "/home/hth/testOnline/";
    }

    static std::string getDatabaseName() {
        //return node["database_name"].as<std::string>();
        return "test";
    }

    static std::string getDatabaseUserName() {
        //std::cout << " max trace file " << std::endl;
        //return node["database_user_name"].as<std::string>();
        return "hth";
    }

    static std::string getDatabaseUserPasswd() {
        //std::cout << " max trace file " << std::endl;
        //return node["database_user_passwd"].as<std::string>();
        return "hth";
    }

    static std::string getTrafficStorageName() {
        //std::cout << " max trace file " << std::endl;
        //return node["traffic_storage_name"].as<std::string>();
        return "test";
    }

private:
    static YAML::Node node;
};

#endif
