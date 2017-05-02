/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified: 2012-11-26 09:42
#
# Filename: string_hash.h
#
# Description: hash functions set of string
#
===========================================================================*/

#ifndef _STRING_HASH
#define _STRING_HASH

#include <stdint.h>
#include <string>

using  std::string; 

typedef uint32_t (* STRHASH )( const string & );

// singleton mode
class StringHash {
	private:
		static StringHash * hashSet;
		StringHash() {
		}

		class ReleaseMem {
			public:
				ReleaseMem(){
				}

				~ReleaseMem() {
					if( !hashSet ) delete hashSet;
				}
		};
		// local variable to release memory of StringHash object;
		ReleaseMem release;


	public:
		static StringHash* getHashPtr(){
			if( !hashSet ) {
				hashSet = new StringHash();
			}
			return hashSet;
		}

		static	uint32_t BKDRHash( const string& str );

		static	uint32_t RSHash( const string& str );

		static	uint32_t APHash( const string& str );

		static	uint32_t JSHash( const string& str );

		static	uint32_t DJBHash( const string& str );

		static STRHASH getHashFunc( int32_t idx ); 
};

#endif
