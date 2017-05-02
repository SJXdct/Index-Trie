/*=============================================================================
#     FileName: main.cpp
#     Copyright: All rights reserved.
#         Desc: 
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-12 13:32:43
#      History:
=============================================================================*/
#include "capture/capture_engine.h"
#include "capture/packet.h"
#include "hash_table.h"
#include "storage/storage.h"
#include "query/query.h"
#include "log_file.h"
#include "config_parser.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "../netflowCollector/interface.h"
//#include "../netflowCollector/interface-cpp.h"

#ifdef __cplusplus
}
#endif

using namespace capture;
using namespace std;
using storage::Storage;

#define OFFLINE_TEST


const char *device = "eth0";
const char *trace_file = "/home/hth/trace/10gbps-1/total.txt";
//const char *trace_file = "/home/hth/hth3.pcap";
//const char *trace_file = "/home/hth/desktop/trace/hth3.pcap";

//const std::string storage_name = "test";
//const uint64_t TRACE_FILE_MAX_SIZE = 1 * 1024 * 1024 ;
const std::string storage_name = ConfigParser::getTrafficStorageName();
const uint64_t TRACE_FILE_MAX_SIZE = ConfigParser::getMaxTraceFileSize();
const uint64_t TRACE_FILE_MAX_FLOWS = 50000000;
static Storage* pstorage = NULL;
static Category *main_log  = NULL; 
static Category *error_log = NULL; 
static Category *capture_log = NULL;
static bool capture_done = false;
//const char *device = "wlan0";


static void printTime() {
    timeval now ;
	gettimeofday( &now, NULL );
    printf("time = %d, %d\n",now.tv_sec , now.tv_usec);
}

// scan the hash table each scan-cycle seconds
// export the packets of time-out flows
static void *scanHashTable(void *) {
    main_log->info( "Scan Hash Table Thread is starting!\n");
    //fprintf( stdout, "Scan Hash Table Thread is starting!\n" );
    //int sleepSeconds = 60;
    int sleepSeconds = 1;
    HashTable* hash_table = HashTableInstance::getInstance();
    //pstorage* pstorage = new pstorage( pstorage_name );
    //pstorage = new pstorage( pstorage_name );

    while (true) {
        //fprintf( stdout, "Scaning Hash Table ......\n" );
        main_log->info( "Scaning Hash Table ......\n");
        for (int i = 0; i < hash_table->size(); ++i) {
            for (HashBucketIterator it = (*hash_table)[i].begin(); 
                    it != (*hash_table)[i].end(); ) {
                if ( (*it).needToExport() ) {
                    //(*it).printInfo();
                    pstorage->storePacketsItem( *it );
                    it = (*hash_table)[i].deleteItem( it );
                    //fprintf( stdout, "Bucket ID = %d\n", i);
                } else {
                    ++it;
                }
            }
        }
        //fprintf( stdout, "Scaning Hash Table Finished......\n" );
        main_log->info( "Scaning Hash Table Finished......\n");
        //printTime();

        if ( pstorage->currentFileSize() >= TRACE_FILE_MAX_SIZE ||
               ( capture_done && pstorage->currentFileSize() > 0 )) {
            pstorage->dumpTraceAndIndex();
        }

        sleep( sleepSeconds );
    }

    delete pstorage;
    main_log->info( "Scan Hash Table Thread is terminated!\n");
    //fprintf( stdout, "Scan Hash Table Thread is terminated!\n" );
}

// responsible for capturing the packets and inserting them to hash table
static void *captureThread( void *) {
    fprintf( stdout, "Packets Capture Thread is starting!, max = %lld\n",TRACE_FILE_MAX_SIZE  );
    main_log->info( "Packets Capture Thread is starting!\n");
    printTime();
    string name = "pcap engine";
    PcapEngine* capture = new PcapEngine(name);

    // open live device
    capture->openLiveDevice( (string)device ); 

    // open offline trace file
   // capture->openOfflineTrace( (string)trace_file ); 
    HashTable* hash_table = HashTableInstance::getInstance();
    uint32_t   captured_packets = 0;
    PacketSlice *packet = new PacketSlice();
    PacketSlice *ipv4_record = NULL; 

    while (true) {
        if (capture->getNextPacket(*packet)) {
            if ( ntohs(ETHERNET(packet->getConstData())->ether_type) == ETHERTYPE_IP) {
                //Ipv4Record record(packet);
                //cout << record << endl;
                ipv4_record = new Ipv4Record( packet );
                hash_table->addPacket( ipv4_record );
                ++captured_packets;
                if (captured_packets%100000 == 0) {
                    main_log->log( Priority::INFO, "%u packets are captured!\n", captured_packets );
                }
            }
        }
        else break;
    }
    delete capture;
    //delete packet;
    capture_done = true;
    main_log->info( "Packets Capture Thread is terminated!\n");
    fprintf( stdout, "Packets Capture Thread is terminated!, total = %lld\n",captured_packets );
    printTime();
}

// receiving the flow records from UDP Socket
static void *receiveFlowRecords(void *) {
    main_log->info( "Receiving Flow Records Thread is starting!\n");
//    cout<<"into receiveFlowRecords "<<endl;
    //pstorage* pstorage = new pstorage( pstorage_name );
    //pstorage = new pstorage( pstorage_name );
    char flow[2048] = {0};
    if (-1 == initNfMoudle(9999) ) {
        error_log->info("initNfModule failed\n");
  //      cout<<"initNfModule failed"<<endl;
        exit(-1);
    } else {
        main_log->info("initNfModule successed\n");
    //    cout<<"success"<<endl;
    }

    int sleepSeconds = 1;
    fstream fs;
    fs.open( trace_file );
    string line;
    Ipv4Record record;
    uint64_t captured_packets = 0;


    while (true) {
#ifdef OFFLINE_TEST
        if ( fs.eof() ) {
            capture_done = true;
        } else {
            std::getline( fs, line );
            record.readFromLine( line );
            //TODO: getting flow records from nfcollector
            pstorage->storeFlowRecords( record );
            //std::cout << record << std::endl;
            ++captured_packets;
            if (captured_packets%1000000 == 0) {
                capture_log->log( Priority::INFO, "%u packets are captured!\n", captured_packets );
            }
        }
#else
        if (getNextFlow(flow) < 0) {
            error_log->info("initNfModule failed\n");
            cout<<"getNextFlow failed"<<endl;
        } else {
            cout<<"getNextFlow finished!"<<endl;
            cout<<"flow size: "<<strlen(flow)<<endl;
            record.readFromLine( flow ); 
            //TODO: getting flow records from nfcollector
            pstorage->storeFlowRecords( record );
            ++captured_packets;
            if (captured_packets%1000000 == 0) {
                capture_log->log( Priority::INFO, "%u packets are captured!\n", captured_packets );
            }
        }
        memset(flow, 0, sizeof(flow));
#endif

        // after finishing capture, trace file and index files need to be dumped.
        if ( pstorage->currentFlowsNumber() >= TRACE_FILE_MAX_FLOWS ||
               ( capture_done && pstorage->currentFileSize() > 0 )) {
            pstorage->dumpTraceAndIndex();
            //sleep( sleepSeconds );
        }
        if (capture_done) break;

    }

    fs.close();

    delete pstorage;
    main_log->info( "Receiving Flow Records Thread is terminated!\n");
    //fprintf( stdout, "Scan Hash Table Thread is terminated!\n" );
}


typedef void* (*ThreadFunc)(void *);

struct ThreadStruct {
    ThreadFunc func;
    char*      name;
    pthread_t  threadID;
}threads_list[] = {
      {receiveFlowRecords, "receiving Flow Records thread", },
    //{scanHashTable, "scan HashTable thread", },
    //{captureThread, "packets capture thread",},
};


void parseQueryCommands() {
    //if (pstorage == NULL) {
        //pstorage = new pstorage( pstorage_name );
    //new pstorage(pstorage_name)    //pstorage* pstorage = new pstorage( pstorage_name );
    Query *query = new Query(pstorage);
    char command[4096];
    const char *prefix = "command>";
    std::cout << prefix ;
    while (cin.getline(command, 4096)) {
       query->processCommand(command);
       std::cout << prefix ;
    }
}

static void createDirectory( const std::string& dir) {
        char *tmp = strdup( dir.c_str() );
        char *p = NULL;
        size_t len = strlen(tmp);
        //fprintf(stdout, "len = %d\n", len);
        if(tmp[len - 1] == '/')
            tmp[len - 1] = '\0';
        for(p = tmp + 1; *p; p++)
            if(*p == '/') {
                *p = '\0';
                mkdir(tmp, S_IRWXU);
                //fprintf(stdout, "path = %s\n", tmp);
                *p = '/';
            }
        mkdir(tmp, S_IRWXU);
        free(tmp);
}

void initial() {
    std::string path = ConfigParser::getDirectoryName();
   //
//    std::string path = "/home/hth/testOnline/";
    if (path.back() == '/') path.erase( path.end()-1 );
    std::string data_path = path + "/data";
    std::string log_path = path + "/log"; 
    createDirectory( data_path );
    createDirectory( log_path );
    //fprintf(stdout, "path = %s\n", data_path.c_str());

    pstorage = new Storage(storage_name);
    main_log = LogFile::getMainLog();
    error_log = LogFile::getErrorLog();
    capture_log = LogFile::getCaptureLog();

#ifdef LINKNODE_POOL
     LinkList<uint32_t>::initLinkNodePool( TRACE_FILE_MAX_FLOWS * 6 / 8 + TRACE_FILE_MAX_FLOWS / 5);
#endif


    int threads_num = sizeof(threads_list)/sizeof(ThreadStruct);
    for (int i = 0; i < threads_num; ++i) {
        pthread_create( &(threads_list[i].threadID), NULL, threads_list[i].func, NULL );
    }
}

void clearResource() {

    int threads_num = sizeof(threads_list)/sizeof(ThreadStruct);
    for (int i = 0; i < threads_num; ++i) {
        pthread_join( threads_list[i].threadID, NULL );
    }
    LogFile::clear();
#ifdef LINKNODE_POOL
    LinkList<uint32_t>::clearPool();
#endif
}

int main() {
    initial();
    parseQueryCommands();
    clearResource();
 
   /*
    string name = "pcap engine";
    PcapEngine* capture = new PcapEngine(name);
    capture->openLiveDevice( (string)device ); 
    PacketSlice packet;
    while (true) {
        if (capture->getNextPacket(packet)) {
            cout << packet << std::endl;
            cout << "sdfasf " << ETHERNET(packet.getConstData())->ether_type << endl;
            if ( ntohs(ETHERNET(packet.getConstData())->ether_type) == ETHERTYPE_IP) {
                Ipv4Record record(packet);
                cout << record << endl;
            }
        }
    }
    delete capture;
    */
    return 0;
}
