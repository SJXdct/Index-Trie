/*=============================================================================
#     FileName: hash_bucket.cpp
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Jingxiu Su
#        Email: sujingxiu@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-11 19:58:26
#      History:
=============================================================================*/

#include "hash_bucket.h"

void HashBucket::addPacket( PacketSlice* packet ) {
    // TODO: replace the mutex by better solution
    lock();
    for( std::list<PacketsItem*>::iterator it = items_.begin();
            it != items_.end(); ++it) {
        if ( packet->equalsTo( (*it)->five_tuple_ )) {
            (*it)->addPacket( packet );
            unlock();
            return ;
        }
    }

    PacketsItem* item = new PacketsItem();
    item->addPacket( packet );
    items_.push_back( item );
    unlock();
}

HashBucketIterator HashBucket::deleteItem( const HashBucketIterator& iter ) {
    // TODO:
    lock();
    std::list<PacketsItem*>::iterator it = iter.getIterator();
    // release all packets
    delete *it;

//#define DEBUG_HASHBUCKET
#ifdef DEBUG_HASHBUCKET
    fprintf( stdout, "Size of bucket is %d\n", items_.size() );
#endif
    it = items_.erase( it );
#ifdef DEBUG_HASHBUCKET
    fprintf( stdout, "Size of bucket is %d\n", items_.size() );
#endif

    unlock();
    return HashBucketIterator( it );
}
