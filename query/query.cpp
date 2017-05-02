/*=============================================================================
#     FileName: query.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-18 14:47:04
#      History:
=============================================================================*/

#include "query.h"
#include <pcap.h>
#include <iostream>

using namespace std;

static void printTime() {
    timeval now ;
	gettimeofday( &now, NULL );
    printf("time = %d, %d\n",now.tv_sec , now.tv_usec);
}

struct node {
    uint32_t time;
    uint32_t sip, dip;
    uint16_t sport,dport;
    uint8_t proto;
};

extern int parse_req_parameters(const char*, QueryRequest*);
extern OffsetsResult* parse_command( const char*, FILE*, Query*, QueryRequest*);
//namespace query {

void Query::processCommand(const char* command) {
    // get parameters of request
//#define QUERY_TEST
#ifdef QUERY_TEST
    query_request->sip_index_file = "test_sip_2.idx";
    query_request->dip_index_file = "test_dip_2.idx";
    query_request->sport_index_file = "test_sport_2.idx";
    query_request->dport_index_file = "test_dport_2.idx";
    query_request->proto_index_file = "test_proto_2.idx";
    query_request->data_file_name = "test_trace_2.dat";
#endif

    parse_req_parameters(command, query_request);
    fprintf( stdout, "query type = %d\n", query_request->query_type);
    QUERYTYPE type = query_request->query_type;

    // record start time
    //printTime();

    if (type == HELP) {
    
    } else if (type == SAVE) {
        const char* save_file = query_request->save_file_name.c_str();
        //fprintf( stdout, "save file name = %s\n", save_file);
        pcap_t* pcap = pcap_open_dead( DLT_EN10MB, 1500);
        //pcap_dumper_t *dump = pcap_dump_open(pcap, save_file );
        FILE *dump = fopen( save_file, "wb");
        if (NULL == dump) {
            fprintf(stdout, "cannot open file %s in file %s, line %d\n",
                    save_file, __FILE__, __LINE__);
        }

        std::vector< std::vector<std::string> > mysql_result;
        storage->getMatchedTraces( query_request->start_time, query_request->end_time, mysql_result );

        if (storage->getStartTimestamp() >= query_request->start_time &&
             storage->getStartTimestamp() <= query_request->end_time ) {
            //OffsetsResult* result = parse_command(command, stdout, this, NULL);
             OffsetsResult* result = parse_command(command, NULL, this, NULL);
            if (result != NULL) {
                uint64_t count = storage->saveMatchedFlowsToFile( result,
                        FileManager::getTraceFileName(storage->getStorageName()).c_str(),
                        dump, query_request->start_time, query_request->end_time );
                fprintf( stdout, "%u packets are found in trace file %s\n", count,
                        FileManager::getTraceFileName(storage->getStorageName()).c_str()) ;
            }
        }

        for (int i = 0; i < mysql_result.size(); ++i) {
            query_request->data_file_name   = mysql_result[i][0];
            query_request->sip_index_file   = mysql_result[i][1];
            query_request->dip_index_file   = mysql_result[i][2];
            query_request->sport_index_file = mysql_result[i][3];
            query_request->dport_index_file = mysql_result[i][4];
            query_request->proto_index_file = mysql_result[i][5];
            query_request->time_index_file = mysql_result[i][6];
            //OffsetsResult* result = parse_command(command, stdout, this, query_request);
            OffsetsResult* result = parse_command(command, NULL, this, query_request);
            uint64_t count = 0;
            if (result != NULL) {
                 count = storage->saveMatchedFlowsToFile( result, mysql_result[i][0].c_str(), 
                        dump, query_request->start_time, query_request->end_time );
            }
            fprintf( stdout, "%u packets are found in trace file %s\n", count, mysql_result[i][0].c_str() ) ;
        }
        //pcap_dump_flush( dump );
        //pcap_dump_close( dump );
        fflush(dump);
        fclose(dump);
    } else if (type == COUNT){
        std::vector< std::vector<std::string> > mysql_result;
        storage->getMatchedTraces( query_request->start_time, query_request->end_time, mysql_result );
        //fprintf(stdout, "start = %lld, end = %lld\n", query_request->start_time, query_request->end_time );
        uint64_t packets_count = 0;

        if (storage->getStartTimestamp() >= query_request->start_time &&
             storage->getStartTimestamp() <= query_request->end_time ) {
            OffsetsResult* result = parse_command(command, NULL, this, NULL);
            //OffsetsResult* result = parse_command(command, stdout, this, NULL);
            if (result != NULL) {
                //uint64_t count = storage->saveMatchedFlowsToFile( result,
                        //FileManager::getTraceFileName(storage->getStorageName()).c_str(),
                        //NULL, query_request->start_time, query_request->end_time );
                fprintf( stdout, "%u packets are found in trace file %s\n", result->offsets.size(),
                        FileManager::getTraceFileName(storage->getStorageName()).c_str()) ;
                packets_count += result->offsets.size();
            }
        }

        for (int i = 0; i < mysql_result.size(); ++i) {
            query_request->data_file_name   = mysql_result[i][0];
            query_request->sip_index_file   = mysql_result[i][1];
            query_request->dip_index_file   = mysql_result[i][2];
            query_request->sport_index_file = mysql_result[i][3];
            query_request->dport_index_file = mysql_result[i][4];
            query_request->proto_index_file = mysql_result[i][5];
            query_request->time_index_file = mysql_result[i][6];
            OffsetsResult* result = parse_command(command, NULL, this, query_request);
            //OffsetsResult* result = parse_command(command, stdout, this, query_request);
            uint64_t count = 0;
            if (result != NULL) {
                 //count = storage->saveMatchedFlowsToFile( result, mysql_result[i][0].c_str(), 
                        //NULL, query_request->start_time, query_request->end_time );
                count = result->offsets.size();
                packets_count += count;
            }

            fprintf( stdout, "%u packets are found in trace file %s\n", count, mysql_result[i][0].c_str() ) ;
        }
        fprintf( stdout, "%u packets are found at total\n", packets_count ) ;
    } else if (type == SHOW){
        cout<<"Flow Content Format: TIME SIP DIP SPORT DPORT PROTO"<<endl;
        std::vector< std::vector<std::string> > mysql_result;
        storage->getMatchedTraces( query_request->start_time, query_request->end_time, mysql_result );
        //printTime();
        uint64_t packets_count = 0;

        if (storage->getStartTimestamp() >= query_request->start_time ) {
            OffsetsResult* result = parse_command(command, NULL, this, NULL);
            if (result != NULL) {
              /*  uint64_t count = storage->saveMatchedPacketsToFile( result,
                        FileManager::getTraceFileName(storage->getStorageName()).c_str(),
                        NULL, query_request->start_time, query_request->end_time );
                fprintf( stdout, "%u packets are found in trace file %s\n", count,
                        FileManager::getTraceFileName(storage->getStorageName()).c_str()) ;*/

                 /*  uint64_t count = result->offsets.size();
                   cout<<"queryfrommem result!=NULL, result->offsets.size()= "<<count<<endl;
                   node t[count];
                   FILE * readfp = fopen( query_request->data_file_name.c_str() , "rb" );
                   if(readfp == NULL){
                //       cout<<"open trace file failed!"<<endl;
                cout<<"open trace file "<<query_request->data_file_name.c_str()<<" failed!"<<endl;
                   }
                   int k;
                   while (k < count) {
                    uint32_t offset_value;
                    offset_value = result->offsets[k];
                    if( fseek( readfp, offset_value, SEEK_SET ) == -1 ) 
                        cout<<"Seeking file position failed"<<endl;
                    fread( &t[k], 1, sizeof(t[k]), readfp);
                    //show the details of the flows
                    cout<<t[k].time<<" "<<t[k].sip<<" "<<t[k].dip<<" "<<t[k].sport<<" "<<t[k].dport<<" "<<t[k].proto<<" "<<endl;
                //  fprintf( stdout, "%s %s %s %s %s")
                    ++k;
                }
                fclose(readfp);*/

            }
        }

        for (int i = 0; i < mysql_result.size(); ++i) {
            query_request->data_file_name   = mysql_result[i][0];
            query_request->sip_index_file   = mysql_result[i][1];
            query_request->dip_index_file   = mysql_result[i][2];
            query_request->sport_index_file = mysql_result[i][3];
            query_request->dport_index_file = mysql_result[i][4];
            query_request->proto_index_file = mysql_result[i][5];
            query_request->time_index_file = mysql_result[i][6];
            OffsetsResult* result = parse_command(command,NULL , this, query_request);
            if (result != NULL) {
               // uint64_t count = storage->saveMatchedPacketsToFile( result, mysql_result[i][0].c_str(),
                 //       NULL, query_request->start_time, query_request->end_time );
               // packets_count += count;
              //  fprintf( stdout, "%u packets are found in trace file %s\n", count, mysql_result[i][0].c_str() ) ;
            uint64_t count = result->offsets.size();
         /*   cout<<"queryfromFile result!=NULL, result->offsets.size()= "<<count<<endl;
            cout<<"data_file_name: "<<query_request->data_file_name<<endl;
            cout<<"sip_index_file: "<<query_request->sip_index_file<<endl;
            cout<<"dip_index_file: "<<query_request->dip_index_file<<endl;
            cout<<"sport_index_file: "<<query_request->sport_index_file<<endl;
            cout<<"dport_index_file: "<<query_request->dport_index_file<<endl;
            cout<<"proto_index_file: "<<query_request->proto_index_file<<endl;
            cout<<"time_index_file: "<<query_request->time_index_file<<endl;*/
            node t[count];
            FILE * readfp = fopen( query_request->data_file_name.c_str() , "rb" );
            if(readfp == NULL){
                cout<<"open trace file "<<query_request->data_file_name.c_str()<<" failed!"<<endl;
            }
            int k=0;
            while (k < count) {
             //   cout<<"into while loop"<<endl;
                uint32_t offset_value;
                offset_value = result->offsets[k];
              //  cout<<"offset_value = "<<offset_value<<endl;
                if( fseek( readfp, offset_value, SEEK_SET ) == -1 ) 
                    cout<<"Seeking file position failed"<<endl;
                  if( fread( &t[k], 1, sizeof(t[k]), readfp)== 0 )
                      cout<<"fread error!"<<endl;
                    //show the details of the flows
                  //  printf("proto:%d\n", t[k].proto);
                   // cout<<t[k].time<<" "<<t[k].sip<<" "<<t[k].dip<<" "<<t[k].sport<<" "<<t[k].dport<<" "<<t[k].proto<<" "<<endl;
                   //
                   // convert sip format
                   char *ipaddr = NULL;
                   char addr[20];
                   in_addr inaddr;
                   inaddr.s_addr = t[k].sip;
                   ipaddr = inet_ntoa(inaddr);
                   strcpy(addr, ipaddr);
                   //printf("sip = %s \n",addr);
                   //
                   // convert dip format
                   char *ipaddr1 = NULL;
                   char addr1[20];
                   in_addr inaddr1;
                   inaddr1.s_addr = t[k].dip;
                   ipaddr1 = inet_ntoa(inaddr1);
                   strcpy(addr1, ipaddr1);
                   //
                   //
                  printf(" %u %s %s %hu %hu %d \n",t[k].time, addr, addr1, t[k].sport, t[k].dport, t[k].proto );
                  ++k;
                }

                fclose(readfp);
            }
        }
    }

    //printTime();
}

//end for namespace 
//}




