/*
 * SPI.h
 *
 * Description: 
 *  - Interface for reading from an ADC via SPI using BeagleY-AI
 *
 * Author: Amytis Saghafi (amytis_saghafi@sfu.ca) and Melissa Mosavi (melissa_mosavi@sfu.ca)
 * Date of last modification: Nov. 2025 
 */

#ifndef SPI_H
#define SPI_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEV_PATH "/dev/spidev0.0"

// read and return ADC channel voltage
int Read_ADC_Values(int channel);

#endif