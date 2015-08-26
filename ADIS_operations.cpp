/*
 * ADIS_convert.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: developer
 */

#include "inc/INIT.h"
#include "inc/ADIS_operations.h"

void adis_init(uint8_t adis_commands[]){

	uint16_t adis_dataread_cmds[] = {
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

	adis_16bit_commands_to_char(adis_dataread_cmds, adis_commands, 13); // Convert commands to weird ADIS style

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
	}*/

	adis_spi->transfer(commands, data, ADIS_BUFFER_SIZE__, 0);

}

void adis_extract_message(uint8_t* data, double* output) {

	// Conversion of measurements
	double accl_res_conv_ = 1;
	double gyro_res_conv_ = 0.05;
	//double std_gravity_ = 9.80665;
	//double millig_to_accl_ = std_gravity_/1000;
	double temp_res_conv_ = 0.136;
	double temp_zero_ = 25.0;
	double pow_res_conv_ = 0.002418;

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

void adis_display(double adis_data[]){

	cout << " prod_id: " << adis_data[0] << endl;
	cout << " system_stat: " << adis_data[1] << endl;
	cout << " power_sup: " << adis_data[2] << endl;
	cout << " accl_x: " << adis_data[3] << endl;
	cout << " accl_y: " << adis_data[4] << endl;
	cout << " accl_z: " << adis_data[5] << endl;
	cout << " gyro_x: " << adis_data[6] << endl;
	cout << " gyro_y: " << adis_data[7] << endl;
	cout << " gyro_z: " << adis_data[8] << endl;
	cout << " temp_x: " << adis_data[9] << endl;
	cout << " temp_y: " << adis_data[10] << endl;
	cout << " temp_z: " << adis_data[11] << endl;
	cout << endl;
}


