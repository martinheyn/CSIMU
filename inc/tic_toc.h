/*
 * tic_toc.h
 *
 *  Created on: Aug 18, 2015
 *      Author: developer
 */

#ifndef INC_TIC_TOC_H_
#define INC_TIC_TOC_H_

#include "inc/INIT.h"
using namespace std;

std::stack<clock_t> tictoc_stack;
std::stack<clock_t> tictoc2_stack;

// Forward declarations
void tic();
double toc();
double toc2();

#endif /* INC_TIC_TOC_H_ */
