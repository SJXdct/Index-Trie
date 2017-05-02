#include "trie.h"
#include <cstdio>
#include <queue>
using std::queue;

#ifdef	NODE_POOL
template<typename KEY>
ResourcePool<InterTrieNode*,POOL_MAX> Trie<KEY>::interNodePool;
template<typename KEY>
ResourcePool<LeafTrieNode*,POOL_MAX>  Trie<KEY>::leafNodePool;
/*
struct LinkElem {
    VALUE data[ ELEM_NUM_EACH_NODE ];
    uint8_t elem_num;
    LinkElem *next;
};*/

// must call this function before we use trie tree for indexing in NODE_POOL mode
template<typename KEY>
void Trie<KEY>::initPool( uint32_t num ) {
	assert( num <= POOL_MAX );
	for( int i = 0; i < num; ++i ) {
		Trie<KEY>::interNodePool.push_resource( new InterTrieNode() );
		Trie<KEY>::leafNodePool.push_resource( new LeafTrieNode() );
	}
}

// must delete all memory of pool when trie node would not be used.
template<typename KEY>
void Trie<KEY>::clearPool() {
	TrieNode* nodePtr;
	while( nodePtr = Trie<KEY>::interNodePool.get_free_resource() ) {
		delete nodePtr;
	}
	while( nodePtr = Trie<KEY>::leafNodePool.get_free_resource() ) {
		delete nodePtr;
	}
}
#endif


template<typename KEY>
Trie<KEY>::~Trie() {
	if( root ) {
		//printf("nodeCount = %d\n",nodeCount);
		queue< TrieNode* > Q;
		Q.push( root );
		while( !Q.empty() ) {
			TrieNode* cur = Q.front();
			Q.pop();

			for( int i = 0 ; !cur->isLeafNode() && i < cur->getElemCount(); ++i ) {
					Q.push( ((InterTrieNode*)cur)->getChildren( cur->getOnePosition(i)) );
			}

#ifdef	NODE_POOL
			cur->clear();
			if( cur->isLeafNode() ) {
				Trie<KEY>::leafNodePool.set_resource_free( (LeafTrieNode*)cur );
			} else {
				Trie<KEY>::interNodePool.set_resource_free( (InterTrieNode*)cur );
			}
#else
			delete cur;
#endif
			--nodeCount;
		}
		//printf("finished trie tree destruction!\n");

		assert( nodeCount == 0 );
	}
}


template<typename KEY>
int Trie<KEY>::addItem( const KEY& key, const VALUE& value ) {
//    fprintf( stdout, "[key = %d, value = %d]\n", key, value);
	int num = (sizeof(KEY)<<3)/BITS_PER_NODE, rsnum = (num-1)*BITS_PER_NODE;
	assert( root != NULL );
	TrieNode* current = root;
	KEY k = key;

	for(int i = 1; i < num; ++i ) {
		uint8_t tmp = (uint8_t)( (k>>rsnum)&0xff );
		if( !current->hasChildren(tmp) ) {
#ifndef	NODE_POOL
			if( i != num-1 ) ((InterTrieNode*)current)->addItem( tmp, new InterTrieNode());
			else 			((InterTrieNode*)current)->addItem( tmp, new LeafTrieNode());
#else
			if( i != num-1 ) ((InterTrieNode*)current)->addItem( tmp, Trie<KEY>::getInterNode());
			else 			((InterTrieNode*)current)->addItem( tmp, Trie<KEY>::getLeafNode());
#endif
			++nodeCount;
		}
		current = ((InterTrieNode*)current)->getChildren(tmp);
		assert( current != NULL );
		k <<= BITS_PER_NODE;
	}
	uint8_t tmp = (uint8_t)( (k>>rsnum)&0xff );
	((LeafTrieNode*)current)->addItem( tmp, value );
    //if (sizeof(KEY) == 1) printf("temp = %d, key = %d, value = %d\n", tmp, key, value);
}

template<typename KEY>
int Trie<KEY>::write2file( const char *fileName ) {
	if( !root ) {
		ERROR_INFO("This is an empty Tree",return 0);
	}

    //printf("nodeCount = %d\n",nodeCount);
	FILE *fileHandle = fopen( fileName, "wb");
	if( !fileHandle ) {
		ERROR_INFO("Open file failed",return -1);
	}
	// calculate the offsets of file in advance. 
	uint32_t  pos = root->getSizeOfFile();
	queue< TrieNode* >BQ;
	BQ.push( root );

	uint32_t* offsetArray = new uint32_t[ MAX_PER_NODE ];

	while( !BQ.empty() ) {

		TrieNode* cur = BQ.front(); BQ.pop(); 
		if( cur->isLeafNode() ) {
			cur->write2file( fileHandle );
			continue;
		}

		for( int i = 0 ; i < cur->getElemCount(); ++i ) {
			TrieNode* tmp = ((InterTrieNode*)cur)->getChildren( cur->getOnePosition(i) );
			BQ.push( tmp );
			offsetArray[i] = pos;
			pos += tmp->getSizeOfFile();
		}
		((InterTrieNode*)cur)->setFileOffset( offsetArray );
		cur->write2file( fileHandle );
	}
	fclose( fileHandle );
	delete[] offsetArray;
	return 0;
}


template<typename KEY>
//this function will only be used in queryFromFile
TrieNode* Trie<KEY>::readTrieNode( FILE* fp ) {
	int8_t  isLeafNode;

	// read a Node of trie tree
	if( fread( &isLeafNode, sizeof(int8_t), 1, fp ) != 1 ) {
		ERROR_INFO("Read file error", return NULL);
	}
	TrieNode* nodePtr = NULL;
	if( isLeafNode ) {
		nodePtr = new LeafTrieNode();
	} else {
		nodePtr = new InterTrieNode();
	}
	nodePtr->readFromFile( fp );
	return nodePtr;
}

template<typename KEY>
bool Trie<KEY>::queryFromMem( const KEY& begin, const KEY& end, std::vector<VALUE>& ret) {
	TrieNode*	 nodePtr = root, *nextPtr = NULL;
	uint8_t 	depth = (sizeof(KEY)<<3)/BITS_PER_NODE ;
	// bit number of left shift and right shift
	uint8_t rsnum = (depth-1)*BITS_PER_NODE;
	queue<TrieNode*> TQ[2];
	queue<KEY>	 prefix[2];
	KEY		prefixValue, currentQueue, currentValue;
	KEY		prefixBegin, prefixEnd;
	int pos = 0 ;

	// read root of trie tree
	if( nodePtr == NULL) {
		ERROR_INFO("Empty Trie tree!\n", goto ERROR);
	}

	TQ[0].push( nodePtr );
	prefix[0].push( 0 );

	for( int i = 0; i < depth; ++i ) {
		currentQueue = (i&1);
		prefixBegin = begin>>rsnum;
		prefixEnd = end>>rsnum; 
		rsnum -= BITS_PER_NODE;

#ifdef PRINT
        //printf("%d size = %d\n", i, TQ[currentQueue].size());
#endif
		if( TQ[currentQueue].empty() ) goto ERROR;
        //printf("%d size = %d\n", i, TQ[currentQueue].size());

        while( !TQ[currentQueue].empty() ) {
            nodePtr = TQ[currentQueue].front(); 
            assert( !prefix[currentQueue].empty() );
            prefixValue = prefix[currentQueue].front()<<BITS_PER_NODE; 

            if (nodePtr->isLeafNode()) {
                // TODO: using binary-search;	
                //printf("elemcount = %d\n", nodePtr->getElemCount());
                for( pos = 0 ; pos < nodePtr->getElemCount(); ++pos) {
                    //printf("pos = %d, idx = %d, begin = %d\n", pos, nodePtr->getOnePosition(pos), begin);
                    if( prefixValue + nodePtr->getOnePosition(pos) >= begin ) 
                        break;
                }
                int idx = 0;
                while( pos < nodePtr->getElemCount() ) {
                    idx = nodePtr->getOnePosition(pos);
                    if( prefixValue + idx > end ) break;

                    LinkList<VALUE>* list = ((LeafTrieNode*)nodePtr)->getResult(idx);
                    if ( NULL != list ){
                       // list->copyAndMerge(ret);
                       //
                        std::vector<VALUE> temp;
                        if(list->begin() != NULL){
                            //struct LinkElem *curA = list->begin();
                            LinkList<VALUE>::LinkNode *curA = list->begin();
                            int idx = 0;
                            int idx2 = 0;
                            while( curA!=NULL || idx<ret.size()) {
                                if( curA == NULL || ( idx<ret.size() && curA->data[idx2].offset > ret[idx].offset )) {
                                    temp.push_back( ret[idx++]);
                                } else {
                                    temp.push_back( curA->data[idx2++]);
                                    if(idx2 >= curA->elem_num){
                                        curA = curA->next;
                                        idx2= 0;
                                    }
                                }
                            }

                            ret.swap( temp );

                        }

                    }
                    //printf("pos = %d, idx = %d\n", pos, idx );
                    //leafRes->readFromFileAndMerge( fp, ret );
                    ++pos;
                }
            } else {
                // TODO: using binary-search;	
                // TODO: find start position of prefixBegin-prefixValue by binary-search
                for( pos = 0 ; pos < nodePtr->getElemCount(); ++pos) {
                    if( prefixValue + nodePtr->getOnePosition(pos) >= prefixBegin ) 
                        break;
                }
                int idx = 0;
                while( pos < nodePtr->getElemCount() ) {
                    idx = nodePtr->getOnePosition(pos);
                    if( prefixValue + idx > prefixEnd ) break;
                    else {
                        if( (nextPtr = ((InterTrieNode*)nodePtr)->getChildren(idx)) == NULL ) {
                            continue;
                        }
                        TQ[currentQueue^1].push( nextPtr );
                        prefix[currentQueue^1].push( prefixValue + idx );
                    }
                    ++pos;
				}
			}
			TQ[currentQueue].pop();
			prefix[currentQueue].pop();
		}
	}

	return true;
ERROR:
	return false;
}

/*
template<typename KEY>
std::vector<VALUE> Trie<KEY>::queryFromFile( const char *fileName, const KEY& begin, const KEY& end) {
	FILE * fp = fopen( fileName , "rb" );
	if( !fp ) {
		fprintf( stderr, "Open file %s failed at file %s, in function %s, line %d\n", fileName, __FILE__, __func__, __LINE__);
		return NULL;
	}

    //OffsetsResult* ret = new OffsetsResult();
    std::vector<VALUE> ret;
    TrieNode*	 nodePtr = NULL, *nextPtr = NULL;
	uint32_t*	offsetArray = NULL, *linklistOff = NULL;
	uint32_t 	depth = (sizeof(KEY)<<3)/BITS_PER_NODE ;
	// bit number of left shift and right shift
	uint32_t rsnum = (depth-1)*BITS_PER_NODE;
	queue<TrieNode*> TQ[2];
	queue<KEY>	 prefix[2];
	KEY		prefixValue, currentQueue, currentValue;
	KEY		prefixBegin, prefixEnd;
	int pos = 0 ;

	// read root of trie tree
	if( (nodePtr = readTrieNode(fp)) == NULL) {
		ERROR_INFO("Read trie node error", goto ERROR);
	}

	TQ[0].push( nodePtr );
	prefix[0].push( 0 );

	for( int i = 0; i < depth; ++i ) {
		currentQueue = (i&1);
		prefixBegin = begin>>rsnum;
		prefixEnd = end>>rsnum; 
		rsnum -= BITS_PER_NODE;

#ifdef PRINT
        //printf("%d size = %d\n", i, TQ[currentQueue].size());
#endif

		if( TQ[currentQueue].empty() ) goto ERROR;

        while( !TQ[currentQueue].empty() ) {
			nodePtr = TQ[currentQueue].front(); 
			assert( !prefix[currentQueue].empty() );
			prefixValue = prefix[currentQueue].front()<<BITS_PER_NODE; 
			offsetArray = nodePtr->getFileOffset();
			assert( offsetArray != NULL );

            if (nodePtr->isLeafNode()) {
                assert( i == depth-1 );
                // now we need to process leaf trie nodes 
                // TODO: using binary-search;	
                for( pos = 0 ; pos < nodePtr->getElemCount(); ++pos) {
                    if( prefixValue + nodePtr->getOnePosition(pos) >= begin ) 
                        break;
                }
                if( fseek( fp, offsetArray[pos], SEEK_CUR ) == -1 ) {
                    ERROR_INFO("Seeking file position failed", goto ERROR);
                }
                while( pos < nodePtr->getElemCount() ) {
                    if( prefixValue + nodePtr->getOnePosition(pos)\
                            > end ) break;
                    LinkList<VALUE>* leafRes = new LinkList<VALUE>();
                    leafRes->readFromFileAndMerge( fp, ret );
#ifdef PRINT
                    printf("after read from file, size = %d\n!", leafRes->size());
#endif
                    delete leafRes;
                    leafRes = NULL;
                    ++pos;
                }
            } else {
                // TODO: find start position of prefixBegin-prefixValue by binary-search
                for( int j = 0 ; j < nodePtr->getElemCount(); ++j ) {
                    currentValue = nodePtr->getOnePosition(j) + prefixValue;
#ifdef PRINT
                    //printf("j = %d size = %d\n", j, nodePtr->getElemCount());
#endif
                    if( currentValue >= prefixBegin ) {
                        if( currentValue > prefixEnd ) break;
                        else {
                            if( fseek( fp, offsetArray[j], SEEK_SET ) == -1 ) {
                                ERROR_INFO("Seeking file position failed", goto ERROR);
                            }
                            if( (nextPtr=readTrieNode(fp)) == NULL ) {
                                ERROR_INFO("Read trie node error", goto ERROR);
                            }
                            TQ[currentQueue^1].push( nextPtr );
                            prefix[currentQueue^1].push( currentValue );
                        }
                    }
                }
            }
            TQ[currentQueue].pop();
            prefix[currentQueue].pop();
            delete nodePtr;
            offsetArray = NULL;
		}
	}

	fclose( fp );
	return ret;
ERROR:
	fclose( fp );
	for( int i = 0 ; i < 2; ++i ) {
		while( !TQ[i].empty() ) {
			delete TQ[i].front();
			TQ[i].pop();
		}
	}
    ret.clear();
	return NULL;
}
*/

template<typename KEY>
bool Trie<KEY>::queryFromFile( const char *fileName, const KEY& begin, const KEY& end, std::vector<VALUE>& ret) {

   // printf("Trie::queryFromFile: begin = %d, end = %d \n", begin, end);
	FILE * fp = fopen( fileName , "rb" );
	if( !fp ) {
		fprintf( stderr, "Open file %s failed at file %s, in function %s, line %d\n", fileName, __FILE__, __func__, __LINE__);
		return false;
	}

	TrieNode*	 nodePtr = NULL, *nextPtr = NULL;
	uint32_t*	offsetArray = NULL, *linklistOff = NULL;
	uint32_t 	depth = (sizeof(KEY)<<3)/BITS_PER_NODE ;
	// bit number of left shift and right shift
	uint32_t rsnum = (depth-1)*BITS_PER_NODE;
	queue<TrieNode*> TQ[2];
	queue<KEY>	 prefix[2];
	KEY		prefixValue, currentQueue, currentValue;
	KEY		prefixBegin, prefixEnd;
	int pos = 0 ;

	// read root of trie tree
	if( (nodePtr = readTrieNode(fp)) == NULL) {
		ERROR_INFO("Read trie node error", goto ERROR);
	}

	TQ[0].push( nodePtr );
	prefix[0].push( 0 );

	for( int i = 0; i < depth; ++i ) {
		currentQueue = (i&1);
		prefixBegin = begin>>rsnum;
		prefixEnd = end>>rsnum; 
		rsnum -= BITS_PER_NODE;

#ifdef PRINT
        //printf("%d size = %d\n", i, TQ[currentQueue].size());
#endif

		if( TQ[currentQueue].empty() ) goto ERROR;

        while( !TQ[currentQueue].empty() ) {
			nodePtr = TQ[currentQueue].front(); 
			assert( !prefix[currentQueue].empty() );
			prefixValue = prefix[currentQueue].front()<<BITS_PER_NODE; 
			offsetArray = nodePtr->getFileOffset();
			assert( offsetArray != NULL );

            if (nodePtr->isLeafNode()) {
       //         printf("isLeafNode()\n");
                assert( i == depth-1 );
                // now we need to process leaf trie nodes 
                // TODO: using binary-search;	
                for( pos = 0 ; pos < nodePtr->getElemCount(); ++pos) {
                    if( prefixValue + nodePtr->getOnePosition(pos) >= begin ) 
                        break;
                }
                if( fseek( fp, offsetArray[pos], SEEK_CUR ) == -1 ) {
                    ERROR_INFO("Seeking file position failed", goto ERROR);
                }
     //           printf("pos = %d, nodePtr->getElemCount() = %d \n ", pos, nodePtr->getElemCount());
                while( pos < nodePtr->getElemCount() ) {
           //         printf("into while loop\n");

                    if( prefixValue + nodePtr->getOnePosition(pos)\
                            > end ) break;
                    LinkList<VALUE>* leafRes = new LinkList<VALUE>();
                    leafRes->readFromFileAndMerge( fp, ret );
         //           printf("Trie::queryFromFile ret size = %d\n", ret.size());
#ifdef PRINT
                    printf("after read from file, size = %d\n!", leafRes->size());
#endif
                    delete leafRes;
                    leafRes = NULL;
                    ++pos;
                }
            } else {
             //   printf("InterNode\n");
                // TODO: find start position of prefixBegin-prefixValue by binary-search
                for( int j = 0 ; j < nodePtr->getElemCount(); ++j ) {
                    currentValue = nodePtr->getOnePosition(j) + prefixValue;
#ifdef PRINT
                    //printf("j = %d size = %d\n", j, nodePtr->getElemCount());
#endif
                    if( currentValue >= prefixBegin ) {
                        if( currentValue > prefixEnd ) break;
                        else {
                            if( fseek( fp, offsetArray[j], SEEK_SET ) == -1 ) {
                                ERROR_INFO("Seeking file position failed", goto ERROR);
                            }
                            if( (nextPtr=readTrieNode(fp)) == NULL ) {
                                ERROR_INFO("Read trie node error", goto ERROR);
                            }
                            TQ[currentQueue^1].push( nextPtr );
                            prefix[currentQueue^1].push( currentValue );
                        }
                    }
                }
            }
            TQ[currentQueue].pop();
            prefix[currentQueue].pop();
            delete nodePtr;
            offsetArray = NULL;
		}
	}

	fclose( fp );
	return true;
ERROR:
	fclose( fp );
	for( int i = 0 ; i < 2; ++i ) {
		while( !TQ[i].empty() ) {
			delete TQ[i].front();
			TQ[i].pop();
		}
	}
	return false;
}



/*
template<typename KEY>
LinkList<VALUE>* Trie<KEY>::queryFromFile( const char *fileName, const KEY& begin, const KEY& end){
	FILE * fp = fopen( fileName , "rb" );
	if( !fp ) {
		fprintf( stderr, "Open file %s failed at file %s, in function %s, line %d\n", fileName, __FILE__, __func__, __LINE__);
		return NULL;
	}

	LinkList<VALUE>* ret = NULL, *leafRes = NULL;
	TrieNode*	 nodePtr = NULL, *nextPtr = NULL;
	uint32_t*	offsetArray = NULL, *linklistOff = NULL;
	uint8_t 	depth = (sizeof(KEY)<<3)/BITS_PER_NODE ;
	// bit number of left shift and right shift
    uint32_t rsnum = (depth-1)*BITS_PER_NODE;
	queue<TrieNode*> TQ[2];
	queue<KEY>	 prefix[2];
	KEY		prefixValue, currentQueue, currentValue;
	KEY		prefixBegin, prefixEnd;
	int pos = 0 ;

	// read root of trie tree
	if( (nodePtr = readTrieNode(fp)) == NULL) {
		ERROR_INFO("Read trie node error", goto ERROR);
	}

	TQ[0].push( nodePtr );
	prefix[0].push( 0 );
	ret = new LinkList<VALUE>();

	for( int i = 0; i < depth; ++i ) {
		currentQueue = (i&1);
		prefixBegin = begin>>rsnum;
		prefixEnd = end>>rsnum; 
		rsnum -= BITS_PER_NODE;

#ifdef PRINT
        //printf("%d size = %d\n", i, TQ[currentQueue].size());
#endif

		if( TQ[currentQueue].empty() ) goto ERROR;

        while( !TQ[currentQueue].empty() ) {
			nodePtr = TQ[currentQueue].front(); 
			assert( !prefix[currentQueue].empty() );
			prefixValue = prefix[currentQueue].front()<<BITS_PER_NODE; 
			offsetArray = nodePtr->getFileOffset();
			assert( offsetArray != NULL );

            if (nodePtr->isLeafNode()) {
                assert( i == depth-1 );
                // now we need to process leaf trie nodes 
                // TODO: using binary-search;	
                for( pos = 0 ; pos < nodePtr->getElemCount(); ++pos) {
                    if( prefixValue + nodePtr->getOnePosition(pos) >= begin ) 
                        break;
                }
                if( fseek( fp, offsetArray[pos], SEEK_CUR ) == -1 ) {
                    ERROR_INFO("Seeking file position failed", goto ERROR);
                }
                while( pos < nodePtr->getElemCount() ) {
                    if( prefixValue + nodePtr->getOnePosition(pos)\
                            > end ) break;
                    //LinkList<VALUE>* leafRes = new LinkList<VALUE>();
                    //leafRes->readFromFileAndMerge( fp, ret );
                    ret->readFromFileAndMerge( fp );
                    ++pos;
                }
            } else {
                // TODO: find start position of prefixBegin-prefixValue by binary-search
                for( int j = 0 ; j < nodePtr->getElemCount(); ++j ) {
                    currentValue = nodePtr->getOnePosition(j) + prefixValue;
#ifdef PRINT
                    //printf("j = %d size = %d\n", j, nodePtr->getElemCount());
#endif
                    if( currentValue >= prefixBegin ) {
                        if( currentValue > prefixEnd ) break;
                        else {
                            if( fseek( fp, offsetArray[j], SEEK_SET ) == -1 ) {
                                ERROR_INFO("Seeking file position failed", goto ERROR);
                            }
                            if( (nextPtr=readTrieNode(fp)) == NULL ) {
                                ERROR_INFO("Read trie node error", goto ERROR);
                            }
                            TQ[currentQueue^1].push( nextPtr );
                            prefix[currentQueue^1].push( currentValue );
                        }
                    }
                }
            }
            TQ[currentQueue].pop();
            prefix[currentQueue].pop();
            delete nodePtr;
            offsetArray = NULL;
		}
	}

	fclose( fp );
	return ret;
ERROR:
	fclose( fp );
	for( int i = 0 ; i < 2; ++i ) {
		while( !TQ[i].empty() ) {
			delete TQ[i].front();
			TQ[i].pop();
		}
	}
	delete ret;
	return NULL;
}
*/
template<typename KEY>
int Trie<KEY>::readFromFile( const char *fileName ) {
	FILE * fp = fopen( fileName, "rb");
	if( !fp ) {
		ERROR_INFO("open file failed", return -1);
	}

	int8_t  isLeafNode;
	size_t	isLeafSize = sizeof( isLeafNode );
	TrieNode* curNode = NULL, *newNode = NULL;
	queue< TrieNode *>TQ;

	if( fread( &isLeafNode, isLeafSize, 1, fp ) != 1 ) {
		ERROR_INFO("Read file error", goto ERROR);
	}
	// this tree must be an empty tree when we called this function readFromFile 
	assert( root != NULL );

	root->readFromFile( fp );
	if( !isLeafNode ) {
		TQ.push( root );
	}

	while( !TQ.empty() ) {
		curNode = TQ.front(); TQ.pop(); 

		for( int i = 0 ; i < curNode->getElemCount(); ++i ) {
			fread( &isLeafNode, isLeafSize, 1, fp );
#ifndef	NODE_POOL
			if( isLeafNode ) newNode = new LeafTrieNode();
			else	newNode = new InterTrieNode();
#else
			if( isLeafNode ) newNode = Trie<KEY>::getLeafNode(); 
			else	newNode = Trie<KEY>::getInterNode();
#endif
			nodeCount++;

			if( newNode->readFromFile( fp ) == -1 ) {
				ERROR_INFO("Something wrong in readFromFile", goto ERROR);
			}

			((InterTrieNode*)curNode)->setChildren( curNode->getOnePosition(i), newNode );
			if( !isLeafNode ) {
				TQ.push( newNode );
			} else {
				((LeafTrieNode*)newNode)->readLinklist( fp );	
			}
		}
	}
	fclose( fp );
	return 0;
ERROR:
	fclose( fp );
	return -1;
}
