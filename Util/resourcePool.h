/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified: 2012-8-17 14:34
#
# Filename: resourcePool.h
#
# Description: 
#
===========================================================================*/

#ifndef RESOURCEPOOL_H_
#define RESOURCEPOOL_H_

#include <vector>
#include <assert.h>
#include <cstdio>

template<typename RES, int POOLSIZE>
class ResourcePool {
private:
	RES					queue[POOLSIZE];
	int					cursor;
	//	std::vector<RES>	free_queue;
public:
	ResourcePool() {
		cursor = 0;
	}

	~ResourcePool(){
	}

	inline RES get_free_resource() {
		if( cursor <= 0 ) return NULL;
		return queue[--cursor];
	}

	inline RES get_resource( int idx ) {
		assert( idx >= 0 && idx < cursor );
		return queue[idx];
	}

	inline int  set_resource_free( RES res) {
		return 	push_resource( res );
	}

	inline int  push_resource( RES res) {
		if( cursor >= POOLSIZE ) return -1;
		queue[cursor++] = res;
		return 0;
	}

	inline int	size() { return cursor;}
	inline int capacity() { return POOLSIZE; }

};

#endif
