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

   // for(int i=3;i<=RTC_BUFFER_SIZE__;i++) {
      //  switch (i)
       // {

     //   case 5: // SECONDS  // FIXED BCD to decimal conversion (yeah no hex-numbers this time in contrast to ADIS...)
            //data_rtc[5] = (((rtc_i2c->readByte(0x00) & 0b01110000) >> 4) *10) + (rtc_i2c->readByte(0x00) & 0b00001111);
        	data_rtc[5] = rtc_i2c->readByte(0x00);
        	data_rtc[5] = (((data_rtc[5] & 0b01110000) >> 4) *10) + (data_rtc[5] & 0b00001111);
       // 	break;
      //  case 4: // MINUTES
        //	data_rtc[4] = rtc_i2c->readByte(0x01);
        	//data_rtc[4] = (((data_rtc[4] & 0b01110000) >> 4) *10) + (data_rtc[4] & 0b00001111);
           // break;
       // case 3: // HOURS
        	//data_rtc[3] = rtc_i2c->readByte(0x02);
        	//data_rtc[3] = (((data_rtc[3] & 0b00110000) >> 4) *10) + (data_rtc[3] & 0b00001111);
        	//break;

        	data_rtc[2] = 0;
        	data_rtc[1] = 0;
        	data_rtc[0] = 0;
      /*case 2: // DATE
        	data_rtc[2] = (((rtc_i2c->readByte(0x04) & 0b00110000) >> 4) *10) + (rtc_i2c->readByte(0x04) & 0b00001111);
			break;
        case 1: // MONTH
        	data_rtc[1] = (((rtc_i2c->readByte(0x05) & 0b00010000) >> 4) *10) + (rtc_i2c->readByte(0x05) & 0b00001111);
        	break;
        case 0: // YEAR
        	data_rtc[0] = (((rtc_i2c->readByte(0x06) & 0b11110000) >> 4) *10) + (rtc_i2c->readByte(0x06) & 0b00001111);
        	break;*/
        //}
 //   }


}

void rtc_read_alarm_i2c(BlackLib::BlackI2C* rtc_i2c, double* data_rtc, bool isOpened_i2c )
{
	uint8_t raw_date[4];

	if ( !isOpened_i2c)
	{
		cout << "I cannot open the I2C device!" << endl;
		exit(1);
	}

	for(int i=0;i<=3;i++) {
        switch (i)
        {

        case 3: // SECONDS  // FIXED BCD to decimal conversion (yeah no hex-numbers this time in contrast to ADIS...)
            raw_date[3] = (((rtc_i2c->readByte(0x07) & 0b01110000) >> 4) *10) + (rtc_i2c->readByte(0x07) & 0b00001111);
            data_rtc[3] = (double)raw_date[3];
            break;
        case 2: // MINUTES
        	raw_date[2] = (((rtc_i2c->readByte(0x08) & 0b01110000) >> 4) *10) + (rtc_i2c->readByte(0x08) & 0b00001111);
        	data_rtc[2] = (double)raw_date[2];
        	break;
        case 1: // HOURS
        	raw_date[1] = (((rtc_i2c->readByte(0x09) & 0b00110000) >> 4) *10) + (rtc_i2c->readByte(0x09) & 0b00001111);
        	data_rtc[1] = (double)raw_date[1];
        	break;
        case 0: // DATE
        	raw_date[0] = (((rtc_i2c->readByte(0x0A) & 0b00110000) >> 4) *10) + (rtc_i2c->readByte(0x0A) & 0b00001111);
        	data_rtc[0] = (double)raw_date[0];
        	break;
        }
    }


}

void rtc_set_alarm_i2c(BlackLib::BlackI2C* rtc_i2c, int* data, bool second_alarm, bool isOpened_i2c )
{


	if ( !isOpened_i2c)
	{
		cout << "I cannot open the I2C device!" << endl;
		exit(1);
	}

    for(int i=0;i<=3;i++) {
        switch (i)
        {

        case 3: // SECONDS  // FIXED BCD to decimal conversion (yeah no hex-numbers this time in contrast to ADIS...)
        	if (second_alarm) {
        	// nothing to do because second alarm cannot see seconds
        	}

        	//	return(((decimalByte /10) << 4 | (decimalByte % 10)));
        	else {
            rtc_i2c->writeByte(0x07,(((data[3] / 10) << 4 | (data[3] % 10))));
        	}
            break;
        case 2: // MINUTES
        	if (second_alarm) {
        	rtc_i2c->writeByte(0x0B,(((data[2] / 10) << 4 | (data[2] % 10))));
        	}
        	else {
        	rtc_i2c->writeByte(0x08,(((data[2] / 10) << 4 | (data[2] % 10))));
        	}
            break;
        case 1: // HOURS
        	if (second_alarm) {
        	rtc_i2c->writeByte(0x0C,(((data[1] / 10) << 4 | (data[1] % 10))));
        	}
        	else {
        	rtc_i2c->writeByte(0x09,(((data[1] / 10) << 4 | (data[1] % 10))));
        	}
        	break;
        case 0: // DATE
        	if (second_alarm) {
        	rtc_i2c->writeByte(0x0D,(((data[0] / 10) << 4 | (data[0] % 10))));
        	}
        	else {
        	rtc_i2c->writeByte(0x0A,(((data[0] / 10) << 4 | (data[0] % 10))));
        	}
        	break;
        }
    }


}

bool rtc_check_alarm (BlackLib::BlackI2C* rtc_i2c, bool second_alarm, bool isOpened_i2c ) {
	bool alarm_signal;
		if (second_alarm) {
			 alarm_signal = (rtc_i2c->readByte(0x0F) & 0b00000010);
		}
		else {
			alarm_signal = (rtc_i2c->readByte(0x0F) & 0b00000001);
		}
	return alarm_signal;

}

bool rtc_check_stopalarm (int stoptime[], double output_rtc[]) {
	bool stop_signal;

	if ((stoptime[0] == output_rtc[2]) && (stoptime[1] == output_rtc[3]) && (stoptime[2] == output_rtc[4]) && (stoptime[3] == output_rtc[5]))
		stop_signal = true;
	else
		stop_signal = false;

	return stop_signal;
}

void file_get_startstop (int* starttime, int* stoptime, std::string filelocation) {
	ifstream datefile;
	datefile.open(filelocation);

	datefile >> starttime[0] >> starttime[1] >> starttime[2] >> starttime[3] >> stoptime[0] >> stoptime[1] >> stoptime[2] >> stoptime[3];

	datefile.close();

}

void rtc_extract_message(uint8_t data_rtc[], double output_rtc[]) {
	// This is really really stupid. I hate C++.


	time_t currentTime = time(0);
	tm* currentDate = localtime(&currentTime);

	output_rtc[0] = (double)currentDate->tm_year+1900;
	output_rtc[1] = (double)currentDate->tm_mon+1;
	output_rtc[2] = (double)currentDate->tm_mday;
	//output_rtc[0] = (double)data_rtc[0];//year;
	//output_rtc[1] = (double)data_rtc[1];//month;
	//output_rtc[2] = (double)data_rtc[2];//day;
	//output_rtc[3] = (double)data_rtc[3];//hour;
	//output_rtc[4] = (double)data_rtc[4];//minute;
	output_rtc[5] = (double)data_rtc[5];//second;
	output_rtc[6] = 0;

}

void rtc_read_systemtime(timeval* tnow, double output_rtc[]) {

	time_t currentTime = time(0);
	tm* currentDate = localtime(&currentTime);
	gettimeofday(tnow,NULL);

	output_rtc[0] = (double)currentDate->tm_year+1900;
	output_rtc[1] = (double)currentDate->tm_mon+1;
	output_rtc[2] = (double)currentDate->tm_mday;
	output_rtc[3] = (double)currentDate->tm_hour;
	output_rtc[4] = (double)currentDate->tm_min;
	output_rtc[5] = (double)currentDate->tm_sec;
	output_rtc[6] = (double)tnow->tv_usec;

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

void rtc_reset_alarm_i2c(BlackLib::BlackI2C* rtc_i2c, bool isOpened_i2c ){
	if ( !isOpened_i2c)
	{
		cout << "I cannot open the I2C device!" << endl;
		exit(1);
	}

	rtc_i2c->writeByte(0x0F,0b00001000);

}

void msleep(unsigned long millisec)
{
    struct timespec req={0};
    time_t sec=(int)(millisec/1000);
    millisec=millisec-(sec*1000);
    req.tv_sec=sec;
    req.tv_nsec=millisec*1000000L;
    while(nanosleep(&req,&req)==-1)
         continue;
}
