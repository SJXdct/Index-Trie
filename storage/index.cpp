/*=============================================================================
#     FileName: index.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-14 17:12:06
#      History:
=============================================================================*/
#include "index.h"
#include "trace_file.h"
#include "../log_file.h"

#include <utility>
#include <stdio.h>
#include <string>

using std::pair;

const char * INDEX_FILE_SUFFIX = ".idx";

namespace storage {

typedef pair<uint16_t,Index *> UIPAIR;

static void printTime() {
    timeval now ;
	gettimeofday( &now, NULL );
    printf("time = %d, %d\n",now.tv_sec, now.tv_usec);
}

// sipindex, dipindex, sportindex ...
static void *index_thread_start( void * arg) {
    UIPAIR* uipair = (UIPAIR *) arg;
    INDEXCODE code = (INDEXCODE)uipair->first; 
    Index* index = uipair->second;
    delete uipair; 
    /*
       int state = PTHREAD_CANCEL_ASYNCHRONOUS, oldstate ;
       pthread_setcanceltype( state, &oldstate );
       */
    LogFile::getMainLog()->info("Index Thread %d has code %d is starting\n", (int32_t)pthread_self(), (int)code );

//    printf("Index Thread %d has code %d is starting\n", (int32_t)pthread_self(), (int)code );
    while( 1 ) {

        index->buildIndex( code );

        if ( index->getWriteIndexFlag() & code ) {
            index->writeIndexToFile( code );
            index->resetWriteIndexFlag( code );
            //if (index->getWriteIndexFlag() == 0 ) {
                //printTime();
            //}
        }

        // check exit-signal;
        if ( index->getExitSignal() ) {
            pthread_exit(0);
        }

        /*
        // check the cancel condition when exporter's queue was empty
        if( index->linkNodePool.size() == 0 ) {
        pthread_testcancel();
        }
        */
    }
    LogFile::getMainLog()->info("Index Thread %d has code %d is terminated\n", (int32_t)pthread_self(), (int)code );
  //  printf("Index Thread %d has code %d is terminated\n", (int32_t)pthread_self(), (int)code );
}

Index::Index( const std::string& name, uint16_t flag ): storage_name(name), indexFlag(flag) {
    sipIndex = dipIndex = NULL;
    sportIndex = dportIndex = NULL;
    protoIndex = NULL;
    timeIndex = NULL;
    //pthread_mutex_init( &queueLock, NULL);
    //pthread_cond_init( &emptyCond, NULL );
    //pthread_cond_init( &fullCond, NULL );
    //readCode = 0;

    // initialization of exit signal;
    exit_signal = false;
    pthread_rwlock_init( &exit_mutex, NULL );
    write_index_flag = 0;
    pthread_rwlock_init( &write_index_lock, NULL);

    if( flag & SRCIP ) {
        sipIndex = new IndexBase<uint32_t>(
                FileManager::getSipIndexName(storage_name) );
        pthread_t	tid;
        // this memory should be delete in function index_thread_start
        // why we did it in this way because the local stack memory may be not
        // available or correct when the created thread access
        UIPAIR 	*uipair = new UIPAIR(SRCIP,this);
        int err = pthread_create( &tid, NULL, index_thread_start, (void *)uipair);
        if( err ) {
            ERROR_INFO("cannot create thread!",);
        } else {
            threadVec.push_back( tid );
        }
    }

    if( flag & DSTIP ) {
        dipIndex = new IndexBase<uint32_t>(
                FileManager::getDipIndexName(storage_name));
        pthread_t	tid;
        // this memory should be delete in function index_thread_start
        UIPAIR 	*uipair = new UIPAIR(DSTIP,this);
        int err = pthread_create( &tid, NULL, index_thread_start, (void *)uipair );
        if( err ) {
            ERROR_INFO("cannot create thread!",);
        }else {
            threadVec.push_back( tid );
        }
    }

    if( flag & SPORT ) {
        sportIndex = new IndexBase<uint16_t>(
                FileManager::getSportIndexName(storage_name));
        pthread_t	tid;
        // this memory should be delete in function index_thread_start
        UIPAIR 	*uipair = new UIPAIR(SPORT,this);
        int err = pthread_create( &tid, NULL, index_thread_start, (void *)uipair );
        if( err ) {
            ERROR_INFO("cannot create thread!",);
        } else {
            threadVec.push_back( tid );
        }
    }

    if( flag & DPORT ) {
        dportIndex = new IndexBase<uint16_t>(
                FileManager::getDportIndexName(storage_name));
        pthread_t	tid;
        // this memory should be delete in function index_thread_start
        UIPAIR 	*uipair = new UIPAIR(DPORT,this);
        int err = pthread_create( &tid, NULL, index_thread_start, (void *)uipair);
        if( err ) {
            ERROR_INFO("cannot create thread!",);
        } else {
            threadVec.push_back( tid );
        }
    }

    if( flag & PROTO ) {
        protoIndex = new IndexBase<uint8_t>(
                FileManager::getProtoIndexName(storage_name));
        pthread_t	tid;
        // this memory should be delete in function index_thread_start
        UIPAIR 	*uipair = new UIPAIR(PROTO, this);
        int err = pthread_create( &tid, NULL, index_thread_start, (void *)uipair);
        if( err ) {
            ERROR_INFO("cannot create thread!",);
        } else {
            threadVec.push_back( tid );
        }
    }

    if( flag & TIME ) {
        timeIndex = new IndexBase<uint32_t>(
                FileManager::getTimeIndexName(storage_name));
        pthread_t	tid;
        // this memory should be delete in function index_thread_start
        UIPAIR 	*uipair = new UIPAIR(TIME, this);
        int err = pthread_create( &tid, NULL, index_thread_start, (void *)uipair);
        if( err ) {
            ERROR_INFO("cannot create thread!",);
        } else {
            threadVec.push_back( tid );
        }
    }

}

Index::~Index() {
   //pthread_cond_destroy( &emptyCond );
   //pthread_cond_destroy( &fullCond );
   //pthread_mutex_destroy( &queueLock );

   this->setExitSignal();
   for( int i = 0 ; i < threadVec.size(); ++i ) {
       pthread_join( threadVec[i], NULL);
   }
   pthread_rwlock_destroy( &exit_mutex );
   pthread_rwlock_destroy( &write_index_lock );

   if( sipIndex ) delete sipIndex;
   if( dipIndex ) delete dipIndex;
   if( sportIndex ) delete sportIndex;
   if( dportIndex ) delete dportIndex;
   if( protoIndex ) delete protoIndex;
   if( timeIndex ) delete timeIndex;

   // release memory in linkNodePool
   //for( int i = 0 ; i < linkNodePool.size(); ++i ) {
       //delete linkNodePool.get_resource(i);
   //}
}

void Index::buildIndex(INDEXCODE code) {
    switch( code ) {
        case SRCIP: sipIndex->insertRecords(); break;
        case DSTIP: dipIndex->insertRecords(); break;
        case SPORT: sportIndex->insertRecords(); break; 
        case DPORT: dportIndex->insertRecords(); break;
        case PROTO: protoIndex->insertRecords(); break;
        case TIME:  timeIndex->insertRecords(); break;
        default:
                    ERROR_INFO("Error index code",);
    }
}


void Index::addRecord( const Ipv4Record& record, const VALUE& position) {
    //std::cout << record << std::endl;
    sipIndex->addItem( record.get_srcip(), position );
    dipIndex->addItem( record.get_dstip(), position );
    sportIndex->addItem( record.get_sport(), position );
    dportIndex->addItem( record.get_dport(), position );
    protoIndex->addItem( record.get_proto(), position );
    timeIndex->addItem( record.get_time(), position );
}

OffsetsResult* Index::queryFromFile(INDEXCODE code, const char *file,
        const uint32_t& begin, const uint32_t& end) {
    printf("Index::queryFromFile:code = %d , begin = %d , end = %d\n");
    switch( code ) {
        case SRCIP: return sipIndex->queryFromFile(file, begin, end); 
        case DSTIP: return dipIndex->queryFromFile(file, begin, end); 
        case SPORT: return sportIndex->queryFromFile(file, begin, end); 
        case DPORT: return dportIndex->queryFromFile(file, begin, end); 
        case PROTO: return protoIndex->queryFromFile(file, begin, end); 
        case TIME:  return timeIndex->queryFromFile(file, begin, end); 
        default:
                    ERROR_INFO("Error index code",);
    }
    return NULL;
}

OffsetsResult* Index::queryFromMem(INDEXCODE code, const uint32_t& begin, const uint32_t& end) {
    switch( code ) {
        case SRCIP: return sipIndex->queryFromMem( begin, end); 
        case DSTIP: return dipIndex->queryFromMem( begin, end); 
        case SPORT: return sportIndex->queryFromMem( begin, end); 
        case DPORT: return dportIndex->queryFromMem( begin, end); 
        case PROTO: return protoIndex->queryFromMem( begin, end); 
        case TIME:  return timeIndex->queryFromMem( begin, end); 
        default:
                    ERROR_INFO("Error index code",);
    }
    return NULL;
}

void Index::writeIndexToFile( INDEXCODE code ) {
    std::string file_name;

    if( code & SRCIP ) {
        sipIndex->write2file();
        delete sipIndex;
        // TODO: need thread-safe to sipindex here?
        sipIndex = new IndexBase<uint32_t>(FileManager::getSipIndexName(storage_name));
    }

    if( code & DSTIP ) {
        dipIndex->write2file();
        delete dipIndex;
        dipIndex = new IndexBase<uint32_t>(FileManager::getDipIndexName(storage_name));
    }

    if( code & SPORT ) {
        sportIndex->write2file();
        delete sportIndex;
        sportIndex = new IndexBase<uint16_t>(FileManager::getSportIndexName(storage_name));
    }

    if( code & DPORT ) {
        dportIndex->write2file();
        delete dportIndex;
        dportIndex = new IndexBase<uint16_t>(FileManager::getDportIndexName(storage_name));
    }

    if( code & PROTO ) {
        protoIndex->write2file();
        delete protoIndex;
        protoIndex = new IndexBase<uint8_t>(FileManager::getProtoIndexName(storage_name));
    }

    if( code & TIME ) {
        timeIndex->write2file();
        delete timeIndex;
        timeIndex = new IndexBase<uint32_t>(FileManager::getTimeIndexName(storage_name));
    }

}

//end for namespace
}


