/*=============================================================================
#     FileName: hash_bucket.h
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-11 16:40:52
#      History:
=============================================================================*/

#ifndef HASH_BUCKET_H_
#define HASH_BUCKET_H_

#include "capture/packet.h"
#include <list>
#include <pthread.h>
#include <stdio.h>

using namespace capture;


// NOTE: iterator is not thread-safe
// so lock()/unlock() should be call when used
class HashBucketIterator {
public:
    HashBucketIterator() {}

    HashBucketIterator( std::list<PacketsItem*>::iterator it): iter(it) {}

    HashBucketIterator( const HashBucketIterator& other ) {
        iter = other.getIterator(); 
    }

    ~HashBucketIterator() {}

    PacketsItem& operator*() const {
        return *(*iter);
    }

    PacketsItem* operator->() const {
        return *iter;
    }

    HashBucketIterator& operator++() {
        ++iter;
        return *this;
    }

    HashBucketIterator operator++(int) {
        HashBucketIterator tmp( *this );
        ++ (*this);
        return tmp;
    }

    std::list<PacketsItem*>::iterator getIterator() const {
        return iter;
    }

    bool operator!= (const HashBucketIterator& other) const {
        return iter != other.getIterator();
    }

private:
    std::list<PacketsItem*>::iterator iter;

};


// TODO: thread-safe, may use read/write mutex
class HashBucket {
private:
    std::list<PacketsItem*> items_;
    pthread_mutex_t     mutex_;

public:
    HashBucket() {
        pthread_mutex_init( &mutex_, NULL);
    }

    ~HashBucket() {
        pthread_mutex_destroy( &mutex_ );
        for (std::list<PacketsItem*>::iterator it = items_.begin();
                it != items_.end(); ++it) {
            delete *it;
        }
    }

    void addPacket( PacketSlice* packet );

    HashBucketIterator deleteItem( const HashBucketIterator& iter );

    void lock() {
        pthread_mutex_lock( &mutex_ );
    }

    void unlock() {
        pthread_mutex_unlock( &mutex_ );
    }

    HashBucketIterator begin() {
        return HashBucketIterator( items_.begin() ); 
    }

    HashBucketIterator end() {
        return HashBucketIterator( items_.end() );
    }

    friend class HashBucketIterator;
};


#endif
