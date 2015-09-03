/*
 * CSV_operations.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: developer
 */

#include "inc/INIT.h"
#include "inc/CSV_operations.h"

csv_operations::csv_operations(string const & FileName) // initiates a csv-file (Constructor)
	: csvfile(FileName.c_str(),std::ios_base::out)
{
	//filename = csv_operations::createfilename();
	count = 0;
	this->csvfile.set_delimiter(DELIMITER__);
	this->csvfile << "PROD_ID" << "SYSTEM_STAT" << "POWER_SUP" << "X_ACCL" << "Y_ACCL" << "Z_ACCL" << "X_GYRO" << "Y_GYRO" << "Z_GYRO" << "X_TEMP" << "Y_TEMP" << "Z_TEMP" << "YEAR" << "MONTH" << "DATE" << "HOUR" << "MINUTE" << "SECOND" << "SAMPLES_IN_THIS_SEC" << "RUNNUMBER" << NEWLINE__;
	this->csvfile.flush();
}

csv_operations::~csv_operations() // Destructor
{
	this->csvfile.close();
}

bool csv_operations::is_open()
{
	bool isopen;
	isopen = this->csvfile.is_open();
	return isopen;

}

void csv_operations::csv_write(int readcount, double imu_data[], double rtc_data[])
{
	this->csvfile << imu_data[0] << imu_data[1] << imu_data[2] << imu_data[3] << imu_data[4] << imu_data[5] << imu_data[6] << imu_data[7] << imu_data[8] << imu_data[9] << imu_data[10] << imu_data[11] << rtc_data[0] << rtc_data[1] << rtc_data[2] << rtc_data[3] << rtc_data[4] << rtc_data[5]  << readcount << this->count << NEWLINE__;
	count++;

}

std::string createfilename()
{
	ostringstream newfilename;
	time_t currentTime = time(0);
		tm* currentDate = localtime(&currentTime);
		newfilename << "/media/store/ADIS16364_" << currentDate->tm_year+1900 << currentDate->tm_mon+1 << currentDate->tm_mday << "_" <<currentDate->tm_hour << currentDate->tm_min << currentDate->tm_sec << ".csv";
		return newfilename.str();

}
