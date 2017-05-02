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
#define MAX_LINKNODE 100000001
#define SKIP_NUM	 200
#define ELEM_NUM_EACH_NODE 8

// T must be a type that can use operation sizeof to get its size.
template<typename T>
class LinkList {

	public:
		class LinkNode {
			public:
				T data[ELEM_NUM_EACH_NODE];
                uint8_t  elem_num;
				LinkNode *next;
				//LinkNode *skip;

				LinkNode(): elem_num(0), next( NULL )
                            //, skip(NULL) 
                {
				}

				~LinkNode() {
                     //skip = next = NULL;
                     next = NULL;
				}

				void clear() {
                    elem_num = 0;
					next = NULL;
				}

                bool addElement(T elem) {
                    if (elem_num < ELEM_NUM_EACH_NODE) {
                       data[elem_num++] = elem;
                       return true;
                    } else return false;
                }
/*
                // false: end with node
                // true: end with this
                bool merge(LinkNode& node) {
                    T temp[ELEM_NUM_EACH_NODE << 1];
                    int i = 0, j = 0, k = 0;
                    while (i < elem_num && j < node.elem_num) {
                        if ( data[i] < node.data[j]) {
                           temp[k++]  = data[i++];
                        } else if (data[i] > node.data[j]) {
                            temp[k++] = node.data[j++];
                        } else {
                            temp[k++] = data[i++];
                            ++j;
                        }
                    }
                    bool ret = (i == elem_num);
                    while( i < elem_num ) temp[k++] = data[i++];
                    while( j < node.elem_num ) temp[k++] = node.data[j++];
                    for (i = 0; i < elem_num; ++i) data[i] = temp[i];
                    for (j = 0; j < node.elem_num; ++j) node.data[j] = temp[elem_num + j];
                    return ret;
                }*/
		};
	

#ifdef	LINKNODE_POOL
		static void initLinkNodePool( uint32_t num ) {
			assert( num <= MAX_LINKNODE );
            //printf("linknode size = %d\n", sizeof(LinkNode));
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

		static LinkNode* getLinkNode() {
			assert( cursor > 0 );
			LinkNode *ret = (LinkNode*)LinkList<T>::array[--cursor];
			ret->clear();
			return ret;
		}
		
		static void		freeLinkNode( LinkNode * node ) {
            node->clear();
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
                nodeCount -= ptr->elem_num;
#ifndef	LINKNODE_POOL
				delete ptr;
#else
				LinkList<T>::freeLinkNode( ptr );
#endif
				ptr = pre;
			}

			if( nodeCount != 0 ) {
#ifdef PRINT
				ERROR_INFO("There are Something wrong in LinkList", nodeCount);
#endif
                //printf("nodeCount = %d\n", nodeCount);
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
				head = tail = new LinkNode();
                head->addElement(data);
			} else {
                if (tail->elem_num >= ELEM_NUM_EACH_NODE ) {
                    tail = tail->next = new LinkNode;
                }
                tail->addElement(data);
			}
#else 
			if( head == NULL ) {
				head = tail = LinkList<T>::getLinkNode();
                head->addElement(data);
			} else {
                if (tail->elem_num >= ELEM_NUM_EACH_NODE ) {
                    //tail = tail->next = new LinkNode;
                    tail = tail->next = LinkList<T>::getLinkNode();
                }
                tail->addElement(data);
			}
#endif
			nodeCount++;
			//if( skip_pre ) {
				//skip_pre->skip = tail;
				//skip_pre = skip_pre->next;
			//}
			//if( nodeCount == SKIP_NUM ) {
				//skip_pre = head;
			//}
			return 0;
		}

		inline uint32_t size() {
			return nodeCount;
		}

		inline bool empty() {
			return head == NULL;
		}

		inline LinkNode* begin() {
			return head;
		}

        LinkNode *newNode() {
#ifdef	LINKNODE_POOL
            return LinkList<T>::getLinkNode();
#else 
            return new LinkNode;
#endif
        }


#if 0

		// elements need be ascending ordered after adding elements of another list
		int 	merge2( LinkList<T>& list ) {
			if( list.empty() ) {
				return 0;
			}

			LinkNode *curB = list.begin() ;
			if( head == NULL || !head->merge( curB )) {
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
			printf("size1 = %d, size2 = %d\n", nodeCount, list.size() );
#endif
			if( head == NULL || curB->data < head->data ) {
				swap( curB, head );
			}

			LinkNode *nextPtr = NULL , *curA = head;
#if 0	
			printf("size1 = %d, size2 = %d\n", nodeCount, list.size() );
			assert( i == nodeCount + list.size() );
			assert( curA != NULL );
			// TODO: we need update tail pointer of current linklist
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

#endif
		// TODO: we can use skip-list to accelerate the speed of merging operation when the 
		// querying speed is too slow to tolerate 
		// the elements of two LinkLists must be ascending ordered.
		// only the common elements in two linklist will be reserved in the linklist
		void	intersection(  LinkList* list ) {
			// get the intersection of list A and B
			LinkNode *prev = NULL , *curA = NULL, *temp = NULL, *curB = NULL;
			if( list ) curB = list->begin();
			curA = this->head ;

            head = newNode();
            temp = head;
			nodeCount = 0;
            int idxA = 0, idxB = 0;
			while( curA != NULL && curB != NULL ) {
                while( idxA < curA->elem_num && idxB < curB->elem_num ) {
                    if (curA->data[idxA] > curB->data[idxB]) {
                        ++idxB;
                    } else if (curA->data[idxA] < curB->data[idxB]) {
                        ++idxA;
                    } else {
                        if (!temp->addElement(curA->data[idxA])) {
                            temp = temp->next = newNode();
                            temp->addElement(curA->data[idxA]);
                        }
                        ++idxA;
                        ++idxB;
                        ++nodeCount;
                    }
				}

                if (idxA >= curA->elem_num) {
                    prev = curA->next;
                    freeNode( curA );
                    curA = prev;
                    idxA = 0;
                }
                if (idxB >= curB->elem_num) {
                    prev = curB->next;
                    freeNode( curB );
                    curB = prev;
                    idxB = 0;
				}
			}
            while (curA != NULL) {
                prev = curA->next;
                freeNode( curA );
                curA = prev;
            }
            while (curB != NULL) {
                prev = curB->next;
                freeNode( curB );
                curB = prev;
            }
            
            tail = temp;
			if( temp ) temp->next = NULL;
            list->clear();
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
        
		// get the total size that we wrote this Linklist object in file
		inline int32_t	getSizeOfFile() {
			return sizeof( nodeCount ) + nodeCount * sizeof(T);
		}

		// file IO
		// all memory of linklist's elements will be delete in this function
		int 	write2file( FILE * fp ) {
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}

			fwrite( &nodeCount, sizeof( nodeCount ), 1 , fp ) ;
			int elemSize = sizeof( T );
            T *array = new T[nodeCount];
            int idx = 0;

			LinkNode *tmp;
			for( LinkNode *ptr = head ; ptr ; ptr = tmp ) {
				//fwrite( ptr->data, elemSize * ptr->elem_num  , 1 , fp );
                for (int i = 0; i < ptr->elem_num; ++i) {
                    array[idx++] = ptr->data[i];
                    //printf("%d\n", ptr->data[i]);
                }
                //printf("\n\n");
				tmp = ptr->next;
#ifndef	LINKNODE_POOL
				delete ptr;
#else
				LinkList<T>::freeLinkNode( ptr );
#endif
			}
			
			assert( idx == nodeCount );
			fwrite( array, elemSize * nodeCount, 1 , fp);
			delete[] array;
            this->clear();
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
            T tmpData;
            T * array = new T[tmpNodeCount];
            fread( array, elemSize * tmpNodeCount, 1, fp );

            for (int i = 0 ; i < tmpNodeCount; ++i) {
                ret.push_back( array[i] );
            }

            delete[] array;
			return 0;
		}
		
        // copy data from linkedlist to vector ret
		int		copyAndMerge( std::vector<T>& ret ) {
            if( head == NULL ) {
				return 0;
			}

            //TODO: thread-synchronization
            std::vector<T> temp;
			LinkNode *curA = head;
            int idx = 0;
            int idx2 = 0;
			while ( curA != NULL || idx < ret.size() ) {
                if (curA == NULL || ( idx < ret.size() && curA->data[idx2] > ret[idx] )) {
                    temp.push_back( ret[idx++] );
                } else {
                    temp.push_back( curA->data[idx2++] );
                    if (idx2 >= curA->elem_num ) {
                        curA = curA->next;
                        idx2 = 0;
                    }
				}
			}

            ret.swap( temp );
			return 0;
		}



		// read data from file and load it to current LinkList object.
		int		readFromFileAndMerge( FILE *fp ) {
         //   printf("into readFromFileAndMerge()\n");
			if( !fp ) {
				ERROR_INFO("NULL file pointer fp",return -1);
			}

            uint32_t  tmpNodeCount = 0;
			fread( &tmpNodeCount, sizeof( tmpNodeCount ), 1, fp);
            nodeCount += tmpNodeCount;
        
           // printf("tmpNodeCount=%d, nodeCount=%d \n", tmpNodeCount, nodeCount);
			int elemSize = sizeof( T );
			T * array = new T[tmpNodeCount];
			fread( array, elemSize * tmpNodeCount, 1, fp );

            LinkNode *current = head;
			int i = 0;
			while( i < tmpNodeCount ) {
				if( NULL == head ) {
                    head = tail = newNode();
                    while ( i < tmpNodeCount ) {
                        if (tail->elem_num < ELEM_NUM_EACH_NODE) {
                            tail->addElement( array[i++] );
                        } else  tail = tail->next = newNode();
                    }
                }
				else {
                    LinkNode* tmpNode = new LinkNode();
                    while ( i < tmpNodeCount && tmpNode->addElement(array[i]) ) ++i;

                    while ( current != NULL && current->merge(*tmpNode) ) {
                        //tail = current;
                        current = current->next;
                    }
                    if ( current == NULL ) {
                        current = tail = tail->next = tmpNode;
                    } else {
                        tmpNode->next = current->next;
                        current = current->next = tmpNode;
                    }
                }
				//++i;
				//if( skip_pre ) {
					//skip_pre->skip = tail;
					//skip_pre = skip_pre->next;
				//}

				//if( i == SKIP_NUM ) {
					//skip_pre = head;
				//}
			}
            tail->next = NULL;
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
				if( i == 0 ) {
                    head = tail = new LinkNode;
                    head->addElement(array[i]);
                } else {
                    if (!tail->addElement(array[i])) {
                        tail = tail->next = new LinkNode;
                        tail->addElement(array[i]);
                    }
                }
				//else tail = tail->next = new LinkNode( array[i] );
#else 
				if( i == 0 ) {
					head = tail = LinkList<T>::getLinkNode();
                    head->addElement( array[i] );
				} else {
                    //tail = tail->next = LinkList<T>::getLinkNode();
                    if (!tail->addElement(array[i])) {
                        tail = tail->next = LinkList<T>::getLinkNode();
                        tail->addElement(array[i]);
                    }
				}
#endif
				assert( tail->next == NULL );
				//++i;
				//if( skip_pre ) {
					//skip_pre->skip = tail;
					//skip_pre = skip_pre->next;
				//}

				//if( i == SKIP_NUM ) {
					//skip_pre = head;
				//}
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

        void freeNode(LinkNode* node) {
#ifdef	LINKNODE_POOL
            LinkList<T>::freeLinkNode( node );
#else 
            delete node;
#endif
        }
/*
        LinkNode *newNode() {
#ifdef	LINKNODE_POOL
            return LinkList<T>::getLinkNode();
#else 
            return new LinkNode;
#endif
        }
*/
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
