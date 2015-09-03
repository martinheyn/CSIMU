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


	// SYNCHRONISATION
	//Before we do anything here, we synchronise the system clocks (RTC & internal clock) with the NTP server by calling the service who does that (dirty hack, will only work on OATRC'15 but I am running out of time here)
	cout << "Synchronisation with network time prototocol server, this takes 40 seconds" << endl;
	//system("systemctl start rtc-ds3231.service");
	//msleep(40000);

	//Opening the common folder on the NAS server (Again Hacking it because of lack of time. THIS WILL ONLY WORK ON OATRC'15)
	cout << "Opening the network attached server shared folder, takes about 10 seconds" << endl;
	//system("systemctl start nfs-share.service");
	//msleep(10000);

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

	// CHECKING
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

	// SETTING THE START AND STOP TIME
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


	// STARTING THE PROGRAM LOOP
	cout << "Waiting for start" << endl;

	while ( !startalarm_)
	{
		startalarm_ = rtc_check_alarm (&rtc_i2c_, 0, isOpened_i2c);
		msleep(1);
	}


	cout << "Starting the record loop" << endl;

	double prevsec = 0;
	int s_frequency = 200;
	int e_frequency = 0;
	int readcount = 0;

	struct timeval tnow_;//, loopbegin_exec_, loopend_exec_;

	while ( !stopalarm_)
	{
		//gettimeofday(&loopbegin_exec_,NULL);

		// Read the raw data
		adis_read_spi(&adis_spi_, adis_commands_, adis_rawdata_, isOpened_spi); // Read raw ADIS data
		rtc_read_i2c(&rtc_i2c_, rtc_rawdata_, isOpened_i2c); // Read raw Real Time Clock data
		// READING THE REAL TIME CLOCK IS SO SLOW, TOO SLOW ==> FIX THAT NEXT WEEK.

		// Convert raw data to normal data
		adis_extract_message(adis_rawdata_, adis_data_); // Convert weird ADIS data to readable data
		rtc_extract_message(rtc_rawdata_, rtc_data_); // Convert RTC data to readable data
		//rtc_read_systemtime(&tnow_, rtc_data_);

		if (rtc_data_[5] - prevsec != 0) {
			prevsec = rtc_data_[5];
			e_frequency = s_frequency - readcount;
			cout << "The current freq ist: " << readcount << endl;
			readcount = 0;
		}

		// Display data
		//adis_display(adis_data_);
		//rtc_display(rtc_data_);

		// Send data to CSV file
		csvout->csv_write(adis_data_,rtc_data_);
		//stopalarm_ = rtc_check_alarm (&rtc_i2c_, 1, isOpened_i2c);
		stopalarm_ = rtc_check_stopalarm(stopalarmtime_,rtc_data_);
		//usleep(5000);
		//gettimeofday(&loopend_exec_,NULL);

		//cout<<"ELAPSED TIME IS: " << end_exec.tv_usec - begin_exec.tv_usec << endl;

		//usleep(20000-(loopend_exec_.tv_usec - loopbegin_exec_.tv_usec));
		readcount++;
		//msleep(1000/(s_frequency + 0.5*e_frequency));
		//msleep(1);
		usleep(1);
	}

	rtc_reset_alarm_i2c(&rtc_i2c_, isOpened_i2c); // Remove the alarm from the RTC
	adis_spi_.close(); // Close SPI connection to IMU
	rtc_i2c_.close(); // Close I2C connection to RTC
	system("mv /media/store/*.csv /media/imu_data/"); // Move all .csv files (also the ones forgotten earlier) to the NAS server (AGAIN HACK WORKS ONLY ON OATRC'15)
	return 0;

}
