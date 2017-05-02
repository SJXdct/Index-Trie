/*=============================================================================
#     FileName: mysql_driver.h
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Taihua He
#        Email: hetaihua@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-19 11:01:01
#      History:
=============================================================================*/

#ifndef MYSQL_DRIVER_H_
#define MYSQL_DRIVER_H_

#include "config_parser.h"

#include <mysql/mysql.h>
#include <string> 
#include <stdint.h>
#include <stdio.h>
#include <vector>

static const char *database_name = ConfigParser::getDatabaseName().c_str();
static const char *user_name = ConfigParser::getDatabaseUserName().c_str();
static const char *passwd = ConfigParser::getDatabaseUserPasswd().c_str();
static const char *location = "localhost";


class MysqlDriver {
public:
    MysqlDriver() {
        mysql_init(&mysql);
    }

    ~MysqlDriver() {
        mysql_close(&mysql); 
    }

    bool connect() {
        if (!mysql_real_connect(&mysql, location, user_name, passwd, 
                    database_name, 0, NULL, 0 ) ) {
            fprintf( stderr, "can not connect to database: %s\n", database_name);
            return false;
        }
        return true;
    }

    bool createTable(const std::string& tableName);

    bool getMatchedRows(const std::string& tbname, uint64_t begin, uint64_t end,
            std::vector< std::vector<std::string> >& result );

    bool insertRow( const std::string& tbname, const std::vector<std::string>& columns );

    uint32_t getTotalRows(const std::string& tbname );

private:
    MYSQL mysql;
};

#endif
