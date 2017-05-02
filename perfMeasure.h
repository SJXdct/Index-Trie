/*==========================================================================
#
# Author: hetaihua - hetaihua@ict.ac.cn
#
# Last modified: 2012-12-27 15:51
#
# Filename: perfMeasure.h
#
# Description: this is a module for performance measurement. 
#			You can use it to aggregate the average exection time of some module.
#			Also it can be used to calc the average length of hash-bucket collision chain.
===========================================================================*/

#ifndef _PERFMEASURE_H
#define	_PERFMEASURE_H

#include <stack>
#include <stdint.h>
#include <string>
#include <sys/time.h>
using std::string;
using std::stack;

class PerfMeasure {
	private:
		uint64_t		stat_number;
		double			max;
		double			min;
		double			total;
		string			perfName;
		stack<double>	  records;

	public:
		PerfMeasure(){
		}
		PerfMeasure(const string&  name);

		~PerfMeasure(){
		}

		inline void reset() {
			stat_number = 0;
			total = 0;
		}

		inline void addRecord( double record ) {
			max = std::max( max, record );
			min = std::min( min, record );
			++stat_number;
			total += record;
		}

		inline double getStat() {
			return total/stat_number;
		}

		inline double getMin() {
			return min;
		}

		inline double getMax() {
			return max;
		}

		inline double getTotal(){
			return total;
		}

		// exection time statistic
		// call this function before tested module running each time
		// PerfMeasure.start()
		// module xxx runing
		// PerfMeasure.end()
		void start();

		void end();
		
		void print_stat(); 
};

#endif


