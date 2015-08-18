/*
 * CSV_operations.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: developer
 */

#include "inc/INIT.h"
#include "inc/CSV_operations.h"

csv_operations::csv_operations()
{
	count = 0;
	time_t currentTime = time(0);
	tm* currentDate = localtime(&currentTime);
	strcpy(filename, "/media/store/ADIS"); // save .csv file to SD-card
	strcat(filename, fmt("-%d-%d-%d@%d.%d.%d.csv", // The file name contains date and time
			       currentDate->tm_hour, currentDate->tm_min, currentDate->tm_sec,
			       currentDate->tm_mday, currentDate->tm_mon+1,
			       currentDate->tm_year+1900).c_str());

	csv::ofstream csvout (filename, std::ios_base::out); // Declare and open .csv file
	this->csvout.set_delimiter(DELIMITER__);
	this->csvout << "PROD_ID" << "SYSTEM_STAT" << "POWER_SUP" << "X_ACCL" << "Y_ACCL" << "Z_ACCL" << "X_GYRO" << "Y_GYRO" << "Z_GYRO" << "X_TEMP" << "Y_TEMP" << "Z_TEMP" << "YEAR" << "MONTH" << "DATE" << "HOUR" << "MINUTE" << "SECOND" << "MILLISECONDS" << "RUNNUMBER" << NEWLINE__;

}

csv_operations::~csv_operations()
{
	this->csvout.close();
}

bool csv_operations::is_open()
{
	bool isopen;
	isopen = this->csvout.is_open();
	return isopen;

}

void csv_operations::csv_write(double imu_data[], double rtc_data[])
{
	this->csvout << imu_data[0] << imu_data[1] << imu_data[2] << imu_data[3] << imu_data[4] << imu_data[5] << imu_data[6] << imu_data[7] << imu_data[8] << imu_data[9] << imu_data[10] << imu_data[11] << rtc_data[0] << rtc_data[1] << rtc_data[2] << rtc_data[3] << rtc_data[4] << rtc_data[5]  << rtc_data[6] << this->count << NEWLINE__;
	this->csvout.flush();
	this->count++;

}

