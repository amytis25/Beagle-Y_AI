/*
 * timing.h
 *
 * Description: 
 *  - Interface for timing functions using BeagleY-AI
 *
 * Author: Amytis Saghafi (amytis_saghafi@sfu.ca) and Melissa Mosavi (melissa_mosavi@sfu.ca)
 * Date of last modification: Nov. 2025 
 */

#ifndef TIMING_H
#define TIMING_H
#include <time.h>

// Get the current time in milliseconds
long long getTimeInMs(void);

// Sleep for the specified delay in milliseconds
void sleepForMs(long long delayInMs);

#endif

