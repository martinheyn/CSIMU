/*
 * main.cpp
 *
 *  Created on: Jul 30, 2015
 *      Author: developer
 */

#include "inc/INIT.h"
#include "inc/ADIS_operations.h"
#include "inc/CSV_operations.h"
#include "inc/RTC_operations.h"

// Initialise output buffers

int main()
{
	cout << "Starting the ADIS test program" << endl;

	cout << "Initalisation in progress" << endl;

	//Opening and declaring the SPI port for the IMU
	double adis_data_ [ADIS_NUMREGISTERS__];
	bool isOpened_spi;

	uint8_t adis_commands_[ADIS_BUFFER_SIZE__]; // length of data buffer must be 11 entries x 2 byte = 22!!!!!
	uint8_t adis_rawdata_[ADIS_BUFFER_SIZE__]; //length of data buffer must be 11 entries x 2 byte = 22!!!!!
	adis_init(adis_commands_);

	BlackLib::BlackSPI adis_spi_(BlackLib::SPI0_0, 8, BlackLib::SpiMode3, 300000); // Declare IMU device and open SPI port
	isOpened_spi = adis_spi_.open(BlackLib::NonBlock);

	//Opening and declaring the I2C port for the RTC
	double rtc_data_ [RTC_NUMREGISTERS__ + 1]; // one extra for milliseconds
	bool isOpened_i2c;

	uint8_t rtc_rawdata_[RTC_NUMREGISTERS__];

	BlackLib::BlackI2C rtc_i2c_ (BlackLib::I2C_1, 0x68); // Declare RTC and open I2C port
	isOpened_i2c = rtc_i2c_.open(BlackLib::NonBlock);

	//Opening and declaring the csv file
	csv_operations *csvout = new csv_operations(createfilename());

	// Check that everything went well
	if (isOpened_spi)
		cout << "IMU is responding" << endl;
	else
		cout << "Warning, IMU is not responding" << endl;

	if (isOpened_i2c)
		cout << "RTC is responding" << endl;
	else
		cout << "Warning, RTC is not responding" << endl;

	if (csvout->is_open())
		cout << "CSV file is ready" << endl;
	else
		cout << "Warning, CSV file is not open" << endl;

	// Reading and setting the alarm times
	int startalarmtime_[4];
	int stopalarmtime_[4];
	bool startalarm_ = 0;
	bool stopalarm_ = 0;
	double controlalarm_[4];

	file_get_startstop(startalarmtime_,stopalarmtime_, ALARMFILE__);

	cout << "I have read the following start-times: " << startalarmtime_[0] << "--" << startalarmtime_[1] << ":" << startalarmtime_[2] << ":" << startalarmtime_[3] << endl;
	cout << "I have read the following stop-times: " << stopalarmtime_[0] << "--" << stopalarmtime_[1] << ":" << stopalarmtime_[2] << ":" << stopalarmtime_[3] << endl;

	rtc_reset_alarm_i2c(&rtc_i2c_, isOpened_i2c);
	rtc_set_alarm_i2c(&rtc_i2c_, startalarmtime_,0, isOpened_i2c); // set first alarm clock
	rtc_set_alarm_i2c(&rtc_i2c_, stopalarmtime_,1, isOpened_i2c); // set second alarm clock
	rtc_read_alarm_i2c(&rtc_i2c_, controlalarm_,isOpened_i2c);
	cout << "I have set the following alarm: " << controlalarm_[0] << "--" << controlalarm_[1] << ":" << controlalarm_[2] << ":" << controlalarm_[3] << endl;


	// Start actual program

	cout << "Waiting for start" << endl;

	while ( !startalarm_)
	{
		startalarm_ = rtc_check_alarm (&rtc_i2c_, 0, isOpened_i2c);
		msleep(10);
	}


	cout << "Starting the record loop" << endl;

	int tempcount = 1;
	while ( !stopalarm_)
	{
		// Read the raw data
		adis_read_spi(&adis_spi_, adis_commands_, adis_rawdata_, isOpened_spi); // Read raw ADIS data
		//rtc_read_i2c(&rtc_i2c_, rtc_rawdata_, isOpened_i2c); // Read raw Real Time Clock data
		// READING THE REAL TIME CLOCK IS SO SLOW, TOO SLOW ==> FIX THAT NEXT WEEK.

		// Convert raw data to normal data
		adis_extract_message(adis_rawdata_, adis_data_); // Convert weird ADIS data to readable data
		//rtc_extract_message(rtc_rawdata_, rtc_data_); // Convert RTC data to readable data

		// Display data
		//adis_display(adis_data_);
		//rtc_display(rtc_data_);

		// Send data to CSV file
		csvout->csv_write(adis_data_,rtc_data_);
		stopalarm_ = rtc_check_alarm (&rtc_i2c_, 1, isOpened_i2c);
		tempcount++;
		//usleep(5000);
		msleep(0.1);
	}

	rtc_reset_alarm_i2c(&rtc_i2c_, isOpened_i2c);
	adis_spi_.close(); // Close SPI connection to IMU
	rtc_i2c_.close(); // Close I2C connection to RTC
	cout << "I managed to write: " << tempcount << " lines." << endl;
	return 0;

}
