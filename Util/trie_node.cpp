#include "trie_node.h"

/* layout of storage in file
 * |-------------------------|
 * |   isLeaf	             |
 * |   elemCount             |
 * |   bitmap->			 |
 * |   1st child offset      |
 * |   2nd child offset      |
 * |   .........             |
 * |   last child offset     |
 * |-------------------------|
 */
// we must have given a realistic array address to the variable fileOffArray 
// before we calling the funtion write2file, because we cannot get the writing offset 
// of each child TrieNode in file in real-time. So we must calculate the offset of each child TrieNode
// and store this value into an array in advance .
// NOTE: size of single BTree file must be less than 4G because offset is int32_t type 

// file IO
int InterTrieNode::write2file( FILE * fp) {
	if( !fp ) {
		ERROR_INFO("NULL file pointer fp",return -1);
	}

	fwrite( &(isLeaf), sizeof(isLeaf) , 1, fp);
	fwrite( &(elemCount), sizeof(elemCount), 1, fp);
	bitmap->dump2file( fp );

	// we need to calculate the children's offset of file in advance. 
	assert( fileOffArray != NULL );
	fwrite( fileOffArray, sizeof( uint32_t ) * elemCount , 1, fp );
	fileOffArray = NULL; // make sure that each calling of this function should call function setFileOffset firstly.
	return 0;
}

int InterTrieNode::readFromFile( FILE *fp ) {
	if( !fp ) {
		ERROR_INFO("NULL file pointer fp",return -1);
	}
	
	//we have readed isLeaf value before we call this function
	//fread( &(isLeaf), sizeof(isLeaf) , 1, fp);
	fread( &(elemCount), sizeof(elemCount), 1, fp);
	bitmap->readFromFile( fp );
	fileOffArray = new uint32_t[ elemCount ];
	//read file-offset of linklist
	fread( fileOffArray, sizeof(uint32_t)*elemCount, 1, fp );
	return 0;
}

// get the total size that we writed this InternalNode Object in file
int32_t InterTrieNode::getSizeOfFile() {
	int32_t ret = sizeof( elemCount ) + sizeof(isLeaf);
	ret += bitmap->getSizeOfFile();
	ret += elemCount * sizeof( uint32_t ) ;
	return ret;
}


int InterTrieNode::addItem( const uint8_t& pos, TrieNodePtr child) {
	if( elemCount >= MAX_PER_NODE ||
			pos < 0 || pos >= MAX_PER_NODE ) {
		ERROR_INFO("wrong parameter pos",return -1);
	}

	childPtr[pos] = child;
	++elemCount;
	setBit( pos );
	return 0;
}

void InterTrieNode::clear(){
	for( int i = 0 ; i < MAX_PER_NODE; ++i ) childPtr[i] = NULL;
}

LeafTrieNode::LeafTrieNode() : TrieNode( 1 ) {
	for( int i = 0 ; i < MAX_PER_NODE; ++i ) resultArray[i] = NULL;
}

LeafTrieNode::~LeafTrieNode() {
	/*
	if( elemCount >= (MAX_PER_NODE>>1) ) {

		for( int i = 0 ; i < MAX_PER_NODE; ++i )
			if( resultArray[i] ) 
				delete resultArray[i];
				
	} else {
	*/
		for( int i = 0 ; i < elemCount; ++i ) {
			delete resultArray[ getOnePosition(i) ]; 
		}
	
}

void LeafTrieNode::clear() {
	for( int i = 0 ; i < MAX_PER_NODE; ++i ) {
		if( resultArray[i] ) {
			delete resultArray[i];
			resultArray[i] = NULL;
		}
	}
}

int LeafTrieNode::addItem( const uint8_t& pos, const VALUE& value ) {
	if( resultArray[pos] == NULL ) {
		resultArray[pos] = new LinkList<VALUE>();
		if( !resultArray[pos] ) {
			ERROR_INFO("cannot alloc memory for linklist", return -1);
		}
		resultArray[pos]->clear();
		++elemCount;
		setBit( pos );
	}
	resultArray[pos]->addElement(value);
    //printf("offset = %d, num = %d \n", value.offset, value.packets_num);
	return 0;
}

LinkList<VALUE>* LeafTrieNode::getResult( const uint8_t idx ) {
	assert( idx >= 0 && idx <= MAX_PER_NODE );
	assert( resultArray[idx] != NULL );
	return resultArray[idx];
}

/* layout of storage in file
 * |-------------------------|
 * |   isLeaf	             |
 * |   elemCount             |
 * |   bitmap->               |
 * |   offset Array          |
 * |   1st values' linklist  |
 * |   2nd values' linklist  |
 * |   .........             |
 * |   last values' linklist |
 * |-------------------------|
 */

int LeafTrieNode::write2file( FILE * fp) {
	if( !fp ) {
		ERROR_INFO("NULL file pointer fp",return -1);
	}

	fwrite( &(isLeaf), sizeof(isLeaf), 1, fp);
	fwrite( &(elemCount), sizeof(elemCount), 1, fp);
	bitmap->dump2file( fp );
	uint32_t offset = 0;
	getBitPosition();

	// write file-offset of linklist
    //printf("-----------------------LeafTrieNode::write2file()------------ \n");
	for( int i = 0 ; i < elemCount ; ++i ) {
		fwrite( &offset, sizeof(offset), 1 , fp );
      //  printf(" offset %d is %d\n", i, offset);
       // printf(" num %d is %d, num*8 is %d\n", i, resultArray[bitPosition[i]]->size(), 8*resultArray[bitPosition[i]]->size());
		/*
		if( resultArray[bitPosition[i]] == NULL ) {
			for( int j = 0 ; j < elemCount; ++j ) {
				printf("%d ",bitPosition[j]);
			}
			printf("Count = %d\n", elemCount );
		}
		*/
		assert( resultArray[ bitPosition[i] ] != NULL );
		offset += resultArray[ bitPosition[i] ]->getSizeOfFile();
	}
	// write result-linklist
    //printf("-----------------------LeafTrieNode::write2file(): write result linklist to file------------ \n");
	for( int i = 0 ; i < elemCount ; ++i ) {
      //  printf("elemCount %d \n", i);
		if( resultArray[ bitPosition[i] ]->write2file( fp ) == -1 ) {
			ERROR_INFO("something error in linklist writing of leaf trie node\n",return -1);
		}
	}
	return 0;	
}

int LeafTrieNode::readFromFile( FILE * fp) {
	if( !fp ) {
		ERROR_INFO("NULL file pointer fp",return -1);
	}
	//we have read isLeaf value before we call this function
	//read( fd, &(isLeaf), sizeof(isLeaf));
	fread( &(elemCount), sizeof(elemCount), 1, fp);
	bitmap->readFromFile( fp );
	fileOffArray = new uint32_t[ elemCount ];
	//read file-offset of linklist
	fread( fileOffArray, sizeof(uint32_t), elemCount, fp );
	return 0;
}

int LeafTrieNode::readLinklist( FILE* fp ) {
	if( elemCount >= (MAX_PER_NODE>>1) ) {
		// get one-bit positions of bitmap
		for( int i = 0 ; i < elemCount ; ++i ) {
			int pos = getOnePosition( i );
			resultArray[pos] = new LinkList<VALUE>();
			if( resultArray[pos]->readFromFile( fp ) == -1 ) {
				return -1;
			}
		}
	} else {
		for( int i = 0 ; i < MAX_PER_NODE; ++i ) {
			if( bitmap->getBit(i) ) {
				resultArray[i] = new LinkList<VALUE>();
				if( resultArray[i]->readFromFile( fp ) == -1 ) {
					return -1;
				}
			}
		}
	}
	return 0;
}	

// get the total size that we writed this LeafNode Object in file
int32_t LeafTrieNode::getSizeOfFile() {
	int32_t ret = sizeof( elemCount ) + sizeof( isLeaf);
	ret += bitmap->getSizeOfFile();
	// for offset array size
	ret += elemCount * sizeof(uint32_t);

	if( elemCount >= (MAX_PER_NODE>>1) ) {
		// get one-bit positions of bitmap
		for( int i = 0 ; i < elemCount ; ++i ) {
			int pos = getOnePosition( i );
			ret += resultArray[pos]->getSizeOfFile();
		}
	} else {
		for( int i = 0 ; i < MAX_PER_NODE; ++i ) {
			if( bitmap->getBit(i) ) {
				ret += resultArray[i]->getSizeOfFile();
			}
		}
	}

	return ret;
}
