/*=============================================================================
#     FileName: log_file.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-20 11:34:18
#      History:
=============================================================================*/
#include "log_file.h"
#include "config_parser.h"
#include <string>

bool LogFile::error_log_init = false;
bool LogFile::capture_log_init = false;
bool LogFile::main_log_init = false;
//static const std::string path_name = "";
std::string LogFile::path_name = ".";

Category* LogFile::getErrorLog() {
   std::string path = ConfigParser::getDirectoryName();
   if (path.back() == '/') path.erase(path.end()-1);
   std::string file_name = path + "/log/error.log";
   //std::string file_name = path_name + std::string("/logs/error.log");
   if (!error_log_init) {
        Layout* layout = new BasicLayout();
        Appender* appender = new RollingFileAppender("rollFileAppender", file_name);
        appender->setLayout( layout );
        Category& temp = Category::getInstance("error");
        temp.setAdditivity(false);
        temp.setAppender( appender );
        temp.setPriority(Priority::WARN);
        error_log_init = true;
    }
    assert( Category::exists("error") != NULL );
    return Category::exists( "error" );
}


Category* LogFile::getMainLog() {
    //std::string file_name = path_name + std::string("/logs/main.log");
    std::string path = ConfigParser::getDirectoryName();
    if (path.back() == '/') path.erase(path.end()-1);
    std::string file_name = path + "/log/main.log";
    //fprintf( stdout, "file = %s\n", file_name.c_str());
    if (!main_log_init) {
        Layout* layout = new BasicLayout();
        Appender* appender = new RollingFileAppender("FileAppender", file_name);
        appender->setLayout( layout );
        Category& temp = Category::getInstance("main");
        temp.setAdditivity(false);
        temp.setAppender( appender );
        main_log_init = true;
    }
    assert( Category::exists("main") != NULL );
    return Category::exists( "main" );
}


Category* LogFile::getCaptureLog() {
    //std::string file_name = path_name + std::string("/logs/capture.log");
    std::string path = ConfigParser::getDirectoryName();
    if (path.back() == '/') path.erase(path.end()-1);
    std::string file_name = path + "/log/capture.log";
    if (!capture_log_init) {
        Layout* layout = new BasicLayout();
        Appender* appender = new RollingFileAppender("FileAppender", file_name);
        appender->setLayout( layout );
        Category& temp = Category::getInstance("capture");
        temp.setAdditivity(false);
        temp.setAppender( appender );
        //temp.setPriority(Priority::WARN);
        capture_log_init = true;
        //assert( Category::exists("capture") != NULL );
    }
    //fprintf(stdout, "current Category size = %d\n", Category::getCurrentCategories()->size());
    //std::vector<Category*> vec = *Category::getCurrentCategories();
    //for (int i = 0;i < vec.size(); ++i) {
        //fprintf( stdout, "name = %s\n", (vec[i])->getName().c_str());
    //}
    assert( Category::exists("capture") != NULL );
    return Category::exists( "capture" );
}


