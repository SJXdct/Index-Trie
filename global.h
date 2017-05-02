/* This is a file stored some global variables.
 * global variables and macro definition should be defined here as much as possible. 
 *
 * NOTE:
 * 	If we changed the dependent libraries like "libtrace" and didn't want to modify other
 * 	modules and code, we should replace the "typedef" in this file. But we should make sure
 *  new data-structure to be typedef must have some needed data members.
 *
 *
 * Create by hth on 24/08/12.
 */

#ifndef GLOBAL_H_
#define	GLOBAL_H_

//#include "libtrace.h"


#define ERROR_INFO(msg,next)  do { \
	fprintf(stderr,"error msg: %s in file %d at line %s\n",\
			__FILE__,__LINE__,msg);\
	next;\
}while(0)

typedef unsigned char Byte;
template<typename T>
inline void swap( T&a, T&b) {
	T tmp = a; a = b; b = tmp;
}

//namespace libtrace {
	//typedef libtrace_t 					TRACE;
	//typedef libtrace_packet_t			PACKET;
	//typedef libtrace_linktype_t			LINKTYPE;
	//typedef	libtrace_ethertype_t		ETHERTYPE;
	//typedef libtrace_ether_t 			ETHERHEADER;
	//typedef libtrace_ip_t				IPV4HEADER;
	//typedef	libtrace_ip6_t				IPV6HEADER;
	//typedef	libtrace_icmp_t				ICMPHEADER;
	//typedef	libtrace_out_t				FILEOUTPUT;
	//typedef	trace_option_compresstype_t COMPRESSTYPE;
//};

#endif
