/*=============================================================================
#     FileName: config_parser.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-20 20:04:11
#      History:
=============================================================================*/

#include "config_parser.h"
#include "log_file.h"

#include <fstream>

YAML::Node ConfigParser::node = YAML::LoadFile("../config.yaml");

void ConfigParser::readConfig(const std::string& file) {
    try {
        node = YAML::LoadFile( file );
    } catch( YAML::ParserException& e) {
        LogFile::getErrorLog()->error( e.what() );
    }
}
