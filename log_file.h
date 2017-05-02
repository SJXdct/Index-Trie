/*=============================================================================
#     FileName: log_file.h
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-20 11:01:04
#      History:
=============================================================================*/

#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <log4cpp/Category.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/RollingFileAppender.hh>

using namespace log4cpp;

class LogFile {
public:
    LogFile() {
    }

    ~LogFile() {
    }

    static Category* getErrorLog() ;
    static Category* getCaptureLog();
    static Category* getMainLog();
    static void clear() {
        Category::shutdown(); 
    }
private:
    static bool error_log_init;
    static bool capture_log_init;
    static bool main_log_init;
    static std::string path_name;
};



#endif
