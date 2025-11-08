/*
 * SPI.h
 *
 * Description: SPI functions for BeagleY-AI.
 *              Provides an interface to communicate with Joystick SPI devices.
 *
 * Author: Amytis Saghafi (amytis_saghafi@sfu.ca)
 * Date of last modification: Oct. 2025 
 */

#ifndef SPI_H
#define SPI_H

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEV_PATH "/dev/spidev0.0"
// joystick_values struct to hold normalized x and y values
typedef struct  {
    int x; // normalized x value (-100 to 100)
    int y; // normalized y value (-100 to 100)
} joystick_values;

// read a channel from the ADC and return the 12-bit raw value (0..4095)
int read_ch(int fd, int ch, uint32_t speed_hz);

// normalize a 12-bit ADC value (0..4095) to -100..100 with center at 2048
int normalize_adc(int raw);

// read and return normalized joystick x and y values
joystick_values Read_ADC_Values(void);

#endif