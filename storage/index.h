/*=============================================================================
#     FileName: index.h
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-14 14:08:58
#      History:
=============================================================================*/
#ifndef INDEX_H_
#define INDEX_H_

#ifdef BTREE
#include "../util/btree.h"
#else 
#include "../util/trie.h"
#endif

#include "../capture/packet.h"

#include <pthread.h>
#include <vector>

//#include "resourcePool.h"
//#include "../record.h"
using std::vector;
using capture::Ipv4Record;

namespace storage {

//typedef uint64_t VALUE;
//typedef uint64_t OFFSET;
typedef uint32_t OFFSET;
typedef struct LISTELEM VALUE;

/*typedef struct{
    uint32_t offset;
    uint32_t packets_num;
}VALUE;
*/
enum INDEXCODE {
	SRCIP = 1,
	DSTIP = 2,
	SPORT = 4,
	DPORT = 8,
	PROTO = 16,
    TIME = 32
};
/*
struct OffsetsResult {
    std::vector<OFFSET> offsets;
    void intersection( OffsetsResult* result) {
        std::vector<OFFSET>* temp = new std::vector<OFFSET>;
        int i = 0, j = 0;
        while (i < offsets.size() && j < result->offsets.size() ) {
            if ( offsets[i] == result->offsets[j]) {
                temp->push_back( offsets[i] );
                ++i;
                ++j;
            } else if ( offsets[i] > result->offsets[j]) {
                ++j;
            } else {
                ++i;
            }
        }
        offsets.swap(*temp);
        delete temp;
    }
    void merge( OffsetsResult* result) {
        std::vector<OFFSET>* temp = new std::vector<OFFSET>;
        int i = 0, j = 0;
        while (i < offsets.size() && j < result->offsets.size() ) {
            if ( offsets[i] == result->offsets[j]) {
                temp->push_back( offsets[i] );
                ++i;
                ++j;
            } else if ( offsets[i] > result->offsets[j]) {
                temp->push_back( result->offsets[j] );
                ++j;
            } else {
                temp->push_back( offsets[i] );
                ++i;
            }
        }
        offsets.swap(*temp);
        delete temp;
    }

    size_t size() {
            return offsets.size();
    }
//added by sujingxiuW
//
   // void clear(){
     //   offsets.clear();
   // }
//
};

*/
struct OffsetsResult {
    std::vector<VALUE> offsets;
    void intersection( OffsetsResult* result) {
        std::vector<VALUE>* temp = new std::vector<VALUE>;
        //LinkList<VALUE>* temp = new LinkList<VALUE>;
        int i = 0, j = 0;
        while (i < offsets.size() && j < result->offsets.size() ) {
        //    cout<<"offset1= "<<offsets[i].offset << " offset2 = "<<result->offsets[j].offset<<endl;
            if ( offsets[i].offset == result->offsets[j].offset) {
                temp->push_back( offsets[i] );
                ++i;
                ++j;
            } else if ( offsets[i].offset > result->offsets[j].offset) {
                ++j;
            } else {
                ++i;
            }
        }
        offsets.swap(*temp);
        delete temp;
    }
    void merge( OffsetsResult* result) {
        std::vector<VALUE>* temp = new std::vector<VALUE>;
        int i = 0, j = 0;
        while (i < offsets.size() && j < result->offsets.size() ) {
            if ( offsets[i].offset == result->offsets[j].offset) {
                temp->push_back( offsets[i] );
                ++i;
                ++j;
            } else if ( offsets[i].offset > result->offsets[j].offset) {
                temp->push_back( result->offsets[j] );
                ++j;
            } else {
                temp->push_back( offsets[i] );
                ++i;
            }
        }
        offsets.swap(*temp);
        delete temp;
    }

    size_t size() {
            return offsets.size();
    }
};



static void *index_thread_start( void *) ;

// used as an interface
template<typename KEY>
class IndexBase {
public:

	typedef LinkList<VALUE>* RESULT;

    //added by sujingxiu
   // IndexBase(){};
    
    //

	IndexBase(const std::string& name):index_file_(name) {
#ifdef BTREE
		index = new BTree<KEY,VALUE>();
#else
		index = new Trie<KEY>();
#endif
        pthread_mutex_init( &mutex_, NULL);
	}

	~IndexBase() {
		if( index ) delete index;
        pthread_mutex_destroy( &mutex_ );
	}

    /*
	int addItem( const KEY& key, const VALUE& value ) {
        lock();
        int ret = index->addItem( key, value );
        unlock();
        return ret;
	}
    */

    void addItem( const KEY& key, const VALUE& value ) { 
        lock();
        records_.push_back( KVPR(key,value) );
        //if (sizeof(KEY) == 1) printf("key = %d, value = %d\n", key, value);
        //fprintf( stdout, "size = %d in addItem\n", records_.size() );
        unlock();
    }

    void insertRecords() {
        lock();
        for (int i = 0; i < records_.size(); ++i) {
            index->addItem(records_[i].first, records_[i].second);
        }
        records_.clear();
        unlock();
    }

	int write2file() {
        lock();
		int ret = index->write2file( index_file_.c_str() );
        unlock();
        return ret;
	}

    /*
    //return the linklist values
	RESULT queryFromFile(const char *file, const KEY& b, const KEY& e ) {
		return index->queryFromFile( file, b, e );
	}*/

    
    //use this function
    OffsetsResult* queryFromFile(const char* file, const KEY& b, const KEY& e) {
        OffsetsResult* result = new OffsetsResult();
       // index->queryFromFile( file, b, e, result->offsets);
        //result = index->queryFromFile( file, b, e); //return the linklist value
        index->queryFromFile( file, b, e, result->offsets); 
        return result;
    }

    /////////////////////////////
    //
    //added by sujingxiu
    //
    /*
    bool queryFromFile(const char* file, const KEY& b, const KEY& e, std::vector<OFFSET>& vec){
    return index->queryFromFile( file, b, e, vec);
    }*/
    ///////////////////////////////////

    OffsetsResult* queryFromMem( const KEY& b, const KEY& e) {
        OffsetsResult* result = new OffsetsResult();
        index->queryFromMem( b, e, result->offsets);
        return result;
    }

    int readFromFile( const char * fileName ) {
        return index->readFromFile( fileName );
    }

    void lock() {
        pthread_mutex_lock( &mutex_ );
    }

    void unlock() {
        pthread_mutex_unlock( &mutex_ );
    }

    Trie<KEY>* getIndex(){
        return index;
    }
protected:
    typedef std::pair<KEY,VALUE> KVPR;

#ifdef BTREE
	BTree<KEY,VALUE>*       index;
#else
	Trie<KEY>*			    index;
#endif

	pthread_mutex_t			mutex_;
    std::vector<KVPR>       records_; 
    std::string             index_file_;
};


//TODO: need to fix multi-threads synchronization problems
class Index {
public:

	Index(const std::string &name,  uint16_t flag = SRCIP|DSTIP|SPORT|DPORT|PROTO|TIME ) ;
	
	~Index();

	bool getExitSignal() {
		pthread_rwlock_rdlock( &exit_mutex );
		bool ret = exit_signal;
		pthread_rwlock_unlock( &exit_mutex );
		return ret;
	}

	void setExitSignal() {
		pthread_rwlock_wrlock( &exit_mutex );
		exit_signal = true;
		pthread_rwlock_unlock( &exit_mutex );
	}
	
	uint16_t getWriteIndexFlag() {
		pthread_rwlock_rdlock( &write_index_lock );
		uint16_t ret = write_index_flag;
		pthread_rwlock_unlock( &write_index_lock );
		return ret;
	}

	void setWriteIndexFlag() {
		pthread_rwlock_wrlock( &write_index_lock);
		write_index_flag = indexFlag;
		pthread_rwlock_unlock( &write_index_lock);
	}
	
	void resetWriteIndexFlag(INDEXCODE code) {
		pthread_rwlock_wrlock( &write_index_lock);
		write_index_flag ^= code;
		pthread_rwlock_unlock( &write_index_lock);
	}

	void buildIndex( INDEXCODE code );

    OffsetsResult* queryFromFile(INDEXCODE code, const char *file,
            const uint32_t& begin, const uint32_t& end);
    
    OffsetsResult* queryFromMem(INDEXCODE code, const uint32_t& b, const uint32_t& e);
	//void clear_linkNodePool() ;

    void addRecord(const Ipv4Record& record, const VALUE& position);

    void writeIndexToFile( INDEXCODE code);
		
	friend void *index_thread_start( void  *);

	//friend class Exporter;

private:
	IndexBase<uint32_t>*	sipIndex;
	IndexBase<uint32_t>*	dipIndex;
	IndexBase<uint32_t>*	timeIndex;
	IndexBase<uint16_t>*	sportIndex;
	IndexBase<uint16_t>*	dportIndex;
	IndexBase<uint8_t>*	    protoIndex;

	//ResourcePool<Record*,1000000>	linkNodePool;
    // represent which fileds should be indexed
	uint16_t					indexFlag; 
	vector<pthread_t>			threadVec;
	//pthread_mutex_t				queueLock;
	//pthread_cond_t				emptyCond;
	//pthread_cond_t				fullCond;
    // represent which index have read the queue or not
	//int16_t						readCode; 
	
    pthread_rwlock_t        write_index_lock;
    uint16_t                write_index_flag;

	// signal for thread exiting
    pthread_rwlock_t		exit_mutex;
    bool					exit_signal;

    std::string             storage_name;
};

// end for namespace 
}

#endif
