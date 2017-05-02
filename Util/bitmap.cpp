/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified: 2012-11-14 15:01
#
# Filename: bitmap.cpp
#
# Description: 
#
===========================================================================*/
#include "bitmap.h"
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <algorithm>

uint8_t num2pos_table[256] = {
	-1,0,1,-1,2,-1,-1,-1,3,-1,-1,-1,-1,-1,-1,-1,4,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,5,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,6,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,7,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
};

inline int num2pos( uint32_t num ) {
	int ret = 0;
	while( num >= 256 ) {
		ret += 8;
		num >>= 8;
	}
	assert( num2pos_table[num] >= 0);
	return ret + num2pos_table[num];
}

Bitmap::Bitmap( uint64_t _bitSize ) :bitSize( _bitSize ) {
	assert( bitSize > 0 );

	elemSize = (uint32_t)( (bitSize+BITMASK) >> RIGHT_SHIFT_NUM ); 
	byteArray = new uint32_t[elemSize];
	memset( byteArray, 0 , elemSize*sizeof(uint32_t) );
	bitSize = elemSize << RIGHT_SHIFT_NUM;

	if( !byteArray ) {
		ERROR_INFO("Cannot alloc memory for bitmap",);
	}
}

Bitmap::~Bitmap() {
	if( byteArray != NULL ) delete[] byteArray;
}
	
void Bitmap::merge( const Bitmap& bm ) {
	for( int i = 0 ; i < bm.elemSize && i < elemSize; ++i ) {
		byteArray[ i ] |= bm.byteArray[i];
	}
}

void Bitmap::copy( Bitmap & bm ) {
	for( int i = 0 ; i < bm.elemSize && i < elemSize; ++i ) {
		byteArray[i] = bm.byteArray[i];
	}
}

void Bitmap::clear(){
	memset( byteArray, 0 , elemSize );
}

int Bitmap::dump2array( void *dst , int32_t byteLen ) const {
	assert( dst && byteLen >= 0 );

	if( !dst || byteLen < 0 ) {
		return -1;
	}
	byteLen = std::min( byteLen, getByteSize() );
	memcpy( dst, (void *)byteArray, byteLen );
	return 0;
}

int Bitmap::dump2file( int fd, int32_t offset ) const {
	assert( offset >= 0 );
	if( offset < 0 ) {
		return -1;
	}

	// seek target position that have a distance offset from current position defaultly. 
	off_t ret = lseek( fd, offset, SEEK_CUR );
	if( ret == -1 ) {
		ERROR_INFO( strerror(errno),return -1);
	}

	if( -1 == write( fd, (void *)&bitSize, sizeof(bitSize)) ) {
		ERROR_INFO( strerror(errno), return -1);
	}

	if( -1 == write( fd, (void *)&elemSize, sizeof(elemSize))) {
		ERROR_INFO( strerror(errno), return -1);
	}

	char *wp = (char *)byteArray;
	int  wsize = elemSize * sizeof( int32_t );

	while( 1 ) {
		ret = write( fd, (void *)wp, wsize ); 
		if( ret == -1 ) {
			ERROR_INFO( strerror(errno), return -1);
		}
		if( ret >= wsize ) break;
		wp += ret;
		wsize -= ret;
	}
	return 0;
}

int Bitmap::readFromFile( int fd, int32_t offset ) {
	assert( offset >= 0 );
	if( offset < 0 ) {
		return -1;
	}

	// seek target position that have a distance offset from current position defaultly. 
	int ret = lseek( fd, offset, SEEK_CUR );
	if( ret == -1 ) {
		ERROR_INFO( strerror(errno), return -1);
	}

	if( read( fd, (void *)&bitSize, sizeof(bitSize)) == -1 ) {
		ERROR_INFO( strerror(errno), return -1);
	}
	
	if( read( fd, (void *)&elemSize, sizeof(elemSize)) == -1 ) {
		ERROR_INFO( strerror(errno), return -1);
	}

	if( !byteArray ) delete[] byteArray;
	byteArray = new uint32_t[ elemSize ];

	char *rp = (char *)byteArray;
	int rsize = elemSize * sizeof( int32_t );

	while(1) {
		ret = read( fd, (void *)rp, rsize );
		if( ret == -1 ) {
			ERROR_INFO( strerror(errno), return -1);
		}

		if( ret >= rsize ) break;
		rp += ret;
		rsize -= ret;
	}
	return 0;
}


// file io
int Bitmap::dump2file( FILE * fp ) const {
	if( !fp ) {
		ERROR_INFO("NULL file pointer fp",return -1);
	}

	fwrite( &elemSize, sizeof(elemSize), 1, fp);
	fwrite( byteArray, sizeof(int32_t)*elemSize, 1, fp );
	return 0;
}


int Bitmap::readFromFile( FILE * fp ) {
	if( !fp ) {
		ERROR_INFO("NULL file pointer fp",return -1);
	}
	
	fread( &elemSize, sizeof(elemSize), 1, fp);
	// 32 when used uint32_t
	bitSize = elemSize * 32;

	if( !byteArray ) delete[] byteArray;
	byteArray = new uint32_t[ elemSize ];

	fread( byteArray, sizeof(uint32_t)*elemSize, 1, fp );
	return 0;
}

void Bitmap::getOnePosition( uint32_t *pos) {
	assert(pos != NULL);
	int cnt = 0;
	for( int i = 0 ;i < elemSize; ++i ) {
		uint32_t tmp = byteArray[i];
		while( tmp ) {
			pos[cnt++] = (i<<RIGHT_SHIFT_NUM) + num2pos( tmp&(-tmp) );	
			tmp ^= tmp&(-tmp);
		}
	}
}
