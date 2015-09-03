/*
 * CSV_operations.h
 *
 *  Created on: Aug 18, 2015
 *      Author: developer
 */

#ifndef INC_CSV_OPERATIONS_H_
#define INC_CSV_OPERATIONS_H_

#define NEWLINE__ '\n'
#define DELIMITER__ ';'

#include "inc/INIT.h"
#include "inc/minicsv.h"


class csv_operations
{
  public:
    csv_operations(string const & FileName);                  // der Default-Konstruktor
    ~csv_operations();                     // der Destruktor



    bool is_open();
    void csv_write(int readcount, double imu_data[], double rtc_data[]);        // eine Funktion mit einem (Default-) Parameter

  private:
	std::string filename;
	long count;
	csv::ofstream csvfile; // Declare and open .csv file

};

std::string createfilename();

// Forward declaration of functions
#endif /* INC_CSV_OPERATIONS_H_ */
