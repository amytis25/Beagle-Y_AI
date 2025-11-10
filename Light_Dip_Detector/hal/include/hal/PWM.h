// PWM.h
// ENSC 351 Fall 2025
// PWM control interface for GPIO15
/*
 * PWM.h
 *
 * Description: 
 *  - PWM control interface for GPIO15
 *
 * Author: Amytis Saghafi (amytis_saghafi@sfu.ca) and Melissa Mosavi (melissa_mosavi@sfu.ca)
 * Date of last modification: Nov. 2025 
 */
#ifndef PWM_H
#define PWM_H
#include <stdbool.h>

#define PWM_DUTY_CYCLE_FILE "/dev/hat/pwm/GPIO15/duty_cycle"
#define PWM_PERIOD_FILE "/dev/hat/pwm/GPIO15/period"
#define PWM_ENABLE_FILE "/dev/hat/pwm/GPIO15/enable"

// PWM helper Functions
bool PWM_export(void);
bool PWM_setDutyCycle(int dutyCycle);
bool PWM_setPeriod(int period);
bool PWM_setFrequency(int Hz, int dutyCyclePercent);
bool PWM_enable(void);
bool PWM_disable(void);

#endif