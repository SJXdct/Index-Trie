/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified:	2012-12-11 15:05
#
# Filename:		btree.h
#
# Description: 
#
===========================================================================*/

#ifndef _BTREE_H
#define _BTREE_H

#include "btree_node.h"
#include "../global.h"
#include <stack>
#include <utility>
#include <errno.h>
#include <string.h>
#include <queue>
#include <iostream>
#include <sys/fcntl.h>
#include <sys/types.h>

using std::cout;
using std::stack;
using std::queue;
using std::pair;

template <typename KEY, typename VALUE>
class BTree {
	private:
		BTreeNode<KEY> * 	root;
		int8_t			treeDepth;
		int32_t			nodeCount;

		typedef pair< BTreeNode<KEY>*, int > PIPAIR;

	public:
		BTree() {
			root = NULL;
			nodeCount = 0;
			treeDepth = 0;
		}

		~BTree() {
			printf("nodeCount of BTree is %d\n",nodeCount);
			if( root ) {
				queue< BTreeNode<KEY>* > Q;
				Q.push( root );
				while( !Q.empty() ) {
					BTreeNode<KEY>* cur = Q.front();
					//printf("node pointer %p release\n",cur);
					Q.pop();

					for( int i = 0 ; !cur->isLeafNode() && i < cur->getElemCount(); ++i ) {
						Q.push( ((InternalNode<KEY>*)cur)->getChildren(i) );
					}
					delete cur;
					--nodeCount;
				}

				assert( nodeCount == 0 );
			}
		}

		// print detailed information of tree for debug 
		void outInfo() {
			if( !root ) {
				cout<<"This is an empty Tree"<<std::endl;
			} else {
				queue< BTreeNode<KEY>* > Q;
				Q.push( root );
				int ID = 0 ;

				cout<<"Tree's depth is "<<treeDepth<<std::endl;
				while( !Q.empty() ) {
					BTreeNode<KEY>* cur = Q.front();
					Q.pop();
					cout<<"Information of Node "<<ID++<<" has "<<cur->getElemCount()<<"Keys:"<<std::endl;

					for( int i = 0 ; !cur->isLeafNode() && i < cur->getElemCount(); ++i ) {
						Q.push( ((InternalNode<KEY>*)cur)->getChildren(i) );
					}

					for( int i = 0 ; i < cur->getElemCount(); ++i ) {
						cout<< cur->getKey( i ) <<" "; 
					}
					cout<<std::endl;

					if( cur->isLeafNode() ) {
						for( int i = 0 ; i < cur->getElemCount();++i )
							((LeafNode<KEY,VALUE>*)cur)->getResult(i)->outInfo();
					}
				}
			}
		}

		int addItem( const KEY& key, const VALUE& value ) {
			// if tree is empty
			if( !root ) {
				root = (BTreeNode<KEY> *) new LeafNode<KEY,VALUE>();
				nodeCount++;
				int ret = ( (LeafNode<KEY,VALUE>*) root)->addItem( key, value );
				assert( ret == 0 );
			} else {
				stack< PIPAIR > ST;
				BTreeNode<KEY>*	ptr = root;

				while( !ptr->isLeafNode() ) {
					int pos = ptr->findPosition( key );
					// if inserted key is greater than all keys of the tree, then update 
					// the last node's last key of each level.
					if( pos == -1 ){
						pos = ptr->getElemCount() - 1;
						ptr->setKey( pos, key ); 
					}
					ST.push( PIPAIR( ptr, pos) );

					ptr = ( (InternalNode<KEY>*)ptr)->getChildren( pos );
				}

				if( ((LeafNode<KEY,VALUE> *)ptr)->addItem(key, value) == -1 ) {
					ST.push( PIPAIR( ptr, -1) );
					// then split the leafNode into two leafNodes and maybe need to add several InternalNodes
					return	splitNode( key, value, ST );
				}
			}
			return 0;
		}

		// split the node until we can insert a new item into.
		int splitNode( const KEY& key, const VALUE& value, stack< PIPAIR > & ST ) {
			assert( !ST.empty() );
			BTreeNode<KEY>*	ptr = ST.top().first;
			ST.pop();
			assert( ptr->isLeafNode() );

			// the leafNode is full and we need to split it.
			BTreeNode<KEY>* newLeaf = (BTreeNode<KEY> *)new LeafNode<KEY,VALUE>();
			++nodeCount;

			int32_t	midIndex = ptr->getElemCount()/2 - 1;
			KEY midvalue = ptr->getKey( midIndex ); 
			// copy last half items to new leaf node and change the splitted node's element-count
			newLeaf->copyItems( ptr, midIndex+1, midIndex+1 );
			ptr->setElemCount( midIndex + 1);

			if( key > midvalue ) ((LeafNode<KEY,VALUE>*)newLeaf)->addItem( key, value );
			else	((LeafNode<KEY,VALUE>*)ptr)->addItem( key, value );

			int successed = 0;

			while( !ST.empty() ) {
				// newleaf contains the larger half keys, so we should change the children that parent's pointer point to.
				((InternalNode<KEY> *)ST.top().first)->setChildren( ST.top().second, newLeaf );
				newLeaf = ptr; // the smaller half keys
				ptr = ST.top().first;
				ST.pop();

				// ptr must be an internalNode pointer
				assert( !ptr->isLeafNode() );
				if( ((InternalNode<KEY>*)ptr)->addItem( midvalue, newLeaf) != -1 ) {
					successed = 1;
					break;
				}

				// internal node is full and need to be splitted.
				midvalue = ptr->getKey( midIndex ); 
				BTreeNode<KEY>* newInternal = (BTreeNode<KEY>*) new InternalNode<KEY>();
				++nodeCount;

				// copy last half items to new leaf node and change the splitted node's element-count
				newInternal->copyItems( ptr, midIndex+1, midIndex+1 );
				ptr->setElemCount( midIndex + 1);

				// could key be equal to midvalue ???
				// I don't think that would be happened currently.
				if( key > midvalue ) ( (InternalNode<KEY>*)newInternal )->addItem( midvalue, newLeaf );
				else ((InternalNode<KEY>*)ptr)->addItem( midvalue, newLeaf );

				newLeaf = newInternal;
			}

			// when the splitted node is root
			if( !successed && ST.empty() ) {
				assert( ptr == root );
				InternalNode<KEY> * tmp = new InternalNode<KEY>();
				++nodeCount;
				tmp->addItem( ptr->getKey( ptr->getElemCount()-1 ), ptr );
				tmp->addItem( newLeaf->getKey( newLeaf->getElemCount()-1), newLeaf );
				root = (BTreeNode<KEY> *) tmp;
				++treeDepth;
			}
		}

		// storage layout of B-Tree in file
		// |--------------------------------------|
		// | root info 				|	level 0   |
		// |--------------------------------------|
		// | 1st child of root		|     L		  |
		// | 2st child of root		|     E       |
		// | .....					|     V   1   |
		// | last 2st child of root	|     E		  |
		// | last child of root		|     L		  |
		// |--------------------------------------|
		// | nodes of level 2.....	|   level 2   |
		// |--------------------------------------|
		// | results of linklist| matched results |
		// |--------------------------------------|

		int write2file( const char * fileName ) {
			if( !root ) {
				ERROR_INFO("This is an empty Tree",return 0);
			}

#ifndef USE_FILEIO
			int  fileHandle = open( fileName, O_CREAT | O_WRONLY , S_IRWXU );
			if( fileHandle < 0 ) {
				ERROR_INFO("Open file failed",return -1);
			}
#else 
			FILE *fileHandle = fopen( fileName, "wb");
			if( !fileHandle ) {
				ERROR_INFO("Open file failed",return -1);
			}
#endif
			// calculate the offsets of file in advance. 
			uint32_t  pos = root->getSizeOfFile();
			queue< BTreeNode<KEY> * >BQ;
			BQ.push( root );

			uint32_t* offsetArray = new uint32_t[ MAX_PER_NODE ];

			while( !BQ.empty() ) {

				BTreeNode<KEY>* cur = BQ.front(); BQ.pop(); 
				if( cur->isLeafNode() ) {
					cur->write2file( fileHandle );
					continue;
				}

				for( int i = 0 ; i < cur->getElemCount(); ++i ) {
					BTreeNode<KEY>* tmp = ((InternalNode<KEY>*)cur)->getChildren(i);
					BQ.push( tmp );
					offsetArray[i] = pos;
					pos += tmp->getSizeOfFile();
				}
				((InternalNode<KEY>*)cur)->setFileOffset( offsetArray );
				cur->write2file( fileHandle );
			}

#ifndef USE_FILEIO
			close( fileHandle );
#else 
			fclose( fileHandle );
#endif
			delete[] offsetArray;
			return 0;
		}

		int readFromFile( const char * fileName ) {
#ifndef USE_FILEIO
			int fd = open( fileName, O_RDONLY );
			if( fd < 0 ) {
				ERROR_INFO("Open file failed",return -1);
			}

			int8_t  isLeafNode;
			size_t	isLeafSize = sizeof( isLeafNode );
			uint32_t*	offsetArray = new uint32_t[ MAX_PER_NODE ];
			BTreeNode<KEY>* curNode = NULL, *newNode = NULL;
			queue< BTreeNode<KEY> * >BQ;

			if( read( fd, &isLeafNode, isLeafSize ) != isLeafSize ) {
				ERROR_INFO("Read file error", goto ERROR);
			}

			// this tree must be an empty b-tree when we called this function readFromFile 
			assert( root == NULL );
			if( isLeafNode ) root = (BTreeNode<KEY>*)new LeafNode<KEY,VALUE>();
			else	root = new InternalNode<KEY>();
			nodeCount++;

			root->readFromFile( fd );
			if( !isLeafNode ) {
				read( fd, offsetArray, root->getElemCount() * sizeof(uint32_t) ) ;
				BQ.push( root );
			}

			while( !BQ.empty() ) {
				curNode = BQ.front(); BQ.pop(); 

				for( int i = 0 ; i < curNode->getElemCount(); ++i ) {
					if( read( fd, &isLeafNode, isLeafSize ) != isLeafSize ) {
						ERROR_INFO("Read file error", goto ERROR);
					}
					if( isLeafNode ) {
						newNode = (BTreeNode<KEY>*)new LeafNode<KEY,VALUE>();
					} else {
						newNode = new InternalNode<KEY>();
					}
					nodeCount++;

					if( newNode->readFromFile( fd ) == -1 ) {
						ERROR_INFO("Something wrong in readFromFile", goto ERROR);
					}

					((InternalNode<KEY>*)curNode)->setChildren( i , newNode );
					if( !isLeafNode ) {
						BQ.push( newNode );
						// we must read these offsets variables in file even though it is useless here
						// because we need to make sure what we read next are the correct values.
						read( fd, offsetArray, newNode->getElemCount() * sizeof(uint32_t) ) ;
					}
				}
			}
			close( fd );
			delete offsetArray;
			return 0;
ERROR:
			close( fd );
			delete offsetArray;
			return -1;
#else 
			FILE * fp = fopen( fileName, "rb");
			if( !fp ) {
				ERROR_INFO("open file failed", return -1);
			}

			int8_t  isLeafNode;
			size_t	isLeafSize = sizeof( isLeafNode );
			uint32_t*	offsetArray = new uint32_t[ MAX_PER_NODE ];
			BTreeNode<KEY>* curNode = NULL, *newNode = NULL;
			queue< BTreeNode<KEY> * >BQ;

			if( fread( &isLeafNode, isLeafSize, 1, fp ) != 1 ) {
				ERROR_INFO("Read file error", goto ERROR);
			}
			// this tree must be an empty b-tree when we called this function readFromFile 
			assert( root == NULL );
			if( isLeafNode ) root = (BTreeNode<KEY>*)new LeafNode<KEY,VALUE>();
			else	root = new InternalNode<KEY>();
			nodeCount++;

			root->readFromFile( fp );
			if( !isLeafNode ) {
				fread( offsetArray, root->getElemCount() * sizeof(uint32_t), 1, fp ) ;
				BQ.push( root );
			}

			while( !BQ.empty() ) {
				curNode = BQ.front(); BQ.pop(); 

				for( int i = 0 ; i < curNode->getElemCount(); ++i ) {
					fread( &isLeafNode, isLeafSize, 1, fp );
					if( isLeafNode ) {
						newNode = (BTreeNode<KEY>*)new LeafNode<KEY,VALUE>();
					} else {
						newNode = new InternalNode<KEY>();
					}
					nodeCount++;

					if( newNode->readFromFile( fp ) == -1 ) {
						ERROR_INFO("Something wrong in readFromFile", goto ERROR);
					}

					((InternalNode<KEY>*)curNode)->setChildren( i , newNode );
					if( !isLeafNode ) {
						BQ.push( newNode );
						// we must read these offsets variables in file even though it is useless here
						// because we need to make sure what we read next are the correct values.
						fread( offsetArray, newNode->getElemCount() * sizeof(uint32_t), 1, fp ) ;
					}
				}

			}
			delete offsetArray;
			fclose( fp );
			return 0;
ERROR:
			delete offsetArray;
			fclose( fp );
			return -1;
#endif
		}

		// query records from index file 
		LinkList<VALUE> * queryFromFile( const char * fileName, const KEY& begin, const KEY& end ) {
			FILE * fp = fopen( fileName , "rb" );
			if( !fp ) {
				fprintf( stderr, "Open file %s failed at file %s, in function %s, line %d\n", fileName, __FILE__, __func__, __LINE__);
				return NULL;
			}

			int8_t  isLeafNode;
			size_t	isLeafSize = sizeof( isLeafNode );

			int pos = 0;
			LinkList<VALUE>* ret = NULL;
			BTreeNode<KEY>* nodePtr = NULL;
			uint32_t*	offsetArray = new uint32_t[ MAX_PER_NODE ];

			while( 1 ) {
				if( fread( &isLeafNode, isLeafSize, 1, fp ) != 1 ) {
					ERROR_INFO("Read file error", goto ERROR);
				}
				
				if( isLeafNode ) {
					nodePtr = (BTreeNode<KEY>*)new LeafNode<KEY,VALUE>();
				} else {
					nodePtr = new InternalNode<KEY>();
				}
				nodePtr->readFromFile( fp );
				// we will go to finding results procedure when arrive a leaf node
				if( nodePtr->isLeafNode() ) break;

				fread( offsetArray, nodePtr->getElemCount() * sizeof(uint32_t), 1, fp ) ;
				pos = nodePtr->findPosition( begin );
				delete nodePtr;

				// begin > max key of this node;
				if( pos == -1 ) goto ERROR;

				if( fseek( fp, offsetArray[pos], SEEK_SET ) == -1 ) {
					ERROR_INFO("Seeking file position failed", goto ERROR);
				}
			}

			// now nodePtr is a leaf node 
			if( -1 == (pos = nodePtr->findPosition( begin )) )  goto ERROR;
			ret = new LinkList<VALUE>();

			// we will find all records whose keys are in [begin,end]
			while( 1 ) {
				for( ; pos < nodePtr->getElemCount(); ++pos ) {
					if( nodePtr->getKey(pos) <= end ) {
						ret->merge( *((LeafNode<KEY,VALUE>*)nodePtr)->getResult(pos) );
					} else {
						delete nodePtr;
						nodePtr = NULL;
						break;
					}
				}
				if( !nodePtr ) break; 
				else delete nodePtr;

				// read next leaf node
				// If the query key is greater than the max key of this BTree, we will read BTreeNodes until file hanlder go to ending.
				if( fread( &isLeafNode, isLeafSize, 1, fp ) != 1 ) {
					fprintf(stderr,"current position of file pointer is %d\n",ftell(fp));
					ERROR_INFO("Read file error", goto ERROR);
				}
				assert( isLeafNode != 0 );
				nodePtr = (BTreeNode<KEY>*)new LeafNode<KEY,VALUE>();
				nodePtr->readFromFile( fp );
				pos = 0; // find from the begining position
			}
			
			fclose( fp );
			delete[] offsetArray;
			return ret;
ERROR:
			fclose( fp );
			delete[] offsetArray;
			return NULL;
		}

};


#endif
