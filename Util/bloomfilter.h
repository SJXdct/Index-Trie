/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified: 2012-11-14 15:16
#
# Filename: bloomfilter.h
#
# Description: 
#	N	element number;
#	B	bit size of bitmap；
#	P   the maximum false positive probability that we can endure
#	K	the number of hash functions
#	then:	K = -log(P)/log(2) , B = N*K/log(2)
===========================================================================*/

#ifndef _BLOOMFILTER_H
#define _BLOOMFILTER_H

#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include <errno.h>
#include <vector>
#include "bitmap.h"
#include "../global.h"

#define	defaultElemNum  10000000

template<typename Type>
class BloomFilter {
	public:
		/* HASHFUNC means a typename of hash function */
		typedef	uint32_t (*HASHFUNC)( const Type& elem ) ;
		/*
		BloomFilter() {
		} */

		/* element number must be a 32-bit unsigned integer, so we can hold 4G elements at most */

		BloomFilter( int32_t  _elemNum = defaultElemNum );
		
		BloomFilter( int32_t _elemNum, int32_t _hashNum );

		~BloomFilter();

		/* initialization of bloom filter including creating bitmap and bind hash functions */
		int init();

		/* insert an element into bloom filter */
		int insertElement( Type& elem ) ;

		/* checking an element is in this set or not */
		bool containElement( Type& elem ) const ; 

		/*  change the idx-th hash function to hashFunc */
		void setHashFunc( HASHFUNC hashFunc, int32_t idx ) ;

		int  getHashFuncNum() {
			return hashFuncNum;
		}
		/* merge another bloomfilter's information into current one, 
		 * we must make sure that bit-size of two BloomFilters' bitmap be the same */ 
		void merge( const BloomFilter<Type>& bf ) ;

		void copyBitmap( const BloomFilter<Type>& bf ); 

		int32_t	getByteSize() ;

		/*    storage layout in file
		 *	  |--------------------|
		 *	  | elementNum         |
		 *	  | hashFuncNum        |
		 *	  | bitSize of bitamp  |
		 *	  | bitmap object      |
		 *	  |--------------------|
		 *	  we did not store information of hash functions to file,
		 *	  so we must configure the hash functions after we reading BloomFilter from file.
		 */
		// standard IO
		int dump2file( int fd , int32_t offset = 0, int whence = SEEK_CUR );

		int readFromFile( int fd, int32_t offset = 0, int whence = SEEK_CUR );

		// file IO
		int dump2file( FILE * fp );
		int readFromFile(FILE * fp );

	private:

		static const double		falsePositive = 0.1;		/* default probability of false positive in matching */
		int32_t					elementNum;					/* the number of elements may in this set */
		int32_t					hashFuncNum;				/* the number of hash function */
		Bitmap*					bitmap;						/* bitmap */
		std::vector<HASHFUNC>	hashFuncVec;				/* set of hash functions */ 
};

template<typename Type>
/* implementation of BloomFilter */
BloomFilter<Type>::BloomFilter( int32_t _elemNum ):elementNum(_elemNum){
	hashFuncNum = (int32_t)( -std::log(falsePositive)/std::log(2) );

	int ret = init();
	if( ret ) {
		ERROR_INFO("Create BloomFilter failed", );		
	}
}

template<typename Type>
BloomFilter<Type>::BloomFilter(int32_t _elemNum, int32_t _hashNum):elementNum(_elemNum),\
		hashFuncNum( _hashNum ) {
	int ret = init();
	if( ret ) {
		ERROR_INFO("Create BloomFilter failed", );		
	}
}

template<typename Type>
BloomFilter<Type>::~BloomFilter() {
	if( bitmap != NULL ) delete bitmap ; 
}


template<typename Type>
int BloomFilter<Type>::init() {
	int64_t bitSize = (int64_t)(hashFuncNum/std::log(2)*elementNum);
	bitmap = new Bitmap( bitSize );
	hashFuncVec.resize( hashFuncNum );
	return 0;
}

template<typename Type>
/* we must set hash functions before we using bloom filter */
void BloomFilter<Type>::setHashFunc( HASHFUNC hashFunc, int32_t idx ) {
	assert( idx >= 0 && idx < hashFuncVec.size() );
	if( idx < 0 || idx >= hashFuncVec.size() ) {
		ERROR_INFO("Invalid index when setting hash function of BloomFilter", );		
	}
	hashFuncVec[idx] = hashFunc;
}

template<typename Type>
int BloomFilter<Type>::insertElement( Type& elem ) {
	/* because the number of actually hash functions may be less than number of current 
	 * bloomfilter's needed hash functions, we would use one hash function more than once
	 * then we add an additional corrected value i to make sure independence of each hash result */
	for(int i = 0; i < hashFuncNum; ++i ) {
		uint32_t hashCode = (hashFuncVec[i])(elem) + i;
		hashCode %= bitmap->getBitSize();
		bitmap->setBit( hashCode );
	}
}

template<typename Type>
bool BloomFilter<Type>::containElement( Type& elem )const {
	for(int i = 0; i < hashFuncNum; ++i ) {
		uint32_t hashCode = (hashFuncVec[i])(elem) + i;
		hashCode %= bitmap->getBitSize();
		if( !bitmap->getBit(hashCode) ) return false;
	}
	return true;
}

template<typename Type>
/* byte size we need store into file */
int32_t BloomFilter<Type>::getByteSize(){
	int32_t ret = 0;
	ret += sizeof( elementNum );
	ret += sizeof( hashFuncNum );
	ret += sizeof( bitmap->getBitSize() );
	ret += bitmap->getByteSize();
	return ret;
}

template<typename Type>
void BloomFilter<Type>::merge( const BloomFilter<Type> & bf ) { 
	assert( bitmap->getBitSize() == bf.bitmap->getBitSize() );
	if( bitmap->getBitSize() != bf.bitmap->getBitSize() ) {
		ERROR_INFO("Failed merging two bloomfilters that have different bitSize", );		
	}
	bitmap->merge( bf.bitmap );
}


template<typename Type>
void BloomFilter<Type>::copyBitmap( const BloomFilter<Type>& bf ) {
	bitmap->copy( bf.bitmap );
}

template<typename Type>
int BloomFilter<Type>::dump2file( int fd, int32_t offset, int whence ) {
	assert( offset >= 0 );

	if( offset < 0 ) return -1;

	off_t pos = lseek( fd, offset, whence );
	if( pos == -1 ) {
		ERROR_INFO("seek position of file failed", return -1 );		
	}

	write( fd, (void *)&elementNum, sizeof(elementNum) );
	write( fd, (void *)&hashFuncNum, sizeof(hashFuncNum) );

	/*
	int64_t bitSize = bitmap->getBitSize();
	ret = write( fd, (void *)&bitSize, sizeof(bitSize) );
	if( ret == -1 ) {
		fprintf( stderr, "error msg %s in file %s, func = %s\n", strerror( errno), __FILE__, __func__ );
		return -1;
	}
	*/
	
	if( bitmap->dump2file( fd ) == -1 ) {
		ERROR_INFO( strerror(errno),return -1);		
	}
	return 0;
}

template<typename Type>
int BloomFilter<Type>::readFromFile(int fd, int32_t offset, int whence ){
	assert( offset >= 0 );

	if( offset < 0 ) return -1;

	off_t pos = lseek( fd, offset, whence );
	if( pos == -1 ) {
		ERROR_INFO("seek position of file failed",return -1 );		
	}
	
	read( fd, (void *)&elementNum, sizeof(elementNum) );
	read( fd, (void *)&hashFuncNum, sizeof(hashFuncNum) );
	/*
	int64_t bitSize ; 
	ret = read( fd, (void *)&bitSize, sizeof(bitSize) );
	if( ret == -1 ) {
		fprintf( stderr, "error msg %s in file %s, func = %s\n", strerror( errno), __FILE__, __func__ );
		return -1;
	}

	// we need realloc memory for bitmap, because the bitSize read from file may be different
	// with that we initialized in constructing this BloomFilter object;
	if( bitmap != NULL ) delete bitmap;
	bitmap = new Bitmap( bitSize );
	*/

	hashFuncVec.resize( hashFuncNum );

	if( bitmap->readFromFile( fd ) == -1 ) {
		ERROR_INFO( strerror(errno),return -1);		
	}
	return 0;
}

// file io
template<typename Type>
int BloomFilter<Type>::dump2file( FILE* fp) {

	if( !fp ) {
		ERROR_INFO("NULL pointer fp",return -1);		
	}

	fwrite( (void *)&elementNum, sizeof(elementNum), 1, fp);
	fwrite( (void *)&hashFuncNum, sizeof(hashFuncNum), 1, fp );
	
	if( bitmap->dump2file( fp ) == -1 ) {
		ERROR_INFO( strerror(errno),return -1);		
	}
	return 0;
}

template<typename Type>
int BloomFilter<Type>::readFromFile( FILE* fp ){
	if( !fp ) {
		ERROR_INFO("NULL pointer fp",return -1);		
	}
	
	fread( (void *)&elementNum, sizeof(elementNum), 1, fp );
	fread( (void *)&hashFuncNum, sizeof(hashFuncNum), 1, fp );

	hashFuncVec.resize( hashFuncNum );

	assert( bitmap != NULL );
	if( bitmap->readFromFile( fp ) == -1 ){
		ERROR_INFO( strerror(errno),return -1);		
	}
	return 0;

}
#endif
