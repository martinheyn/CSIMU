/*
 * tic_toc.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: developer
 */

#include "inc/tic_toc.h"


void tic()
{
	tictoc_stack.push(clock());
	tictoc2_stack.push(clock());
}

double toc()
{
	double elap = -1;
	elap = ((double)(clock()- tictoc_stack.top())) / CLOCKS_PER_SEC;
	tictoc_stack.pop();

	return elap;
}

double toc2()
{
	double elap = -1;
	elap = ((double)(clock()- tictoc2_stack.top())) / CLOCKS_PER_SEC;
	tictoc2_stack.pop();

	return elap;
}



