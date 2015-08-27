/*
 * RTC_decode.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: developer
 */

#include "inc/INIT.h"
#include <inc/RTC_operations.h>

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

        case 5: // SECONDS  // FIXED BCD to decimal conversion (yeah no hex-numbers this time in contrast to ADIS...)
            data_rtc[5] = (((rtc_i2c->readByte(0x00) & 0b01110000) >> 4) *10) + (rtc_i2c->readByte(0x00) & 0b00001111);
            break;
        case 4: // MINUTES
        	data_rtc[4] = (((rtc_i2c->readByte(0x01) & 0b01110000) >> 4) *10) + (rtc_i2c->readByte(0x01) & 0b00001111);
            break;
        case 3: // HOURS
        	data_rtc[3] = (((rtc_i2c->readByte(0x02) & 0b00110000) >> 4) *10) + (rtc_i2c->readByte(0x02) & 0b00001111);
        	break;
        case 2: // DATE
        	data_rtc[2] = (((rtc_i2c->readByte(0x04) & 0b00110000) >> 4) *10) + (rtc_i2c->readByte(0x04) & 0b00001111);
			break;
        case 1: // MONTH
        	data_rtc[1] = (((rtc_i2c->readByte(0x05) & 0b00010000) >> 4) *10) + (rtc_i2c->readByte(0x05) & 0b00001111);
        	break;
        case 0: // YEAR
        	data_rtc[0] = (((rtc_i2c->readByte(0x06) & 0b11110000) >> 4) *10) + (rtc_i2c->readByte(0x06) & 0b00001111);
        	break;
        }
    }


}

void rtc_extract_message(uint8_t data_rtc[], double output_rtc[]) {
	// This is really really stupid. I hate C++.

	//time_t currentTime = time(0);
	//tm* currentDate = localtime(&currentTime);

	output_rtc[0] = (double)data_rtc[0];//year;
	output_rtc[1] = (double)data_rtc[1];//month;
	output_rtc[2] = (double)data_rtc[2];//day;
	output_rtc[3] = (double)data_rtc[3];//hour;
	output_rtc[4] = (double)data_rtc[4];//minute;
	output_rtc[5] = (double)data_rtc[5];//second;
	output_rtc[6] = 0;//millisecs;

	/*output_rtc[0] = (double)currentDate->tm_year+1900;
	output_rtc[1] = (double)currentDate->tm_mon+1;
	output_rtc[2] = (double)currentDate->tm_mday;
	output_rtc[3] = (double)currentDate->tm_hour;
	output_rtc[4] = (double)currentDate->tm_min;
	output_rtc[5] = (double)currentDate->tm_sec;
	output_rtc[6] = 0; // we get to that later */
}

void rtc_16bit_commands_to_char(uint16_t rtc_commands[], uint8_t char_commands[], size_t rtc_commands_size) {

    uint i;

    for(i=0;i<rtc_commands_size;i++) {
        char_commands[2*i] = (rtc_commands[i] >> 8) & 0xFF;
        char_commands[2*i+1] = rtc_commands[i] & 0xFF;
    }

}

void rtc_display(double rtc_data[]){
	cout << "RTC: " <<  rtc_data[0] << "." << rtc_data[1] << "." << rtc_data[2] << "  " << rtc_data[3] << ":" << rtc_data[4] << ":" << rtc_data[5] << ":" << rtc_data[6] << endl;

}
