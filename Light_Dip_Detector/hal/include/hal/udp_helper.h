/*
 * udp_helper.h
 *
 * Description: 
 *  - Interface for pushing formatted status text to host over UDP port 12346
 *
 * Author: Amytis Saghafi (amytis_saghafi@sfu.ca) and Melissa Mosavi (melissa_mosavi@sfu.ca)
 * Date of last modification: Nov. 2025 
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "hal/periodTimer.h"

// Open sender socket to host:port (e.g., "192.168.7.1", 12346). Returns false on failure.
bool udp_helper_init(const char* host_ip, int port);

// Display status on host terminal
void udp_helper_send_status(
    int samples_in_second,
    int led_hz,
    double avg_light,
    int dips,
    const Period_statistics_t *light_stats,
    const double *history_samples,
    int history_size);

// Close sender socket.
void udp_helper_close(void);