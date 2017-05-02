#include "string_hash.h"

StringHash* StringHash::hashSet = NULL;

uint32_t StringHash::BKDRHash( const string& str ){
	register uint32_t hash = 0;

	for( int i = 0 ; i < str.length() ; ++i ) {
		hash = hash * 131 + str[i];
	}
	return hash;
}

uint32_t StringHash::RSHash( const string& str ) {
	register uint32_t hash = 0;
	uint32_t magicNumber = 63689;

	for( int i = 0 ; i < str.length(); ++i ) {
		hash = hash * magicNumber + str[i];
		magicNumber *= 378851;
	}
	return hash;
}

uint32_t StringHash::APHash( const string & str ) {
	register uint32_t hash = 0;

	for( int i = 0; i < str.length(); ++i ){
		if( i&1 ) {
			hash ^= (~( (hash<<11) ^ str[i] ^ (hash>>5) ));
		} else {
			hash ^= ( (hash<<7) ^ str[i] ^ (hash>>3) );
		}
	}
	return hash;
}

uint32_t StringHash::JSHash(const string& str) {
	register uint32_t hash = 1315423911;

	for( int i = 0 ; i < str.length(); ++i ) {
		hash ^= ( hash<<5 ) + str[i] + (hash>>2) ;
	}
	return hash;
}

uint32_t StringHash::DJBHash( const string & str ) {
	register uint32_t hash = 5381;

	for( int i = 0; i < str.length(); ++i ) {
		hash += ( hash << 5) + str[i];
	}
	return hash;
}

STRHASH StringHash::getHashFunc( int32_t idx ){
	idx %= 5;
	STRHASH ret = NULL;
	switch( idx ) {
		case 1: ret =(STRHASH)& BKDRHash; break;
		case 2: ret =(STRHASH)& RSHash; break;
		case 3: ret =(STRHASH)& APHash; break;
		case 4: ret =(STRHASH)& JSHash; break;
		case 0: ret =(STRHASH)& DJBHash; break;
	}

	return ret; 
}
