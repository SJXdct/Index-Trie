/*=============================================================================
#     FileName: hash_table.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-12 10:39:46
#      History:
=============================================================================*/

#include "hash_table.h"

HashTable* HashTableInstance:: hash_table_ = NULL;
pthread_mutex_t HashTableInstance:: mutex_ = PTHREAD_MUTEX_INITIALIZER;

void HashTable::addPacket( PacketSlice* packet ) {
    uint32_t hash_code = packet->hashCode();
    hash_code %= table_size_;
    buckets[hash_code].addPacket( packet );
}


HashTable* HashTableInstance::getInstance() {
    if ( hash_table_ == NULL ) {
        lock();
        if (hash_table_ == NULL ) {
            hash_table_ = new HashTable();
        }
        unlock();
    }
    assert( NULL != hash_table_ );
    return hash_table_;
}


