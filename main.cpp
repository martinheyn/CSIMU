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
	csv_operations csvfile;

	// Check that everything went well
	if (isOpened_spi)
		cout << "IMU is responding" << endl;
	else
		cout << "Warning, IMU is not responding" << endl;

	if (isOpened_i2c)
		cout << "RTC is responding" << endl;
	else
		cout << "Warning, RTC is not responding" << endl;

	if (csvfile.is_open())
		cout << "CSV file is ready" << endl;
	else
		cout << "Warning, CSV file is not open" << endl;

	// Start actual program

	cout << "Starting the record loop" << endl;
	while (true)
	{
		// Read the raw data
		adis_read_spi(&adis_spi_, adis_commands_, adis_rawdata_, isOpened_spi); // Read raw ADIS data
		rtc_read_i2c(&rtc_i2c_, rtc_rawdata_, isOpened_i2c); // Read raw Real Time Clock data

		// Convert raw data to normal data
		adis_extract_message(adis_rawdata_, adis_data_); // Convert weird ADIS data to readable data
		rtc_extract_message(rtc_rawdata_, rtc_data_); // Convert RTC data to readable data

		// Display data
		adis_display(adis_data_);
		rtc_display(rtc_data_);

		// Send data to CSV file
		csvfile.csv_write(adis_data_,rtc_data_);

		usleep(5000);
	}

	adis_spi_.close(); // Close SPI connection to IMU
	rtc_i2c_.close(); // Close I2C connection to RTC
	return 0;

}
