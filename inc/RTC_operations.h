/*
 * RTC_decode.h
 *
 *  Created on: Aug 18, 2015
 *      Author: developer
 */

#ifndef INC_RTC_OPERATIONS_H_
#define INC_RTC_OPERATIONS_H_

#define RTC_BUFFER_SIZE__ 5 // Buffer size
#define RTC_NUMREGISTERS__ 5 // how many registers are we going to read

#include "inc/INIT.h"

typedef enum {
	SECONDS = 0x0000,
	MINUTES = 0x0100,
	HOURS = 0x0200,
	DATE = 0x0400,
	MONTH = 0x0500,
	YEAR = 0x0600,

} rtc_commands;

// Forward declaration of functions
void rtc_read_i2c(BlackLib::BlackI2C* rtc_i2c, uint8_t* data_rtc, bool isOpened_i2c );
void rtc_write_i2c(BlackLib::BlackI2C* rtc_i2c, uint8_t* data_rtc, bool second_alarm, bool isOpened_i2c );
bool rtc_check_alarm (BlackLib::BlackI2C* rtc_i2c, bool second_alarm, bool isOpened_i2c );
void rtc_read_systemtime(timeval* tnow, double output_rtc[]);
bool rtc_check_stopalarm (int stoptime[], double output_rtc[]);
void rtc_set_alarm_i2c(BlackLib::BlackI2C* rtc_i2c, int* data, bool second_alarm, bool isOpened_i2c );
void rtc_reset_alarm_i2c(BlackLib::BlackI2C* rtc_i2c, bool isOpened_i2c );
void rtc_read_alarm_i2c(BlackLib::BlackI2C* rtc_i2c, double* data_rtc, bool isOpened_i2c );
void file_get_startstop (int* starttime, int* stoptime, std::string filelocation);
void rtc_extract_message(uint8_t data_rtc[], double output_rtc[]);
void rtc_display(double rtc_data[]);
void rtc_16bit_commands_to_char(uint16_t rtc_commands[], uint8_t char_commands[], size_t rtc_commands_size);
void msleep(unsigned long millisec); // Sleeping function

#endif /* INC_RTC_OPERATIONS_H_ */
