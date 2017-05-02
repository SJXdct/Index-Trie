/*=============================================================================
#     FileName: command_parser.yy
#     Copyright: All rights reserved.
#         Desc:  
#       Author: Taihua He
#        Email: hetaihua@ict.ac.cn
#     HomePage: 
#      Version: 0.0.1
#   LastChange: 2014-02-17 10:20:26
#      History:
=============================================================================*/

%name_prefix="command"
%debug
%error-verbose
%locations

// Make the parser re-entrant
//%pure-parser

%{ 
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
    #include <stdint.h>
	#include <netinet/in.h>
	#include <pthread.h>
	#include <string>
    #include <time.h>

    #include "../storage/storage.h"
    /*#include "../storage/index.h"*/
    /*using storage::Storage;*/
    using namespace storage;

    #include "query.h"

	// Work around a bug in the relation between bison and GCC 3.x:
	#if defined (__GNUC__) && 3 <= __GNUC__
	#define __attribute__(arglist)
	#endif

	/* Internal datastructures whilse parsing */ 
	/*static QueryRequest * q_req;*/
	/*static QueryResult *q_res; */
	/*static IndexField *q_idxfield;*/
	/*static bool q_mem_only;*/
	/*static bool q_subscribe;*/
    static OffsetsResult *result = NULL;
	static uint64_t start_time;
	static uint64_t end_time;
    /*static char*  sip_index_file;*/
    /*static char*  dip_index_file;*/
    /*static char*  sport_index_file;*/
    /*static char*  dport_index_file;*/
    /*static char*  proto_index_file;*/
    static char*  save_file_name;
	/*static broccoli_worker_thread_data* q_bc_thread;*/

	/*static int nextQueryID = 0;*/

	//extern int yylex(YYSTYPE *lvalp, YYLTYPE *llocp);
	extern int yylex();
	struct yy_buffer_state;
	static pthread_mutex_t cmd_parser_lock;

	typedef struct yy_buffer_state *YY_BUFFER_STATE;
	extern void command_delete_buffer (YY_BUFFER_STATE b);
	extern YY_BUFFER_STATE command_scan_string (const char *yy_str);

	int commanderror(const char*);
	int cmd_parse_lineno=0;
	int cmd_parse_errors=0;
	//  const char* parse_filename=NULL;

	/*
	 * symbols shared with main.cc
	 */
	/*Storage* cmd_parser_storage=NULL;*/
    /*static Storage*       storage = NULL;*/
    static QueryRequest*  request = NULL;
    static Query*         query = NULL;
    FILE *cmd_parser_outfp;
	extern void print_stats(FILE *fp);
	/*extern pcap_t* ph;*/
%}

%union {
	uint32_t i;
	int64_t i64;
	char* s;
	double d;
    char *time;
    char *day;
    char *ipaddress;
    OffsetsResult* offsets_result;
	/*IndexField* indexfield_p;*/
	/*IPAddress* ipaddress_p;*/
	/*ConnectionID4* connectionid4_p;*/
	/*QueryResult* queryresult_p;*/
	/*QueryRequest* queryrequest_p;*/
}

%token SEMICOLON COLON DASH ASTERISK
%token LBRACE RBRACE LBRACK RBRACK
%token EQUALITY
%token LPAREN RPAREN
%left LPAREN RPAREN
%left  AND OR
%token <i> TOK_INTEGER
%token <s> TOK_STRING
%token <s> TOK_TIME
%token <s> TOK_DAY
%token <d> TOK_DOUBLE
%token <s> TOK_ID
%token <ipaddress> TOK_IP_ADDR
/*%token TOK_SET_DYN_CLASS TOK_UNSET_DYN_CLASS TOK_ORIG TOK_RESP*/
/*%token TOK_CLASS TOK_FILTER TOK_MAIN TOK_LOG_INTERVAL TOK_DEVICE*/
/*%token TOK_LOGFILE TOK_WORKDIR*/
/*%token TOK_READ_TRACEFILE TOK_BRO_CONNECT_STR*/
/*%token TOK_MEM TOK_DISK TOK_K TOK_M TOK_G TOK_CUTOFF TOK_PRECEDENCE*/
/*%token TOK_NO TOK_PKTS_TO_DISK TOK_CONSOLE TOK_MAX_INDEX_ENTRIES*/
/*%token TOK_FILESIZE TOK_CONN_TIMEOUT*/
/*%token TOK_QUERY TOK_FEED TOK_TO_FILE TOK_SUBSCRIBE*/
/*%token TOK_CONN TOK_START TOK_END TOK_TAG TOK_TS TOK_PROTO*/
/*%token TOK_PRINT TOK_NOP*/
/*%token TOK_BRO_CONNECT TOK_SUSPEND_CUTOFF TOK_UNSUSPEND_CUTOFF*/
/*%token TOK_SUSPEND_TIMEOUT TOK_UNSUSPEND_TIMEOUT*/
/*%token TOK_SHOW TOK_STATS TOK_CONNS TOK_INDEX TOK_MEM_ONLY*/
/*%token TOK_DEBUG_FIFOMEM TOK_SAMPLE*/
/*%token TOK_DAY TOK_TIME*/
%token TOK_SIP TOK_DIP TOK_SPORT TOK_DPORT TOK_PROTO TOK_TIME_START TOK_TIME_END
%token TOK_SAVE TOK_COUNT TOK_SHOW
%token TOK_HELP

%type <i64> timestamp
/*%type <s> string_id*/
/*%type <connectionid4_p> connection4*/
/*%type <i> orig_or_resp*/
%type <s> save_file_name
%type <offsets_result> filter
%type <offsets_result> nonempty_filter
%type <i64> start_time
%type <i64> end_time


%start command

%%

command:
/*// debugging-the-scanner-and-parser commands*/
	/*TOK_NOP { */
		/*if(cmd_parser_outfp)*/
			/*fprintf(cmd_parser_outfp, "TOK_NOP\n"); */
	/*}*/
	/*| TOK_PRINT  {*/
		/*if(cmd_parser_outfp)*/
			/*fprintf(cmd_parser_outfp, "PRINT\n");*/
	/*}*/
	/*| TOK_PRINT TOK_STRING {*/
		/*if(cmd_parser_outfp)*/
			/*fprintf(cmd_parser_outfp, "PRINT string \"%s\"\n", $2);*/
		/*free($2);*/
	/*}*/
	/*| */
    TOK_HELP {
		if (cmd_parser_outfp) {
			fprintf(cmd_parser_outfp, "Online help not yet implemented. See Documentation/TM_HOWTO\n");
		}
        if (request != NULL) {
            request->query_type = HELP;
        }
	}
// query
    | TOK_SAVE save_file_name filter start_time end_time {
		if (cmd_parser_outfp) {
            fprintf(cmd_parser_outfp, "save filter parsing, file_name = %s!\n", $2);
		}
        if (request != NULL) {
            request->query_type = SAVE;
            /*fprintf(cmd_parser_outfp, "save type, request query_type = %d\n", request->query_type);*/
        } else {
            /*fprintf(cmd_parser_outfp,  "request pointer NULL!!!!!\n");*/
        }
    }
    | TOK_COUNT filter start_time end_time {
		if (cmd_parser_outfp) {
			fprintf(cmd_parser_outfp, "count filter\n");
		}
        if (request != NULL) {
            request->query_type = COUNT;
        }
    }
    | TOK_SHOW filter start_time end_time {
		if (cmd_parser_outfp) {
			fprintf(cmd_parser_outfp, "commmand_parser.yy:line 197:show filter\n");
		}
        if (request != NULL) {
            request->query_type = SHOW;
        }
    }
    ;

filter:
    /*empty*/ {
        if (query == NULL) {
            if (cmd_parser_outfp) {
                fprintf(cmd_parser_outfp, "NULL pointer query!!!\n");
            }
            $$ = NULL;
        } else {
            uint32_t ip_begin = 0, ip_end = 0xffffffff;
            if (request == NULL) {
                $$ = query->getMemSipOffsets(ip_begin, ip_end);
            } else {
                $$ = query->getOffsets( request->sip_index_file, SRCIP, ip_begin, ip_end);
            }
            if (cmd_parser_outfp && $$ != NULL) {
                fprintf(cmd_parser_outfp, "Sip Filter parsing, size = %d!\n",
                ((OffsetsResult*)$$)->size());
            }
        }
    }

    | nonempty_filter {
        if (cmd_parser_outfp && $1 != NULL) {
            fprintf(cmd_parser_outfp, "() nonempty_filter parsing, size = %d!\n",
            ((OffsetsResult*)$1)->size());
		}
        result = $1;
        $$ = $1;
    }
    ;

nonempty_filter:
                LPAREN nonempty_filter RPAREN {
                    $$ = $2;
                    if (cmd_parser_outfp && $$ != NULL) {
                        fprintf(cmd_parser_outfp, "() nonempty_filter parsing, size = %d!\n",
                        ((OffsetsResult*)$$)->size());
                    }
                }
                | nonempty_filter AND nonempty_filter {
                    if ($1 == NULL || $3 == NULL) {
                        $$ = NULL;
                        delete($3);
                        delete($1);
                    } else {
                        ((OffsetsResult*)$1)->intersection( (OffsetsResult*)$3 );
                        $$ = $1;
                        if (cmd_parser_outfp && $$ != NULL) {
                            fprintf(cmd_parser_outfp, "And nonempty_filter parsing, size = %d!\n",
                            ((OffsetsResult*)$$)->size());
                        }
                        delete($3);
                    }
                }
                | nonempty_filter OR nonempty_filter {
                    if ($1 == NULL || $3 == NULL) {
                        $$ = NULL;
                        delete($1);
                        delete($3);
                    } else {
                        ((OffsetsResult*)$1)->merge( (OffsetsResult*)$3 );
                        $$ = $1;
                        if (cmd_parser_outfp && $$ != NULL) {
                            fprintf(cmd_parser_outfp, "Or nonempty_filter parsing, size = %d!\n",
                            ((OffsetsResult*)$$)->size());
                        }
                        delete($3);
                    }
                }
                | TOK_SIP EQUALITY TOK_IP_ADDR {
                    uint32_t ip_begin = 0, ip_end = 0;
                    char *ptr = strtok( $3, ".");
                    for (int i = 0; i < 4 && ptr; ++i) {
                        if (strcmp(ptr, "*") == 0) {
                            ip_begin <<= 8;
                            ip_end = (ip_end << 8)|0xff;
                        } else {
                            ip_begin = (ip_begin << 8)|atoi(ptr);
                            ip_end = (ip_end << 8)|atoi(ptr);
                        }
                        ptr = strtok(NULL, ".");
                    }
                    free($3);

                    if (query == NULL) {
                        if (cmd_parser_outfp) {
                            fprintf(cmd_parser_outfp, "NULL pointer query!!!\n");
                        }
                        $$ = NULL;
                    } else {
                        if (request == NULL) {
                            $$ = query->getMemSipOffsets(ip_begin, ip_end);
                        } else {
                            $$ = query->getOffsets( request->sip_index_file, SRCIP, ip_begin, ip_end);
                        }
                        if (cmd_parser_outfp && $$ != NULL) {
                            fprintf(cmd_parser_outfp, "Sip Filter parsing, size = %d!\n",
                            ((OffsetsResult*)$$)->size());
                        }
                    }
                }
                | TOK_DIP EQUALITY TOK_IP_ADDR {
                    uint32_t ip_begin = 0, ip_end = 0;
                    char *ptr = strtok( $3, ".");
                    for (int i = 0; i < 4 && ptr; ++i) {
                        if (strcmp(ptr, "*") == 0) {
                            ip_begin <<= 8;
                            ip_end = (ip_end << 8)|0xff;
                        } else {
                            ip_begin = (ip_begin << 8)|atoi(ptr);
                            ip_end = (ip_end << 8)|atoi(ptr);
                        }
                        ptr = strtok(NULL, ".");
                    }
                    free($3);

                    if (query == NULL) {
                        $$ = NULL;
                    } else {
                        if (request == NULL) {
                            $$ = query->getMemDipOffsets(ip_begin, ip_end); 
                        } else {
                            $$ = query->getOffsets( request->dip_index_file, DSTIP, ip_begin, ip_end);
                        }
                        if (cmd_parser_outfp && $$ != NULL) {
                            fprintf(cmd_parser_outfp, "Sip Filter parsing, size = %d!\n",
                            ((OffsetsResult*)$$)->size());
                        }
                    }
                }
                | TOK_SPORT EQUALITY TOK_INTEGER {
                    if (query == NULL) {
                        $$ = NULL;
                    } else {
                        if (request == NULL) {
                            $$ = query->getMemSportOffsets($3, $3); 
                        } else { 
                            $$ = query->getOffsets( request->sport_index_file, SPORT, $3, $3);
                        }
                        if (cmd_parser_outfp && $$ != NULL ) {
                            fprintf(cmd_parser_outfp, "Sport Filter parsing, size = %d!\n",
                            ((OffsetsResult*)$$)->size());
                        }
                    }
                }
                | TOK_DPORT EQUALITY TOK_INTEGER {
                    if (query == NULL) {
                        $$ = NULL;
                    } else {
                        if (request == NULL) {
                            $$ = query->getMemDportOffsets($3, $3); 
                        } else {  
                            $$ = query->getOffsets( request->dport_index_file, DPORT, $3, $3);
                        }
                        if (cmd_parser_outfp && $$ != NULL) {
                            fprintf(cmd_parser_outfp, "Dport Filter parsing, size = %d!\n",
                            ((OffsetsResult*)$$)->size());
                        }
                    }
                }
                | TOK_PROTO EQUALITY TOK_INTEGER {
                    if (query == NULL) {
                        $$ = NULL;
                    } else {
                        if (request == NULL) {
                            $$ = query->getMemProtoOffsets($3, $3); 
                        } else {  
                            $$ = query->getOffsets( request->proto_index_file, PROTO, $3, $3);
                        }
                        if (cmd_parser_outfp && $$ != NULL) {
                            fprintf(cmd_parser_outfp, "proto Filter parsing, size = %d!\n",
                            ((OffsetsResult*)$$)->size());
                        }
                    }
                }
                ;

start_time:
          /*empty*/ {
            start_time = 0;
            if (request != NULL) {
                request->start_time = 0;
            }
          }
          | TOK_TIME_START EQUALITY timestamp {
            if (cmd_parser_outfp) {
                fprintf(cmd_parser_outfp, "start timestamp = %lld\n", $3);
            }
            if (request != NULL) {
                request->start_time = $3;
            }
            start_time = $3;
            $$ = $3;
          }
          ;

end_time:
        /*empty*/ {
            end_time = 2e10;
            if (request != NULL) {
                request->end_time = 2e10;
            }
        }
        | TOK_TIME_END EQUALITY timestamp {
            if (cmd_parser_outfp) {
                fprintf(cmd_parser_outfp, "end timestamp = %lld\n", $3);
            }
            end_time = $3;
            $$ = $3;
            
            if (request != NULL) {
                request->end_time = $$;
            }
        }
        ;

save_file_name:
              TOK_ID {
                $$ = $1;
                /*save_file_name = $1;*/

                if (request != NULL) {
                    request->save_file_name = std::string($1);
                }
                free($1);
              }
              | TOK_STRING {
                $$ = $1;
                /*save_file_name = $1;*/
                if (cmd_parser_outfp) {
                    fprintf(cmd_parser_outfp, "file name in parsing = %s\n", save_file_name);
                }
                if (request != NULL) {
                    request->save_file_name = std::string($1);
                }
                free($1);
              }
              ;

timestamp:
         TOK_DAY TOK_TIME {
            struct tm t;
            char *ptr = strtok($1, "-");
            t.tm_year = atoi(ptr) - 1900;
            ptr = strtok(NULL, "-");
            t.tm_mon = atoi(ptr) - 1;
            ptr = strtok(NULL, "-");
            t.tm_mday = atoi(ptr);
            ptr = strtok($2,":");
            t.tm_hour = atoi(ptr);
            ptr = strtok(NULL, ":");
            t.tm_min = atoi(ptr);
            t.tm_sec = 0;
            $$ = mktime( &t );
            free($1);
            free($2);
         }
         | TOK_TIME {
            struct tm t;
            time_t rawtime;
            time( &rawtime );
            t = *(localtime(&rawtime));
            char *ptr = strtok($1,":");
            t.tm_hour = atoi(ptr);
            ptr = strtok(NULL, ":");
            t.tm_min = atoi(ptr);
            t.tm_sec = 0;
            $$ = mktime( &t );
            free($1);
         }
         ;
%%
     
void command_parser_init(void) {
	pthread_mutex_init(&cmd_parser_lock, NULL);	
}

void command_parser_finish(void) {
	pthread_mutex_destroy(&cmd_parser_lock);
}

int commanderror(const char *msg) {
	if(cmd_parser_outfp)
		fprintf(cmd_parser_outfp, "PARSER ERROR - %s\n", msg);
	/*tmlog(TM_LOG_NOTE, "query", "PARSER ERROR - %s", msg); */
	cmd_parse_errors++;
	return 0;
}

int parse_req_parameters(const char* cmd, QueryRequest* req) {
    pthread_mutex_lock(&cmd_parser_lock);
    
    cmd_parser_outfp = NULL;
    /*cmd_parser_outfp = stdout;*/
    request = req;

    /*fprintf( stdout , "request pointer = %p\n", request);*/
	YY_BUFFER_STATE bufstate = command_scan_string(cmd);

	cmd_parse_errors=0;
	commandparse();
	command_delete_buffer(bufstate);

    /* set request parameters, e.g. save_file_name, timestamps*/
    /*request->start_time = start_time;*/
    /*request->end_time = end_time;*/
    /*if (save_file_name != NULL) {*/
        /*request->save_file_name = std::string(save_file_name);*/
    /*}*/
    /*free( save_file_name );*/

	pthread_mutex_unlock(&cmd_parser_lock);
	return cmd_parse_errors;
}

/* Don't forget to call cmd_parser_init(), before you call the parser */
OffsetsResult* parse_command(const char* cmd, FILE *outfp, Query* q, QueryRequest* req) {
	pthread_mutex_lock(&cmd_parser_lock);
	// set variable parser_storage global to conf_parser.cc
	cmd_parser_outfp = outfp;
    /*storage = s;*/
    query = q;
    request = req;

	YY_BUFFER_STATE bufstate = command_scan_string(cmd);
    /*fprintf(outfp, "save_file_name = %s\n", save_file_name);*/
    /*fprintf(outfp, "start time = %u, end time = %u\n", start_time, end_time );*/

	cmd_parse_errors=0;
	commandparse();
	fflush(outfp);
	command_delete_buffer(bufstate);

    OffsetsResult* offsets_result = result;
    result = NULL;
    /*storage = NULL;*/
    query = NULL;
    request = NULL;
	pthread_mutex_unlock(&cmd_parser_lock);
	/*return cmd_parse_errors;*/
    return offsets_result;
}
