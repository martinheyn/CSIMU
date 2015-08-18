/*
 * ADIS_16364.h
 *
 *  Created on: Jul 30, 2015
 *      Author: developer
 */

#ifndef ADIS_16364_H_
#define ADIS_16364_H_

#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>
#include <ctime>
#include <cmath>
#include <sys/time.h>
#include <iomanip>
//#include <zmq.hpp>
//#include "imuudpio.pb.h"
//#include "multiadismessage.pb.h"
#include <signal.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include "gpiopin.hpp"
#include <poll.h>
#include <fstream>
#include "lib/BlackLib/BlackLib.h"
#include "lib/BlackLib/BlackCore.h"
#include "lib/BlackLib/BlackErr.h"
#include "lib/BlackLib/BlackDef.h"
#include <thread>
#include <ctime>
#include <stack>
#include <cstdarg>

#define ADIS_BUFFER_SIZE__ 26 // Buffer size is number of registers + 2 normally
#define PROD_ID__ 16364
#define ADIS_NUMREGISTERS__ 12 // how many registers are we going to read
#define RTC_BUFFER_SIZE__ 5 // Buffer size
#define RTC_NUMREGISTERS__ 5 // how many registers are we going to read
#define NEWLINE__ '\n'
#define DELIMITER__ ';'
#define LED0_PATH "/sys/class/leds/beaglebone:green:usr0"
#define LED1_PATH "/sys/class/leds/beaglebone:green:usr1"
#define LED2_PATH "/sys/class/leds/beaglebone:green:usr2"
#define LED3_PATH "/sys/class/leds/beaglebone:green:usr3"
#define TIMESTEP__ 1000; // Time step in milliseconds



typedef enum {
    SYS_E_FLAG = 0x3400, // we take MSC_CTRL here
    DIAG_STAT = 0x3C00, // we take DIAG_STAT here

    X_TEMP_OUT = 0x1000,
	Y_TEMP_OUT = 0x1200,
	Z_TEMP_OUT = 0x1400,
    X_GYRO_OUT = 0x0400,
    Y_GYRO_OUT = 0x0600,
    Z_GYRO_OUT = 0x0800,
    X_ACCL_OUT = 0x0A00,
    Y_ACCL_OUT = 0x0C00,
    Z_ACCL_OUT = 0x0E00,

    PROD_ID = 0x5600,
    SERIAL_NUM = 0x5800,
	POW_SUP = 0x0200,

} adis_commands;

typedef enum {
	SECONDS = 0x0000,
	MINUTES = 0x0100,
	HOURS = 0x0200,
	DATE = 0x0400,
	MONTH = 0x0500,
	YEAR = 0x0600,

} rtc_commands;

#endif /* ADXL345_H_ */

