/*
 * ADIS_16364.cpp
 *
 *  Created on: Jul 30, 2015
 *      Author: developer
 */


#include "inc/ADIS_16364_V2.h"
#include "inc/minicsv.h"

#include <iostream>
#include <unistd.h>
#include <math.h>
#include <stdio.h>

uint16_t adis_dataread_cmds_[] = {
    PROD_ID,
	DIAG_STAT,
	POW_SUP,

	X_ACCL_OUT,
	Y_ACCL_OUT,
	Z_ACCL_OUT,

	X_GYRO_OUT,
    Y_GYRO_OUT,
    Z_GYRO_OUT,

    X_TEMP_OUT,
    Y_TEMP_OUT,
    Z_TEMP_OUT,

    PROD_ID // each entry requires 2 x 8 bit (1 byte) => length of data buffer must be 11 x 2 byte = 22!!!!!
};

uint16_t rtc_dataread_cmds_[] = {
		SECONDS,
		MINUTES,
		HOURS,
		DATE,
		MONTH,
		YEAR
};

// THIS IS FOR ADIS16364

double accl_res_conv_ = 1;
double gyro_res_conv_ = 0.05;
//double std_gravity_ = 9.80665;
//double millig_to_accl_ = std_gravity_/1000;
double temp_res_conv_ = 0.136;
double temp_zero_ = 25.0;
double pow_res_conv_ = 0.002418;

uint8_t commands_[ADIS_BUFFER_SIZE__]; // length of data buffer must be 11 entries x 2 byte = 22!!!!!
uint8_t data_[ADIS_BUFFER_SIZE__]; //length of data buffer must be 11 entries x 2 byte = 22!!!!!
uint8_t rtc_commands_[RTC_BUFFER_SIZE__];
uint8_t data_rtc_[RTC_NUMREGISTERS__];
double output_ [ADIS_NUMREGISTERS__];
double output_rtc_ [RTC_NUMREGISTERS__ + 1]; // one extra for milliseconds
uint16_t milliseconds = 0;
uint16_t timestep = TIMESTEP__; // Timestep in milliseconds
uint16_t sleepstep = timestep * 1000; //microseconds
double differtime = 0;
uint16_t sleepstepcorr = 0;
bool isOpened_spi;
bool isOpened_i2c;

std::stack<clock_t> tictoc_stack;
std::stack<clock_t> tictoc2_stack;

using namespace std;

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

void adis_16bit_commands_to_char(uint16_t adis_commands[], uint8_t char_commands[], size_t adis_commands_size) {

    uint i;

    for(i=0;i<adis_commands_size;i++) {
        char_commands[2*i] = (adis_commands[i] >> 8) & 0xFF;
        char_commands[2*i+1] = adis_commands[i] & 0xFF;
    }

}

uint16_t adis_14bit_to_16bit(uint16_t adis_14bit)
{
	uint16_t adis_16bit = 0;
	adis_14bit = adis_14bit & 0x3fff; // only the first 14 bits are relevant: 0x3fff = 0011 1111 1111 1111
	adis_16bit = adis_14bit | ((adis_14bit & 0b0010000000000000) << 2) | ((adis_14bit & 0b0010000000000000) << 1);
	// Fixing the negatives numbers. The MSB is the sign of the number. The IMU only has 14bit, an integer has 16bit...we have to shift the 1 in case of negatives numbers by two the left.
	return adis_16bit;
}

uint16_t adis_12bit_to_16bit(uint16_t adis_12bit)
{
	uint16_t adis_16bit = 0;
	adis_12bit = adis_12bit & 0xfff; // only the first 12 bits are relevant: 0xfff = 0000 1111 1111 1111
	adis_16bit = adis_12bit | ((adis_12bit & 0b0000100000000000) << 4) | ((adis_12bit & 0b0000100000000000) << 3) | ((adis_12bit & 0b0000100000000000) << 2) | ((adis_12bit & 0b0000100000000000) << 1);
	// Fixing the negatives numbers.
	return adis_16bit;
}

//void extract_adis_message(uint8_t* data, dp::imuinterface::proto::ImuSensorAdis* msg) {
void extract_adis_message(uint8_t* data, double* output) {

	int16_t prod_id = 0;
	int16_t diag = 0;
	int16_t power_int = 0;
	int16_t accl_x_int, accl_y_int, accl_z_int = 0;
	int16_t gyro_x_int, gyro_y_int, gyro_z_int = 0;
	int16_t temp_x_int, temp_y_int, temp_z_int = 0;

	//double temp_x, temp_y, temp_z, gyro_x, gyro_y, gyro_z, accl_x, accl_y, accl_z;

	prod_id = data[3]; // the first 16 bits are random numbers!!!! AAAAAAAAAAAAAAAAH JUST WHO DOES THAT???
	prod_id = prod_id | (data[2] << 8);  // And to make things easy you have to read the second set of numbers first...
	output[0] = prod_id;

	diag = data[5];
	diag = diag | (data[4] << 8);
	output[1] = diag;

	power_int = data[7];
	power_int = power_int | (data[6] << 8);
	power_int = adis_12bit_to_16bit(power_int);
	output[2] = power_int * pow_res_conv_;

	accl_x_int = data[9];
	accl_x_int = accl_x_int | (data[8] << 8);
	accl_x_int = adis_14bit_to_16bit(accl_x_int); // Converting from 14 to 16 bit
	//accl_x = accl_x_int * accl_res_conv_;
	output[3] = accl_x_int * accl_res_conv_;

	accl_y_int = data[11];
	accl_y_int = accl_y_int | (data[10] << 8);
	accl_y_int = adis_14bit_to_16bit(accl_y_int); // Converting from 14 to 16 bit
	//accl_y = accl_y_int * accl_res_conv_;
	output[4] = accl_y_int * accl_res_conv_;


	accl_z_int = data[13];
	accl_z_int = accl_z_int | (data[12] << 8);
	accl_z_int = adis_14bit_to_16bit(accl_z_int); // Converting from 14 to 16 bit
	//accl_z = accl_z_int * accl_res_conv_;
	output[5] = accl_z_int * accl_res_conv_;

	gyro_x_int = data[15];
	gyro_x_int = gyro_x_int | (data[14] << 8);
	gyro_x_int = adis_14bit_to_16bit(gyro_x_int); // Converting from 14 to 16 bit
	//gyro_x = gyro_x_int * gyro_res_conv_;
	output[6] = gyro_x_int * gyro_res_conv_;

	gyro_y_int = data[17];
	gyro_y_int = gyro_y_int | (data[16] << 8);
	gyro_y_int = adis_14bit_to_16bit(gyro_y_int); // Converting from 14 to 16 bit
	//gyro_y = gyro_y_int * gyro_res_conv_;
	output[7] = gyro_y_int * gyro_res_conv_;

	gyro_z_int = data[19];
	gyro_z_int = gyro_z_int | (data[18] << 8);
	gyro_z_int = adis_14bit_to_16bit(gyro_z_int); // Converting from 14 to 16 bit
	//gyro_z = gyro_z_int * gyro_res_conv_;
	output[8] = gyro_z_int * gyro_res_conv_;

	temp_x_int = data[21];
	temp_x_int = temp_x_int | (data[20] << 8);
	temp_x_int = adis_12bit_to_16bit(temp_x_int); // Converting from 12 to 16 bit
	//temp_x = temp_zero_ + (temp_x_int * temp_res_conv_);
	output[9] = temp_zero_ + (temp_x_int * temp_res_conv_);

	temp_y_int = data[23];
	temp_y_int = temp_y_int | (data[22] << 8);
	temp_y_int = adis_12bit_to_16bit(temp_y_int); // Converting from 12 to 16 bit
	//temp_y = temp_zero_ + (temp_y_int * temp_res_conv_);
	output[10] = temp_zero_ + (temp_y_int * temp_res_conv_);

	temp_z_int = data[25];
	temp_z_int = temp_z_int | (data[24] << 8);
	temp_z_int = adis_12bit_to_16bit(temp_z_int); // Converting from 12 to 16 bit
	//temp_z = temp_zero_ + (temp_z_int * temp_res_conv_);
	output[11] = temp_zero_ + (temp_z_int * temp_res_conv_);

}

void convert_rtc_message(uint8_t* data_rtc, int millisecs, double* output_rtc) {
	// This is really really stupid. I hate C++.

	time_t currentTime = time(0);
	tm* currentDate = localtime(&currentTime);

	int8_t year = 0;
	int8_t month = 0;
	int8_t day = 0;
	int8_t hour = 0;
	int8_t minute = 0;
	int8_t second = 0;

	year = data_rtc[0];
	month = data_rtc[1];
	day = data_rtc[2];
	hour = data_rtc[3];
	minute = data_rtc[4];
	second = data_rtc[5];

/*	output_rtc[0] = year;
	output_rtc[1] = month;
	output_rtc[2] = day;
	output_rtc[3] = hour;
	output_rtc[4] = minute;
	output_rtc[5] = second;
	output_rtc[6] = millisecs; */

	output_rtc[0] = (double)currentDate->tm_year+1900;
	output_rtc[1] = (double)currentDate->tm_mon+1;
	output_rtc[2] = (double)currentDate->tm_mday;
	output_rtc[3] = (double)currentDate->tm_hour;
	output_rtc[4] = (double)currentDate->tm_min;
	output_rtc[5] = (double)currentDate->tm_sec;
	output_rtc[6] = millisecs;
}

//void adis_read_spi(uint32_t adis_number) {
void adis_read_spi(BlackLib::BlackSPI* adis_spi, uint8_t* commands, uint8_t* data, bool isOpened_spi){

	//isOpened_spi = adis_spi->open(BlackLib::NonBlock);

	if ( !isOpened_spi)
	{
		cout << "I cannot open the SPI device. Help!" << endl;
		exit(1);
	}
	/*else
	{
	    cout << "Device Path   : " << adis_spi->getPortName() << endl;
	    cout << "Max Speed(Hz) : " << adis_spi->getMaximumSpeed() << endl;
	    cout << "Bits Per Word : " << (int)adis_spi->getBitsPerWord() << endl;
	    cout << "Mode          : " << (int)adis_spi->getMode() << endl;
	} */

	adis_spi->transfer(commands, data, ADIS_BUFFER_SIZE__, 0);

}

void rtc_read_i2c(BlackLib::BlackI2C* rtc_i2c, uint8_t* data_rtc, bool isOpened_i2c )
{


	if ( !isOpened_i2c)
	{
		cout << "I cannot open the I2C device!" << endl;
		exit(1);
	}

    for(int i=0;i<=RTC_BUFFER_SIZE__;i++) {
        switch (i)
        {
        case 5: // SECONDS
            data_rtc[5] = (rtc_i2c->readByte(0x00) & 0b01111111);
            break;
        case 4: // MINUTES
            data_rtc[4] = (rtc_i2c->readByte(0x01) & 0b01111111);
            break;
        case 3: // HOURS
        	data_rtc[3] = (rtc_i2c->readByte(0x02) & 0b00011111);
        	break;
        case 2: // DATE
			data_rtc[2] = (rtc_i2c->readByte(0x04) & 0b00011111);
			break;
        case 1: // MONTH
        	data_rtc[1] = (rtc_i2c->readByte(0x05) & 0b00011111);
        	break;
        case 0: // YEAR
        	data_rtc[0] = (rtc_i2c->readByte(0x06) & 0b11111111);
        	break;
        }
    }


}

void signed_sealed_delivered(double* sensordata, double* rtcdata, int count, csv::ofstream* file)
{
	bool isOpened_csv = file->is_open();

	if ( !isOpened_csv)
	{
		cout << "I cannot open the CSV file. Help!" << endl;
		exit(1);
	}
	else
	{
	    *file << sensordata[0] << sensordata[1] << sensordata[2] << sensordata[3] << sensordata[4] << sensordata[5] << sensordata[6] << sensordata[7] << sensordata[8] << sensordata[9] << sensordata[10] << sensordata[11] << rtcdata[0] << rtcdata[1] << rtcdata[2] << rtcdata[3] << rtcdata[4] << rtcdata[5]  << rtcdata[6] << count << NEWLINE__;
	}

}

void leds_on_off(bool onoff)
{
	std::fstream fs0;
	std::fstream fs1;
	std::fstream fs2;
	std::fstream fs3;

	fs0.open(LED0_PATH "/brightness", std::fstream::out);
	fs1.open(LED1_PATH "/brightness", std::fstream::out);
	fs2.open(LED2_PATH "/brightness", std::fstream::out);
	fs3.open(LED3_PATH "/brightness", std::fstream::out);

	if (onoff == true)
	{
		fs0 << "1";
		fs1 << "1";
		fs2 << "1";
		fs3 << "1";
		fs0.close();
		fs1.close();
		fs2.close();
		fs3.close();
	}
	else
	{
		fs1 << "0";
		fs2 << "0";
		fs3 << "0";
		fs1.close();
		fs2.close();
		fs3.close();
	}


}

void led2_on_off(bool onoff)
{
	std::fstream fs2;

	fs2.open(LED3_PATH "/brightness", std::fstream::out);

	if (onoff == true)
	{
		fs2 << "1";
		fs2.close();
	}
	else
	{
		fs2 << "0";
		fs2.close();
	}


}

void led3_on_off(bool onoff)
{
	std::fstream fs3;

	fs3.open(LED3_PATH "/brightness", std::fstream::out);

	if (onoff == true)
	{
		fs3 << "1";
		fs3.close();
	}
	else
	{
		fs3 << "0";
		fs3.close();
	}
}

std::string fmt(const std::string& fmt, ...) {
    int size = 200;
    std::string str;
    va_list ap;
    while (1) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char*)str.c_str(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
    return str;
}


int main()
{
	cout << "Starting the ADIS test program" << endl;
	long count = 1;
	long secsave = 0;
	char filename[256] = {0};

	leds_on_off(true);

	// The upcoming section has only one purpose: Create a unique filename for the .csv data
	time_t currentTime = time(0);
	tm* currentDate = localtime(&currentTime);
	strcpy(filename, "/media/store/ADIS"); // save .csv file to SD-card
	strcat(filename, fmt("-%d-%d-%d@%d.%d.%d.csv", // The file name contains date and time
		       currentDate->tm_hour, currentDate->tm_min, currentDate->tm_sec,
		       currentDate->tm_mday, currentDate->tm_mon+1,
		       currentDate->tm_year+1900).c_str());


	csv::ofstream csvout (filename, std::ios_base::out); // Declare and open .csv file
	csvout.set_delimiter(DELIMITER__);
	csvout << "PROD_ID" << "SYSTEM_STAT" << "POWER_SUP" << "X_ACCL" << "Y_ACCL" << "Z_ACCL" << "X_GYRO" << "Y_GYRO" << "Z_GYRO" << "X_TEMP" << "Y_TEMP" << "Z_TEMP" << "YEAR" << "MONTH" << "DATE" << "HOUR" << "MINUTE" << "SECOND" << "MILLISECONDS" << "RUNNUMBER" << NEWLINE__;

	BlackLib::BlackSPI adis_spi_(BlackLib::SPI0_0, 8, BlackLib::SpiMode3, 300000); // Declare IMU device and open SPI port
	BlackLib::BlackI2C rtc_i2c_ (BlackLib::I2C_1, 0x68); // Declare RTC and open I2C port

	isOpened_spi = adis_spi_.open(BlackLib::NonBlock);
	isOpened_i2c = rtc_i2c_.open(BlackLib::NonBlock);

	adis_16bit_commands_to_char(adis_dataread_cmds_, commands_, 13); // Convert commands to weird ADIS style
	adis_16bit_commands_to_char(rtc_dataread_cmds_, rtc_commands_, 6);

	/*while (data_rtc_[6] != 0)
	{
		rtc_read_i2c(&rtc_i2c_, data_rtc_, isOpened_i2c); // Read Real Time Clock data
		convert_rtc_message(data_rtc_,milliseconds,output_rtc_);
		usleep(1); // And wait until the full second arrives
	}*/

	//tic(); // Start counting milliseconds

	while (true)
	{
		adis_read_spi(&adis_spi_, commands_, data_, isOpened_spi); // Read the ADIS data

		/*if (output_rtc_[5] - 1  >= (secsave)) // Set Milliseconds to zero
			{
			milliseconds = 0;
			secsave = output_rtc_[5]; // save the previous second;
			} */

		rtc_read_i2c(&rtc_i2c_, data_rtc_, isOpened_i2c); // Read Real Time Clock data


		extract_adis_message(data_, output_); // Convert weird ADIS data to readable data
		convert_rtc_message(data_rtc_, milliseconds, output_rtc_);

		//cout << "RTC: " <<  output_rtc_[0] << "." << output_rtc_[1] << "." << output_rtc_[2] << "  " << output_rtc_[3] << ":" << output_rtc_[4] << ":" << output_rtc_[5] << ":" << output_rtc_[6] << endl;
		//cout << "RTC: " << std::hex << (int)who_am_I << std::dec << endl;

		//cout << " prod_id: " << output_[0] << endl;
		//cout << " system_stat: " << output_[1] << endl;
		//cout << " power_sup: " << output_[2] << endl;
		//cout << " accl_x: " << output_[3] << endl;
		//cout << " accl_y: " << output_[4] << endl;
		//cout << " accl_z: " << output_[5] << endl;
		//cout << " gyro_x: " << output_[6] << endl;
		//cout << " gyro_y: " << output_[7] << endl;
		//cout << " gyro_z: " << output_[8] << endl;
		//cout << " temp_x: " << output_[9] << endl;
		//cout << " temp_y: " << output_[10] << endl;
		//cout << " temp_z: " << output_[11] << endl;
		//cout << endl;


		signed_sealed_delivered(output_, output_rtc_, count, &csvout);

		count++;
		//cout << "Run: " << count << endl;

		//differtime = toc(); // How long took the operation?
		//sleepstepcorr = sleepstep - (differtime * 1000 * 1000); // Correct sleeping time
		csvout.flush(); // write .csv file
		usleep(5000);
		//differtime = toc2(); // How long took it? Stop second timer
		//cout << "It took " << differtime << " seconds" << endl;
		//milliseconds = milliseconds + (differtime * 1000); // Add that to the millisecond counter
		//tic();
	}

	adis_spi_.close(); // Close SPI connection to IMU
	rtc_i2c_.close(); // Close I2C connection to RTC
	return 0;

}
