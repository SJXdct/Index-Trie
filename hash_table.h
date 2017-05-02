/*=============================================================================
#     FileName: hash_table.h
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-12 09:58:49
#      History:
=============================================================================*/

#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_


#include "hash_bucket.h"
#include "capture/packet.h"

#include <pthread.h>
#include <assert.h>

using namespace capture;

class HashTable {
public:
    HashTable( int size = 100000): table_size_(size) {
        buckets = new HashBucket[table_size_];
    }

    ~HashTable() {
        delete[] buckets;
    }

    void addPacket(PacketSlice* packet);

    HashBucket& operator[](uint32_t idx) {
        assert( idx < table_size_ );
        return (buckets[idx]);
    }

    const HashBucket& operator[](uint32_t idx) const {
        assert( idx < table_size_ );
        return (buckets[idx]);
    }

    size_t size() {
        return table_size_;
    }


private:
    HashBucket*  buckets;
    uint32_t     table_size_;
};


class HashTableInstance {
public:
    static void lock() {
        pthread_mutex_lock( &mutex_ );
    }

    static void unlock() {
        pthread_mutex_unlock( &mutex_ );
    }

    static HashTable* getInstance();

    static void deleteInstance() {
        lock();
        delete hash_table_;
        hash_table_ = NULL;
        unlock();
    }


private:
    static HashTable* hash_table_;
    static pthread_mutex_t mutex_;
};


#endif
