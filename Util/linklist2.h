/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified: 2012-11-28 09:26
#
# Filename: linklist.h
#
# Description: 
#
===========================================================================*/

#ifndef _LINKLIST_H
#define _LINKLIST_H

#include <cstdio>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include "../global.h"
#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

//#define	LINKNODE_POOL
#define MAX_LINKNODE 400001000
#define SKIP_NUM	 200

// T must be a type that can use operation sizeof to get its size.
template<typename T>
class LinkList {

	public:
		class LinkNode {
			public:
				T data;
				LinkNode *next;
				LinkNode *skip;

				LinkNode(){
					skip = next = NULL;
				}

				LinkNode(T _data): data(_data), next( NULL ), skip(NULL) {
				}

				~LinkNode() {
					skip = next = NULL;
				}

				void clear() {
					skip = next = NULL;
				}
		};
	

#ifdef	LINKNODE_POOL
		static void initLinkNodePool( uint32_t num ) {
			assert( num <= MAX_LINKNODE );
			LinkList<T>::startPtr = new LinkNode[ num ];
			for( int i = 0; i < num; ++i )
				LinkList<T>::array[i] = ((LinkNode*)startPtr)+i;
				//LinkList<T>::array[i] = new LinkNode();

			LinkList<T>::cursor = num;
		}

		static void clearPool() {
			//for( int i = 0 ; i < LinkList<T>::cursor; ++i )
				//delete (LinkNode*)LinkList<T>::array[i];
			delete[] (LinkNode*)startPtr;
		}

		static LinkNode* getLinkNode( const T& t) {
			assert( cursor > 0 );
			LinkNode *ret = (LinkNode*)LinkList<T>::array[--cursor];
			ret->data = t;
			ret->clear();
			return ret;
		}
		
		static void		freeLinkNode( LinkNode * node ) {
			LinkList<T>::array[cursor++] = node;
		}

		static uint32_t getCursor() {
			return cursor;
		}
#endif
		
		LinkList() {
			skip_pre = head = tail = NULL;
			nodeCount = 0;
		}

		~LinkList() {
			LinkNode *pre = NULL;
			for( LinkNode* ptr = head; ptr != NULL; ) {
				pre = ptr->next;
#ifndef	LINKNODE_POOL
				delete ptr;
#else
				LinkList<T>::freeLinkNode( ptr );
#endif
				ptr = pre;
				--nodeCount;
			}

			if( nodeCount != 0 ) {
				ERROR_INFO("There are Something wrong in LinkList",);
			}
		}

		// initialization of linklist
		void clear() {
			skip_pre = head = tail = NULL;
			nodeCount = 0;
		}

		// -1 for failed, 0 for successed.
		int 	addElement( const T& data ) {
#ifndef LINKNODE_POOL
			if( head == NULL ) {
				head = tail = new LinkNode( data );
			} else {
				tail = tail->next = new LinkNode( data );
			}
#else 
			if( head == NULL ) {
				head = tail = LinkList<T>::getLinkNode( data );
			} else {
				tail = tail->next = LinkList<T>::getLinkNode( data );
			}
#endif
			if( skip_pre ) {
				skip_pre->skip = tail;
				skip_pre = skip_pre->next;
			}
			nodeCount++;
			if( nodeCount == SKIP_NUM ) {
				skip_pre = head;
			}
			return 0;
		}

		inline uint32_t size() {
			return nodeCount;
		}

		inline bool empty() {
			return head == NULL;
		}

		// elements need be ascending ordered after adding elements of another list
		int 	merge2( LinkList<T>& list ) {
			if( list.empty() ) {
				return 0;
			}

			LinkNode *curB = list.begin() ;
			if( head == NULL || curB->data < head->data ) {
				swap( curB, head );
			}

			LinkNode *nextPtr = NULL , *curA = head;
			while( 1 ) {
				// find a position to insert this element that pointer cur point to.
				if( NULL == curB ) break;
				while( curA->next ) {
					curA = curA->next;
				}
				if( NULL == curA->next ){
					curA->next = curB;
					break;
				}
			}

			nodeCount += list.size();
			// clear the list because all nodes had been transfered into current one 
			list.clear();
			return 0;
		}

		// elements need be ascending ordered after adding elements of another list
		int 	merge( LinkList<T>& list ) {
			if( list.empty() ) {
				return 0;
			}

			LinkNode *curB = list.begin() ;
#if  0
			int i = 0 ;
			for( LinkNode* p = head; p ; p = p->next, i++ ) if( nodeCount == 29 && list.size() == 112 ) printf("%p\n",p) ;
			printf("size1 = %d, size2 = %d\n", nodeCount, list.size() );
			assert( i == nodeCount );
			i = 0 ;

			for( LinkNode* p = curB; p ; p = p->next, i++) if( nodeCount == 29 && list.size() == 112 ) printf("%p\n",p) ;
			assert( i == list.size() );
			printf("size1 = %d, size2 = %d\n", nodeCount, list.size() );
#endif
			if( head == NULL || curB->data < head->data ) {
				swap( curB, head );
			}

			LinkNode *nextPtr = NULL , *curA = head;
#if 0	
			assert( curA == head );
			while( curA->next ) curA = curA->next;
			curA->next = curB;
			i = 0 ;
			for( LinkNode* p = head; p ; p = p->next, i++) if( nodeCount == 29 && list.size() == 112 ) printf("%p\n",p) ;
			printf("size1 = %d, size2 = %d\n", nodeCount, list.size() );
			assert( i == nodeCount + list.size() );
			assert( curA != NULL );
			// TODO: we need update tail pointer of current linklist
			//printf("merging in\n");
#endif		
			while( 1 ) {
				// find a position to insert this element that pointer cur point to.
				if( NULL == curB ) break;
				while( curA->next &&  curA->next->data < curB->data ) {
                    //if( curA->skip && curA->skip->data < curB->data ) curA = curA->skip;
                    //else
                        curA = curA->next;
				}
				if( NULL == curA->next ){
					curA->next = curB;
					break;
				}
				// insert element that curB points to 
				nextPtr = curA->next;
				curA = curA->next = curB;
				curB = curB->next;
				curA->next = nextPtr;
				/*
				curA->next = curB;
				curA = nextPtr;
				while( curB->next && curB->next->data < curA->data ) {
					if( curB->skip && curB->skip->data < curA->data ) curB = curB->skip;
					else curB = curB->next;
				}
				nextPtr = curB->next;	
				curB->next = curA;
				curB = nextPtr;
				*/
			}

			nodeCount += list.size();
			// clear the list because all nodes had been transfered into current one 
			list.clear();
			return 0;
		}

		inline LinkNode* begin() {
			return head;
		}

		// TODO: we can use skip-list to accelerate the speed of merging operation when the 
		// querying speed is too slow to tolerate 
		// the elements of two LinkLists must be ascending ordered.
		// only the common elements in two linklist will be reserved in the linklist
		void	intersection(  LinkList* list ) {
			// get the intersection of list A and B
			LinkNode *preA = NULL , *curA = NULL, *temp = NULL, *curB = NULL;
			if( list ) curB = list->begin();
			curA = this->head ;
			head = NULL;
			nodeCount = 0;
			while( curA != NULL ) {
				while( curB && curA->data > curB->data ) {
					//if( curB->skip && curA->data > curB->skip->data ) {
						//curB = curB->skip;
					//}
					curB = curB->next;
				}

				if( !curB ) {
					while( curA ) {
						temp = curA->next;
#ifdef	LINKNODE_POOL
						LinkList<T>::freeLinkNode( curA );
#else 
						delete curA;
#endif
						curA = temp;
					}
					break;
				}
				if( curA->data == curB->data ) {
					// modify the LinkList's head after merging
					if( NULL == preA ) preA = head = curA;
					else preA = preA->next = curA;
					++nodeCount;
					curA = curA->next;
				} else {
					temp = curA->next;
#ifdef	LINKNODE_POOL
					LinkList<T>::freeLinkNode( curA );
#else 
					delete curA;
#endif
					curA = temp;
				}
			}
			if( preA ) preA->next = NULL;
		}

		/* layout of storage in file
		 * |-------------------------|
		 * |   nodeCount             |
		 * |   1st node's data       |
		 * |   2nd node's data       |
		 * |    ...........          |
		 * |   last node's data      |
		 * |-------------------------|
		 */
		// all memory of linklist's elements will be delete after this function
		int 	write2file( int fd, int32_t offset = 0 ) {
			// we will seek from current position defaultly.
		    assert( fd >= 0 );	
			off_t pos = lseek( fd, offset, SEEK_CUR );
			if( pos < 0 ) {
				ERROR_INFO("seeking file position error",return -1);
			}

			write( fd, &nodeCount, sizeof( nodeCount ) ) ;

			int elemSize = sizeof( T );
			T * array = new T[nodeCount];
			int i = 0;
			// copy elements to an array and then use only once system_call "write" to write data into file
			for( LinkNode *ptr = head ; ptr ; ptr = ptr->next ) {
				*( array + i++ ) = ptr->data;
				delete ptr;	
			}
			head = NULL;

			assert( i == nodeCount );
			write( fd, array, elemSize * nodeCount );
			delete[] array;
			return 0;
		}

		// get the total size that we wrote this Linklist object in file
		inline int32_t	getSizeOfFile() {
			return sizeof( nodeCount ) + nodeCount * sizeof(T);
		}

		// read data from file and load it to current LinkList object.
		int		readFromFile( int fd, int32_t offset = 0 ) {
		    assert( fd >= 0 );	
			// we will seek from current position defaultly.
			off_t pos = lseek( fd, offset, SEEK_CUR );
			if( pos < 0 ) {
				ERROR_INFO("NULL file pointer fp", return -1);
			}

			read( fd, &nodeCount, sizeof( nodeCount ));

			int elemSize = sizeof( T );
			T * array = new T[nodeCount];
			read( fd, array, elemSize * nodeCount );

			int i = 0;
			while( i < nodeCount ) {
				if( i == 0 ) head = tail = new LinkNode( array[i] );
				else tail = tail->next = new LinkNode( array[i] );
				++i;
			}

			delete[] array;
			return 0;
		}


		// file IO
		// all memory of linklist's elements will be delete in this function
		int 	write2file( FILE * fp ) {
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}

			fwrite( &nodeCount, sizeof( nodeCount ), 1 , fp ) ;

			int elemSize = sizeof( T );
			LinkNode *tmp;
			for( LinkNode *ptr = head ; ptr ; ptr = tmp ) {
				fwrite( &(ptr->data), elemSize , 1 , fp );
				tmp = ptr->next;
#ifndef	LINKNODE_POOL
				delete ptr;
#else
				LinkList<T>::freeLinkNode( ptr );
#endif
			}
			
			head = NULL;
			nodeCount = 0;
		
			return 0;
		}
			
		// read data from file and load it to current LinkList object.
		int		readFromFileAndMerge( FILE *fp, std::vector<T>& ret ) {
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}

            uint32_t  tmpNodeCount = 0;
			fread( &tmpNodeCount, sizeof( tmpNodeCount ), 1, fp);

			int elemSize = sizeof( T );
			T * array = new T[tmpNodeCount];
			fread( array, elemSize * tmpNodeCount, 1, fp );

            ret.insert( ret.end(), array, array + tmpNodeCount );
			delete[] array;
			return 0;
		}


		// read data from file and load it to current LinkList object.
		int		readFromFileAndMerge( FILE *fp ) {
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}

            uint32_t  tmpNodeCount = 0;
			fread( &tmpNodeCount, sizeof( tmpNodeCount ), 1, fp);
            nodeCount += tmpNodeCount;

			int elemSize = sizeof( T );
			T * array = new T[tmpNodeCount];
			fread( array, elemSize * tmpNodeCount, 1, fp );

            LinkNode *current = head;
			int i = 0;
			while( i < tmpNodeCount ) {
#ifndef LINKNODE_POOL
				if( NULL == head ) head = tail = new LinkNode( array[i] );
				else {
                    while ( current != NULL && current->data < array[i] ) {
                        current = current->next;
                    }
                    if ( current == NULL || current == tail) {
                        tail = tail->next = new LinkNode( array[i] );
                    } else {
                        LinkNode *tmp = new LinkNode( array[i] ); 
                        tmp->next = current->next;
                        current = current->next = tmp;
                    }
                }
#else 
				if( NULL == head) {
					head = tail = LinkList<T>::getLinkNode( array[i] );
				} else {
                    /*
                    while ( current != NULL && current->data < array[i] ) {
                        if ( current->skip && current->skip->data < array[i] ) {
                            current = current->skip;
                        } else current = current->next;
                    }
                    if ( current == NULL || current == tail) {
                        tail = tail->next = LinkList<T>::getLinkNode( array[i] );
                    } else {
                        LinkNode *tmp = LinkList<T>::getLinkNode( array[i] );
                        tmp->next = current->next;
                        current = current->next = tmp;
                    }
                    */
                    tail = tail->next = LinkList<T>::getLinkNode( array[i] );
				}
#endif
				assert( tail->next == NULL );
				++i;
				if( skip_pre ) {
					skip_pre->skip = tail;
					skip_pre = skip_pre->next;
				}

				if( i == SKIP_NUM ) {
					skip_pre = head;
				}
			}
			delete[] array;
			return 0;
		}

		// read data from file and load it to current LinkList object.
		int		readFromFile( FILE *fp ) {
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}

            this->clear();
			fread( &nodeCount, sizeof( nodeCount ), 1, fp);

			int elemSize = sizeof( T );
			T * array = new T[nodeCount];
			fread( array, elemSize * nodeCount, 1, fp );

			int i = 0;
			while( i < nodeCount ) {
#ifndef LINKNODE_POOL
				if( i == 0 ) head = tail = new LinkNode( array[i] );
				else tail = tail->next = new LinkNode( array[i] );
#else 
				if( i == 0 ) {
					head = tail = LinkList<T>::getLinkNode( array[i] );
				} else {
					tail = tail->next = LinkList<T>::getLinkNode( array[i] );
				}
#endif
				assert( tail->next == NULL );
				++i;
				if( skip_pre ) {
					skip_pre->skip = tail;
					skip_pre = skip_pre->next;
				}

				if( i == SKIP_NUM ) {
					skip_pre = head;
				}
			}
			delete[] array;
			return 0;
		}

#ifdef DEBUG
		// print detailed Information of linklist for debug
		void outInfo() {
			cout<<"Informations of Linklist, has "<<nodeCount<<" elements"<<endl;
			for( LinkNode* ptr = head; ptr ; ptr = ptr->next ) {
				cout<< ptr->data <<" ";
			}
			cout<<endl;
		}
#endif
	
	
	private:
		LinkNode* 	head;
		LinkNode* 	tail;
		LinkNode* 	skip_pre;
		uint32_t	nodeCount;
#ifdef	LINKNODE_POOL
		static	void*	array[MAX_LINKNODE];
		static	void*	startPtr;
		static	uint32_t	cursor;
#endif

};

#ifdef LINKNODE_POOL
template<typename T>
uint32_t LinkList<T>::cursor = 0;
template<typename T>
void * LinkList<T>::array[MAX_LINKNODE];
template<typename T>
void* LinkList<T>::startPtr = NULL;
#endif

#endif
