
#ifndef _TRIE_H
#define _TRIE_H

#include "trie_node.h"
#include "linklist.h"
#include <vector>
#include "resourcePool.h"

#define POOL_MAX 500000
#define ELEM_NUM_EACH_NODE 8
/*
struct LinkElem {
    VALUE data[ ELEM_NUM_EACH_NODE ];
    uint8_t elem_num;
    LinkElem *next;
};

*/

template<typename KEY>
class Trie {
	private:
		TrieNode*   root;
		uint32_t	nodeCount;
		//typedef 	uint32_t VALUE;
		typedef struct LISTELEM VALUE;
		#define		MASK	(MAX_PER_NODE-1)
#ifdef	NODE_POOL
		static		ResourcePool<InterTrieNode*,POOL_MAX> interNodePool;
		static		ResourcePool<LeafTrieNode*,POOL_MAX>  leafNodePool; 
#endif

	public:

#ifdef	NODE_POOL
		static void initPool( uint32_t );

		static void clearPool();

		static inline InterTrieNode* getInterNode() {
			return Trie<KEY>::interNodePool.get_free_resource();
		}

		static inline LeafTrieNode* getLeafNode() {
			return Trie<KEY>::leafNodePool.get_free_resource();
		}
#endif

		Trie() {
#ifdef	NODE_POOL
            if (sizeof(KEY) > 1) root = Trie<KEY>::getInterNode(); 
            else root = Trie<KEY>::getLeafNode();
#endif
			if( sizeof(KEY) > 1 ) root = new InterTrieNode();
			else	root = new LeafTrieNode();
			nodeCount = 1 ;
		}

		~Trie();

        uint32_t getNodeCount() {
            return nodeCount;
        }

		int 	addItem( const KEY& key, const VALUE& value);
		int		readFromFile( const char *);
		int 	write2file( const char *);
		TrieNode*	readTrieNode( FILE* );
        //std::vector<VALUE> queryFromFile( const char *name, const KEY& begin, const KEY& end );
		//LinkList<VALUE>* queryFromFile( const char *name, const KEY& begin, const KEY& end );
        bool queryFromFile( const char *name, const KEY& begin, const KEY& end, std::vector<VALUE>& vec );
        bool queryFromMem( const KEY& begin, const KEY& end, std::vector<VALUE>& vec );
};

#include "trie.cpp"

#endif
