/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified: 2012-11-29 13:50
#
# Filename: btree_node.h
#
# Description: 
#
===========================================================================*/

#ifndef _BTREE_NODE_H
#define _BTREE_NODE_H

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "../global.h"
#include "linklist.h"

#define MAX_PER_NODE 128

// type KEY must have follow properties:
// 1. supporting comparison( > , < , = ) operation
// 2. supporting sizeof operation

template<typename KEY>
class BTreeNode {
	protected:
		uint8_t	elemCount; /* the elemCount of each BTreeNode must be less than min( 256, MAX_PER_NODE ) */
		KEY 	keyArray[MAX_PER_NODE];  /* the array stored the key values */
		int8_t 	isLeaf;		/* flag to represent current Node is leaf or internal node */
		int8_t	depth;		/* depth in the tree, unused at present */

	public:
		BTreeNode( int8_t  _leaf = 0 ): isLeaf( _leaf ) {
			elemCount = 0;
			depth = 0;
		}

		/* deconstructor function should be virtual */
		virtual ~BTreeNode()  {
		}

		// find a position k satisfied keyArray[k] >= key and we will return the lastposition when no position satisfy that condition  
		virtual int findPosition( const KEY& key )  {
			if( elemCount == 0 ) return 0;

			// If the query key is greater than the max key of this BTreeNode,
			if( keyArray[elemCount-1] < key ) {
				return -1;
			}

			int l = 0 , r = elemCount-1;
			while( l < r ) {
				int mid = (l+r)>>1;
				if( keyArray[mid] > key ) {
					r = mid;
				} else if( keyArray[mid] < key ) {
					l = mid + 1;
				} else  return mid;
			}

			// if no element in array, we will return position 0 because l = 0 and r = -1;
			return l;
		}

#ifndef USE_FILEIO
		virtual int  write2file( int fd, off_t offset = 0, int whence = SEEK_CUR )=0; 
		virtual int  readFromFile( int fd, off_t offset = 0, int whence = SEEK_CUR )= 0;
#else
		virtual int  write2file( FILE * fp )=0; 
		virtual int  readFromFile( FILE * fp )= 0;
#endif
		virtual int  getSizeOfFile() = 0; 

		virtual void copyItems( BTreeNode<KEY>* ptr, int32_t start, int32_t n ) {
			for( int i = 0 ; i < n &&  elemCount < MAX_PER_NODE ; ++i ) {
				keyArray[elemCount++] = ptr->getKey( start + i );
			}
		}

		inline void setDepth( int _depth ) {
			depth = _depth;
		}

		inline void incDepth() {
			++depth;
		}

		inline int getElemCount() {
			return elemCount;
		}

		inline bool isLeafNode() {
			return isLeaf != 0;
		}

		inline void setElemCount( int32_t cnt ) {
			assert( cnt >= 0 && cnt <= MAX_PER_NODE );
			elemCount = cnt;
		}

		inline KEY getKey( int32_t idx ) {
			assert( idx >= 0 && idx < elemCount );
			return keyArray[idx];
		}

		inline void setKey( int32_t idx, const KEY& key ) {
			assert( idx >= 0 && idx < elemCount );
			keyArray[ idx ] = key;
		}
};

// implementation of internal btree node 
template<typename KEY>
class InternalNode: public BTreeNode<KEY> {
	private:
		typedef BTreeNode<KEY>*	BTreeNodePtr;
		BTreeNodePtr		childPtr[MAX_PER_NODE];
		uint32_t*			fileOffArray; // stored the offset-array of file, used when write BTreeNode into file.

	public:
		InternalNode() : BTreeNode<KEY>(0) {
			for( int i = 0 ; i < MAX_PER_NODE; ++i ) childPtr[i] = NULL;
			fileOffArray = NULL;
		}

		~InternalNode() {
		}

		int addItem( const KEY& key, BTreeNodePtr child, int pos = -1 ) {
			
			if( BTreeNode<KEY>::elemCount >= MAX_PER_NODE ) {
				return -1;
			}

			// the caller have not setted the position when pos = -1;
			if( pos == -1 ) pos = findPosition( key );	

			// key > max key of this Node; 
			if( pos == -1 ) {
				pos = this->elemCount ;
			} else {
				// insertion sort procedure
				for( int i = BTreeNode<KEY>::elemCount; i > pos ; --i ) {
					BTreeNode<KEY>::keyArray[i] = BTreeNode<KEY>::keyArray[i-1];
					childPtr[i] = childPtr[i-1];
				}
			}
			BTreeNode<KEY>::keyArray[pos] = key;
			childPtr[pos] = child;
			++BTreeNode<KEY>::elemCount;
			return 0;
		}
	
		void copyItems( BTreeNodePtr ptr, int32_t start, int32_t n ) {
			BTreeNode<KEY>::copyItems( ptr, start, n );

			for( int i = 0 ; i < n && i < MAX_PER_NODE; ++i ) {
				childPtr[i] = ((InternalNode<KEY>*)ptr)->getChildren( start + i );
			}

		}

		BTreeNodePtr getChildren( int idx ) {
			assert( idx >= 0 && idx < BTreeNode<KEY>::elemCount );
			return childPtr[ idx ];
		}

		void setChildren( int idx, BTreeNodePtr ptr ) {
			assert( idx >= 0 && idx < BTreeNode<KEY>::elemCount );
			childPtr[idx] = ptr;
		}

		// we must call this function before we calling function write2file 
		void setFileOffset( uint32_t *ptr ) {
			assert( ptr != NULL );
			fileOffArray = ptr;
		}

		/* layout of storage in file
		 * |-------------------------|
		 * |   isLeaf	             |
		 * |   elemCount             |
		 * |   1st key 				 | 
		 * |   2nd key    			 | 
		 * |   .........             |
		 * |   last key              |
		 * |   1st child offset      |
		 * |   2nd child offset      |
		 * |   .........             |
		 * |   last child offset     |
		 * |-------------------------|
		 */
		// we must have given a realistic array address to the variable fileOffArray 
		// before we calling the funtion write2file, because we cannot get the writing offset 
		// of each child BTreeNode in file in real-time. So we must calculate the offset of each child BTreeNode
		// and store this value into an array in advance .
		// NOTE: size of single BTree file must be less than 4G because offset is int32_t type 
		
#ifndef USE_FILEIO
		int write2file( int fd, off_t offset = 0 , int whence = SEEK_CUR ) {
			// we will seek from current position defaultly.
			// offset must be non-negative intergers when whence = SEEK_CUR
		    assert( fd >= 0 );	
			off_t pos = lseek( fd, offset, whence) ;
			if( pos < 0 ) {
				ERROR_INFO("reseeking file handler position failed",return -1);
			}

			write( fd, &(this->isLeaf), sizeof(BTreeNode<KEY>::isLeaf));
			write( fd, &(this->elemCount), sizeof(BTreeNode<KEY>::elemCount));
			write( fd, BTreeNode<KEY>::keyArray, sizeof(KEY) * BTreeNode<KEY>::elemCount );
			// we need to calculate the children's offset of file in advance. 
			assert( fileOffArray != NULL );
			write( fd, fileOffArray, sizeof( uint32_t ) * BTreeNode<KEY>::elemCount );
			fileOffArray = NULL; // make sure that each calling of this function should call function setFileOffset firstly.
			return 0;
		}

		int readFromFile( int fd, off_t offset = 0, int whence = SEEK_CUR ) {
			// we will seek from current position defaultly.
			assert( fd >= 0 );	
			off_t pos = lseek( fd, offset, whence) ;
			if( pos < 0 ) {
				ERROR_INFO("reseeking file handler position failed",return -1);
			}

			//we have readed isLeaf value before we call this function
			//read( fd, &(this->isLeaf), sizeof(BTreeNode<KEY>::isLeaf));
			read( fd, &(this->elemCount), sizeof(BTreeNode<KEY>::elemCount));
			read( fd, BTreeNode<KEY>::keyArray, sizeof(KEY) * BTreeNode<KEY>::elemCount );
			return 0;
		}
#else 
		// file IO
		int write2file( FILE * fp) {
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}

			fwrite( &(this->isLeaf), sizeof(BTreeNode<KEY>::isLeaf) , 1, fp);
			fwrite( &(this->elemCount), sizeof(BTreeNode<KEY>::elemCount), 1, fp);
			fwrite( BTreeNode<KEY>::keyArray, sizeof(KEY) * BTreeNode<KEY>::elemCount , 1, fp);

			// we need to calculate the children's offset of file in advance. 
			assert( fileOffArray != NULL );
			fwrite( fileOffArray, sizeof( uint32_t ) * BTreeNode<KEY>::elemCount , 1, fp );
			fileOffArray = NULL; // make sure that each calling of this function should call function setFileOffset firstly.
			return 0;
		}

		int readFromFile( FILE *fp ) {
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}
			//we have readed isLeaf value before we call this function
			//read( fd, &(this->isLeaf), sizeof(BTreeNode<KEY>::isLeaf));
			fread( &(this->elemCount), sizeof(BTreeNode<KEY>::elemCount), 1, fp);
			fread( BTreeNode<KEY>::keyArray, sizeof(KEY) * BTreeNode<KEY>::elemCount, 1, fp );
			return 0;
		}
#endif
		// get the total size that we writed this InternalNode Object in file
		int32_t getSizeOfFile() {
			int32_t ret = sizeof( BTreeNode<KEY>::elemCount ) + sizeof(this->isLeaf);
			ret += BTreeNode<KEY>::elemCount * sizeof( KEY ) ;
			ret += BTreeNode<KEY>::elemCount * sizeof( uint32_t ) ;
			return ret;
		}
};


// implementation of leaf b-tree node 
template<typename KEY, typename VALUE>
class LeafNode: BTreeNode<KEY> {
	private:

		LinkList<VALUE>*	resultArray[MAX_PER_NODE]; /* the matched result-list of each key */

	public:
		LeafNode() : BTreeNode<KEY>( 1 ) {
			for( int i = 0 ; i < MAX_PER_NODE; ++i ) resultArray[i] = NULL;
		}

		~LeafNode() {
			for( int i = 0 ; i < this->elemCount; ++i ) {
				assert( resultArray[i] != NULL );
				if( resultArray[i] ) 
					delete resultArray[i];
			}
		}

		int addItem( const KEY& key, const VALUE& value ) {

			// insertion sort procedure
			int pos = findPosition( key );

			// key has existed in this leaf node 
			if( pos >= 0 && pos < this->elemCount && BTreeNode<KEY>::keyArray[pos] == key ) {
				if( resultArray[pos] == NULL ) {
					printf("key = %u value = %u, pos = %d\n",key,value,pos);
					printf("ERROR in here with node pointer = %p\n",this);
				}
				assert( resultArray[pos] != NULL );
				resultArray[pos]->addElement( value );
				return 0;
			}

			// when this node is full
			if( BTreeNode<KEY>::elemCount >= MAX_PER_NODE ) {
				return -1 ;
			}

			// when added key is greater than all of existed keys
			if( pos == -1 ) {
				pos = this->elemCount;
			} else {
				// add a new key in this leaf node 
				for( int i = BTreeNode<KEY>::elemCount ; i > pos; --i ) {
					BTreeNode<KEY>::keyArray[i] = BTreeNode<KEY>::keyArray[i-1];
					resultArray[i] = resultArray[i-1];
				}
			}

			resultArray[pos] = new LinkList<VALUE>();
			if( !resultArray[pos] ) {
				ERROR_INFO("cannot alloc memory for linklist", return -1);
			}
			BTreeNode<KEY>::keyArray[pos] = key;
			resultArray[pos]->clear();
			resultArray[pos]->addElement(value);
			++BTreeNode<KEY>::elemCount;
			return 0;
		}


		void copyItems( BTreeNode<KEY>* ptr, int32_t start, int32_t n ) {
			BTreeNode<KEY>::copyItems( ptr, start, n );

			for( int i = 0 ; i < n && start+i < MAX_PER_NODE; ++i ) {
				resultArray[i] = ((LeafNode<KEY,VALUE>*)ptr)->getResult( start+i );
			}
		}


		LinkList<VALUE>* getResult( int32_t idx ) {
			assert( idx >= 0 && idx <= MAX_PER_NODE );
			assert( resultArray[idx] != NULL );
			return resultArray[idx];
		}

		/* layout of storage in file
		 * |-------------------------|
		 * |   isLeaf	             |
		 * |   elemCount             |
		 * |   1st key 				 | 
		 * |   1st values' linklist  |
		 * |   2nd key    			 | 
		 * |   2nd values' linklist  |
		 * |   .........             |
		 * |   .........             |
		 * |   last key              |
		 * |   last values' linklist |
		 * |-------------------------|
		 */

#ifndef USE_FILEIO
		int write2file( int fd, off_t offset = 0 , int whence = SEEK_CUR ) {
			// we will seek from current position defaultly.
		    assert( fd >= 0 );	
			off_t pos = lseek( fd, offset, whence) ;
			if( pos < 0 ) {
				ERROR_INFO("reseeking file handler position failed",return -1);
			}

			write( fd, &(this->isLeaf), sizeof(BTreeNode<KEY>::isLeaf));
			write( fd, &(this->elemCount), sizeof(BTreeNode<KEY>::elemCount));
			//write( fd, &BTreeNode<KEY>::elemCount, sizeof( BTreeNode<KEY>::elemCount ));
			for( int i = 0 ; i < BTreeNode<KEY>::elemCount ; ++i ) {
				write( fd, &(BTreeNode<KEY>::keyArray[i]), sizeof( KEY ) );
				resultArray[i]->write2file( fd );
			}
			return 0;	
		}

		int readFromFile( int fd, off_t offset = 0 , int whence = SEEK_CUR ) {
			// we will seek from current position defaultly.
		    assert( fd >= 0 );	
			off_t pos = lseek( fd, offset, whence) ;
			if( pos < 0 ) {
				ERROR_INFO("reseeking file handler position failed",return -1);
			}

			//we have readed isLeaf value before we call this function
			//read( fd, &(this->isLeaf), sizeof(BTreeNode<KEY>::isLeaf));
			read( fd, &(this->elemCount), sizeof(BTreeNode<KEY>::elemCount));
			//write( fd, &BTreeNode<KEY>::elemCount, sizeof( BTreeNode<KEY>::elemCount ));
			for( int i = 0 ; i < BTreeNode<KEY>::elemCount ; ++i ) {
				read( fd, &(BTreeNode<KEY>::keyArray[i]), sizeof( KEY ) );
				assert( resultArray[i] == NULL );
				resultArray[i] = new LinkList<VALUE>();
				resultArray[i]->readFromFile( fd );
			}
			return 0;
		}

#else 
		int write2file( FILE * fp) {
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}

			fwrite( &(this->isLeaf), sizeof(BTreeNode<KEY>::isLeaf), 1, fp);
			fwrite( &(this->elemCount), sizeof(BTreeNode<KEY>::elemCount), 1, fp);
			//write( fd, &BTreeNode<KEY>::elemCount, sizeof( BTreeNode<KEY>::elemCount ));
			for( int i = 0 ; i < BTreeNode<KEY>::elemCount ; ++i ) {
				fwrite( &(BTreeNode<KEY>::keyArray[i]), sizeof( KEY ), 1, fp );
				if( resultArray[i]->write2file( fp ) == -1 ) {
					return -1;
				}
			}
			return 0;	
		}

		int readFromFile( FILE * fp) {
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}
			//we have readed isLeaf value before we call this function
			//read( fd, &(this->isLeaf), sizeof(BTreeNode<KEY>::isLeaf));
			fread( &(this->elemCount), sizeof(BTreeNode<KEY>::elemCount), 1, fp);
			for( int i = 0 ; i < BTreeNode<KEY>::elemCount ; ++i ) {
				fread( &(BTreeNode<KEY>::keyArray[i]), sizeof( KEY ), 1, fp );
				assert( resultArray[i] == NULL );
				resultArray[i] = new LinkList<VALUE>();
				if( resultArray[i]->readFromFile( fp ) == -1 ) {
					return -1;
				}
			}
			return 0;
		}	
#endif

		// get the total size that we writed this LeafNode Object in file
		int32_t getSizeOfFile() {
			int32_t ret = sizeof( BTreeNode<KEY>::elemCount ) + sizeof( this->isLeaf);
			ret += BTreeNode<KEY>::elemCount * sizeof( KEY ) ;
			
			for( int i = 0 ; i < BTreeNode<KEY>::elemCount ; ++i ) {
				ret += resultArray[i]->getSizeOfFile();
			}
			return ret;
		}

};

#endif

