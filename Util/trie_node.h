/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified: 2012-11-29 13:50
#
# Filename: trie_node.h
#
# Description: 
#
===========================================================================*/

#ifndef _TRIE_NODE_H
#define _TRIE_NODE_H

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "../global.h"
#include "bitmap.h"
#include "linklist.h"

#define BITS_PER_NODE 8
#define MAX_PER_NODE (1<<BITS_PER_NODE)

struct LISTELEM {
    uint32_t offset;
    uint16_t packets_num;
};

class TrieNode {
	protected:
		int8_t 		isLeaf;		/* flag to represent current Node is leaf or internal node */
		Bitmap*  	bitmap;		// the ith bit indicate the ith children is NULL or not
		uint16_t	elemCount;
		uint32_t*	fileOffArray; // stored the offset-array of file, used when write TrieNode into file.
		uint32_t*	bitPosition;

	public:
		TrieNode( int8_t  _leaf = 0 ): isLeaf( _leaf ) {
			bitmap = new Bitmap(MAX_PER_NODE);
			fileOffArray = NULL;
			bitPosition = NULL;
			elemCount = 0;
		}

		/* deconstructor function should be virtual */
		virtual ~TrieNode()  {
			delete bitmap;
			if( bitPosition ) delete[] bitPosition ;
			if( fileOffArray) delete[] fileOffArray;
            //printf("TrieNode\n");
		}

		virtual void clear() {
			isLeaf = 0;
			elemCount = 0;
			fileOffArray = bitPosition = NULL;
			bitmap->clear();
		}

		virtual int  write2file( FILE * fp )=0; 
		virtual int  readFromFile( FILE * fp )= 0;
		virtual int  getSizeOfFile() = 0; 

		inline int getElemCount() {
			return elemCount;
		}

		inline bool isLeafNode() {
			return isLeaf != 0;
		}

		inline bool hasChildren( uint8_t & pos ) {
			assert( pos >= 0 && pos < MAX_PER_NODE );
			return bitmap->getBit( pos );
		}

		inline void setBit( const uint8_t& pos ) {
			bitmap->setBit( pos );
		}

		// we must call this function before we calling function write2file 
		inline void setFileOffset( uint32_t *ptr ) {
			assert( ptr != NULL );
			fileOffArray = ptr;
		}	

		inline uint8_t getOnePosition( uint8_t idx ) {
			assert( idx >= 0 && idx < elemCount );
			if( !bitPosition ) getBitPosition();
			return bitPosition[idx];
		}

		inline void getBitPosition() {
			bitPosition = new uint32_t[MAX_PER_NODE];
			bitmap->getOnePosition( bitPosition );
		}

		inline uint32_t* getFileOffset() {
			return fileOffArray;
		}
};

class InterTrieNode: public TrieNode{
	private:
		typedef TrieNode*	TrieNodePtr;
		TrieNodePtr 		childPtr[MAX_PER_NODE];

	public:
		InterTrieNode() : TrieNode(0) {
			for( int i = 0; i < MAX_PER_NODE; ++i ) childPtr[i] = NULL;
		}

		~InterTrieNode() {
		}

		void clear();

		int addItem( const uint8_t& pos, TrieNodePtr child) ;

		int write2file( FILE *fp );

		int32_t getSizeOfFile();

		int readFromFile( FILE * fp);

		inline	TrieNodePtr getChildren( const uint8_t& idx ) {
			assert( idx >= 0 && idx < MAX_PER_NODE );
			return childPtr[ idx ];
		}

		inline void setChildren( const uint8_t &idx, TrieNodePtr child ) {
			assert( idx >= 0 && idx < MAX_PER_NODE );
			childPtr[ idx ] = child;
		}

};

//typedef uint32_t VALUE;
typedef struct LISTELEM VALUE;

class LeafTrieNode: public TrieNode{
	private:

		LinkList<VALUE>*	resultArray[MAX_PER_NODE]; /* the matched result-list of each key */

	public:
		LeafTrieNode(); 

		~LeafTrieNode();

		void clear();

		int addItem( const uint8_t& pos, const VALUE& value );

		LinkList<VALUE>* getResult( const uint8_t idx );

		int write2file( FILE * fp);

		int readFromFile( FILE * fp);

		int readLinklist( FILE *fp );
	
		// get the total size that we writed this LeafNode Object in file
		int32_t getSizeOfFile();

};

#endif

