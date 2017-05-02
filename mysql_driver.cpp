/*=============================================================================
#     FileName: mysql_driver.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Taihua He
#        Email: hetaihua@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-19 15:44:14
#      History:
=============================================================================*/

#include "mysql_driver.h"
#include <sstream>

bool MysqlDriver::createTable(const std::string& tbname) {
    char sql[1024];
    snprintf( sql, 1024, "create table if not exists %s ( \
        id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, \
        start_time BIGINT NOT NULL, \
        end_time BIGINT, \
        data_file_name CHAR(255) NOT NULL, \
        sip_file_name CHAR(255) NOT NULL, \
        dip_file_name CHAR(255) NOT NULL, \
        sport_file_name CHAR(255) NOT NULL, \
        dport_file_name CHAR(255) NOT NULL, \
        proto_file_name CHAR(255) NOT NULL, \
        time_file_name CHAR(255) NOT NULL, \
        data_file_size BIGINT \
        );", tbname.c_str());

    if (mysql_query( &mysql, sql ) != 0) {
        fprintf(stderr, "Error message: %s\n", mysql_error(&mysql));
        return false;
    } else return true;
}


bool MysqlDriver::getMatchedRows(const std::string& tbname, uint64_t begin, uint64_t end,
        std::vector< std::vector<std::string> >& result ) {
    char sql[1024];
    snprintf( sql, 1024, "select data_file_name, sip_file_name,\
        dip_file_name, sport_file_name,\
        dport_file_name, proto_file_name, time_file_name\
        FROM %s where start_time <= %llu and end_time >= %llu",
         tbname.c_str(), end, begin );

    //fprintf( stdout, "sql sentence = %s\n", sql );

    int ret = mysql_query( &mysql, sql );

    if (ret) {
      return false;
    } else {
        MYSQL_RES* res = mysql_store_result(&mysql);
        if (res) {
            MYSQL_ROW row;
            std::vector<std::string> row_vec;
            while ((row = mysql_fetch_row(res))) {
                row_vec.clear();
                int columns = mysql_num_fields(res);
                for (int i = 0; i < columns; ++i) {
                    row_vec.push_back( std::string(row[i]) );
                }
                result.push_back( row_vec );
            }
            mysql_free_result(res);
            return true;
        } else {
            return false;
        }
    }
}

bool MysqlDriver::insertRow( const std::string& tbname, const std::vector<std::string>& columns ) {
    char sql[1024];
    snprintf( sql, 1024, "insert into %s (start_time, end_time,\
        data_file_name, sip_file_name, dip_file_name, \
        sport_file_name, dport_file_name, proto_file_name, time_file_name,\
        data_file_size) values (", tbname.c_str() );

    std::stringstream ss;
    ss << sql ;
    for (int i = 0; i < columns.size(); ++i) {
        if (i != 0) ss << ",";
        ss << "\"" << columns[i] << "\""; 
    }
    ss << ");";

    if (mysql_query( &mysql, ss.str().c_str() ) != 0) {
        fprintf(stderr, "Error message: %s\n", mysql_error(&mysql));
        fprintf(stderr, "Error sql: %s\n", ss.str().c_str());
        return false;
    } else return true;

    //return mysql_query(&mysql, ss.str().c_str());
}

uint32_t MysqlDriver::getTotalRows(const std::string& tbname ) {
    char sql[1024];
    snprintf( sql, 1024, "select * FROM %s ;", tbname.c_str() );

    int ret = mysql_query( &mysql, sql );

    if (ret) {
      return 0;
    } else {
        MYSQL_RES* res = mysql_store_result(&mysql);
        if (res) {
            uint32_t rows = mysql_num_rows(res);
            mysql_free_result(res);
            return rows;
        } else {
            return 0;
        }
    }
}
