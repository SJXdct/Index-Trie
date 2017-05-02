/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified: 2012-12-27 16:16
#
# Filename: perfMeasure.cpp
#
# Description: 
#
===========================================================================*/
#include "perfMeasure.h"
#include "global.h"
#include <cstdio>
#include <time.h>

PerfMeasure::PerfMeasure( const string& name ) :\
		perfName(name), stat_number(0),total(0),max(0){
		}


void PerfMeasure::start(){
	timeval	now;
	gettimeofday( &now, NULL );
	records.push( now.tv_sec * 1000000 + now.tv_usec );
}

void PerfMeasure::end() {
	if( records.empty() ) {
		ERROR_INFO("records stack is empty!",return );
	}
	timeval	now;
	gettimeofday( &now, NULL );
	this->addRecord(now.tv_sec * 1000000 + now.tv_usec - records.top() );
	records.pop();
}


void PerfMeasure::print_stat() {
	printf("Performance of %s: Average=%.6lf(s), max=%.6lf, min=%.6lf, total=%.6lf\n",\
			perfName.c_str(), this->getStat(), max, min, total );
}
