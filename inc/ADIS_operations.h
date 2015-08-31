#ifndef INC_ADIS_OPERATIONS_H_
#define INC_ADIS_OPERATIONS_H_

#define ADIS_BUFFER_SIZE__ 26 // Buffer size is number of registers + 2 normally
#define ADIS_NUMREGISTERS__ 12 // how many registers are we going to read
#define PROD_ID__ 16364 // Product ID of sensor

#include "inc/INIT.h"

// Register addresses of sensors
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



// Forward declaration of functions
void adis_init(uint8_t adis_commands[]);
void adis_16bit_commands_to_char(uint16_t adis_commands[], uint8_t char_commands[], size_t adis_commands_size);
uint16_t adis_14bit_to_16bit(uint16_t adis_14bit);
uint16_t adis_12bit_to_16bit(uint16_t adis_12bit);
void adis_read_spi(BlackLib::BlackSPI* adis_spi, uint8_t* commands, uint8_t* data, bool isOpened_spi);
void adis_extract_message(uint8_t* data, double* output);
void adis_display(double adis_data[]);

#endif /* INC_ADIS_OPERATIONS*/
