/*
 * SPI.c
 *
 * Description: SPI functions for BeagleY-AI.
 *              Provides an interface to communicate with Joystick SPI devices.
 *
 * Author: Amytis Saghafi (amytis_saghafi@sfu.ca)
 * Date of last modification: Oct. 2025 
 */

#include "hal/led.h"
#include "hal/timing.h"
#include "hal/SPI.h"
#include <stdio.h> // fopen, fprintf, fclose, perror
#include <stdlib.h>  // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <stdbool.h>
#include <time.h>

// from SPI guide
// at center ch0 (x) = 2048, ch1 (y) = 2048
int read_ch(int fd, int ch, uint32_t speed_hz) {
    // fd is the file descriptor for the SPI device
    // ch is the channel number on the ADC
    // speed_hz SPI clock speed
    // tx this is our request message to the ADC
    // rx this is our receive buffer
    uint8_t tx[3] = { (uint8_t)(0x06 | ((ch & 0x04) >> 2)),
                      (uint8_t)((ch & 0x03) << 6),
                      0x00 };

    uint8_t rx[3] = { 0 };

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = speed_hz,
        .bits_per_word = 8,
        .cs_change = 0
    };

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) return -1;

    return ((rx[1] & 0x0F) << 8) | rx[2];  // 12-bit result
}

// Normalize 12-bit ADC value (0..4095) to -100..100 with center at 2048
int normalize_adc(int raw) {
    const int center = 2048;
    const int span = 2048; // symmetric span
    int offset = raw - center; // -2048..+2047
    double frac = (double)offset / (double)span; // approx -1..+1
    if (frac > 1.0) frac = 1.0;
    if (frac < -1.0) frac = -1.0;
    return (int)(frac * 100.0);
}

// from SPI guide
joystick_values Read_ADC_Values(void) {
    const char* dev = SPI_DEV_PATH;
    uint8_t mode = 0;       // SPI mode 0
    uint8_t bits = 8;
    uint32_t speed = 250000;
    joystick_values jv = {0, 0}; // Default return value in case of error

    int fd = open(dev, O_RDWR);
    if (fd < 0) { perror("open"); return jv; }
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) == -1) { perror("mode"); return jv; }
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) { perror("bpw"); return jv; }
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) { perror("speed"); return jv; }

    int ch0_x = read_ch(fd, 0, speed);
    int ch1_y = read_ch(fd, 1, speed);

    if (ch0_x < 0 || ch1_y < 0) {
        fprintf(stderr, "SPI read error\n");
    } else {
        int n_x = normalize_adc(ch0_x);
        int n_y = normalize_adc(ch1_y);
        //printf("CH0 (x) raw=%d normalized=%d  CH1 (y) raw=%d normalized=%d\n",ch0_x, n_x, ch1_y, n_y);
        jv.x = n_x;
        jv.y = n_y;
    }

    close(fd);
    return jv;
}