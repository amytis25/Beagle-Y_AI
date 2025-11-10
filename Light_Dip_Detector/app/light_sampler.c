/*
 * light_sampler.c
 *
 * Description: 
 *  - Use the light sensor to read current light level.
 *       - using SPI.c through sampler.c
 *   - compute and print out the number of dips in the light intensity seen in the previous second.
 * - Use the rotary encoder to control how fast the LED blinks (using PWM). This LED emitter
 *    is designed to flash directly at the “detector” light sensor.
 *       - using rotary_encoder.c to encode switch with value and PWM.c to take this value and set the LED blink rate
 * - Each second, print some information to the terminal.
 *       - host terminal: utilizes udp_helper.c and printf
 *       - local (target) terminal: printf
 * - Listen to a UDP socket and respond to commands
 *       - UDP.c
 *       - udp_helper.c is to send 1s interval host terminal commands only.
 * - Use the terminal to display some simple status information.
 *       - host terminal: done in main function and udp_helper.c
 *       - target terminal: done in main function
 *
 * Author: Amytis Saghafi (amytis_saghafi@sfu.ca) and Melissa Mosavi (melissa_mosavi@sfu.ca)
 * Date of last modification: Nov. 2025 
 */


#include "hal/timing.h"
#include "hal/sampler.h"
#include "hal/periodTimer.h"
#include "hal/PWM.h"
#include "hal/rotary_encoder.h"
#include "hal/UDP.h"
#include "hal/udp_helper.h"   

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

#define MS_IN_SECOND 1000
#define STUDENT_NAME "Melissa Mosavi & Amytis Saghafi"

// Flag to indicate when the program should exit
static volatile bool running = true;

// Signal handler for CTRL+C
static void cleanup_handler(int signo) {
    if (signo == SIGINT) {
        // Avoid printf in signal handler; just request shutdown:
        running = false;
    }
}

// Handler for <3Hz blinks
static const int HW_SAFE_FREQ = 100;      // carrier when doing soft blink
static bool soft_mode = false;
static long long soft_next_ms = 0;
static bool soft_on = false;        // whether LED is "on" phase in soft mode

// Hysteresis for soft-blink switching to mitigate boundary case glitching
static const int SOFT_ENTER_HZ = 3;  
static const int SOFT_EXIT_HZ  = 5; 

// Function to cleanup all resources
static void cleanup_resources(void) {
    // Stop UDP server (control)
    udp_stop();

    // Stop UDP helper (host status updates)
    udp_helper_close();
    
    // Cleanup all modules in reverse order of initialization
    PWM_disable();
    Sampler_cleanup();
    rotary_close();
    Period_cleanup();
    
    printf("\nCleanup complete. Exiting.\n");
}

// Current PWM frequency (Hz). Made global so UDP control callbacks can access it.
static int current_freq = 10;

// Format and display status each second (fixed-width fields for stable alignment).
// (Local terminal only; host push is done by udp_helper_send_status().)
static void display_status(
    int samples_in_second,
    int led_hz,
    double avg_light,
    int dips,
    const Period_statistics_t *light_stats,
    const double *history_samples,
    int history_size)
{
    (void)samples_in_second;
    (void)avg_light;
    (void)history_samples;
    (void)history_size;

    // BYAI console output
    printf("\nStatus:\n");
    printf("Name: %s\n", STUDENT_NAME);
    printf("LED (PWM) flashes: %d Hz\n", led_hz);
    printf("Dips in last 1 s: %d\n", dips);
    if (light_stats) {
        printf("Max time between ADC light samples: %6.1f ms\n", light_stats->maxPeriodInMs);
    } else {
        printf("Max time between ADC light samples: N/A\n");
    }
    fflush(stdout);
}

static void cb_request_shutdown(void) { running = false; } // for UDP stop command

int main() {
    // Set up signal handler for CTRL+C
    if (signal(SIGINT, cleanup_handler) == SIG_ERR) {
        fprintf(stderr, "Failed to set up signal handler\n");
        return -1;
    }
    
    printf("Starting light_sampler application...\n");

    // Initialize modules
    Period_init();  // Initialize period timer first
    if (!PWM_export()) {
        fprintf(stderr, "Failed to export PWM\n");
        return -1;
    }
    
    if (!rotary_init()) {
        fprintf(stderr, "Failed to initialize rotary encoder\n");
        return -1;
    }

    Sampler_init();
    rotary_start();
    PWM_enable();

    // Start UDP command server, port 12345
    UdpCallbacks cb = {
        .get_count        = Sampler_getNumSamplesTaken,
        .get_history_size = Sampler_getHistorySize,
        .get_dips         = Sampler_getDipCount,
        .get_history      = Sampler_getHistory,
        .request_shutdown = cb_request_shutdown
    };

    // UDP socket for command server, port 12345 
    if (udp_start(12345, cb) != 0) {
        fprintf(stderr, "udp_start failed\n");
    }

    // UDP socket for host live updates, port 12346  
    if (!udp_helper_init("192.168.7.1", 12346)) {
        fprintf(stderr, "udp_helper_init failed\n");
    }

    // Set initial PWM frequency @ 50% duty cycle
    if (!PWM_setFrequency(current_freq, 50)) {
    fprintf(stderr, "PWM_setFrequency failed at init\n");
    }

    long long lastTime = getTimeInMs();

    // Main processing loop
    while (running) {
        // Update LED blink rate based on rotary encoder
        int edges = rotary_getCount();
        int detents = edges / 4;
        
        // Requested frequency from knob 
        int req = 10 + detents;
        if (req < 0) { // If already at 0Hz and decreasing
            req = 0;
            rotary_resetCount(-10 * 4);        // cut rotary movements to keep at '0'. 10 Hz offset
            PWM_setFrequency(HW_SAFE_FREQ, 0); // LED fully off
            soft_mode = true; // ensure reset to soft mode for 1Hz to work again
            soft_on = false; 
        } else if (req >= 500) {
            req = 500;
            rotary_resetCount((500 - 10) * 4); // keep at '500'. 10 Hz offset
            }

        //Soft mode for 2Hz with Hysteresis
        bool want_soft = soft_mode ? (req < SOFT_EXIT_HZ) : (req <= SOFT_ENTER_HZ); 
        if (want_soft) {
            if (!soft_mode) {
                soft_mode   = true;
                soft_on     = false;                         // start OFF
                PWM_setFrequency(HW_SAFE_FREQ, 0);           // 100 Hz carrier, LED off
                soft_next_ms = getTimeInMs() + (req ? 500/req : 500); // half-period
                }
                
            long long now = getTimeInMs();
            if (req == 0) {
                if (now >= soft_next_ms) soft_next_ms = now + 500;
                PWM_setFrequency(HW_SAFE_FREQ, 0);
            } else if (now >= soft_next_ms) {
                soft_on = !soft_on;
                PWM_setFrequency(HW_SAFE_FREQ, soft_on ? 50 : 0);
                soft_next_ms = now + 500/req;                // half-period
                }
            } else { // PWM mode
                if (soft_mode) { // if in soft mode, leave soft mode
                    soft_mode = false;
                    soft_on   = false;
                }
                    
                static int last_hw = -1;
                if (req != last_hw) {
                    PWM_setFrequency(req, 50);
                    last_hw = req;
                }
            }       
        
        current_freq = req;

        long long timeDiff = (getTimeInMs() - lastTime)/MS_IN_SECOND;
        if (timeDiff >= 1) {
            // Process light samples every second
            Sampler_moveCurrentDataToHistory();
            
            Period_statistics_t _lastSecondsSample;

            _lastSecondsSample = Sampler_getLastSecondStatistics();
            
            int dips_in_last_second = Sampler_getDipCount();
            int history_size;
            double* samples = Sampler_getHistory(&history_size);
            if (samples) {
                double avg = Sampler_getAverageReading();
                lastTime = getTimeInMs();

                // 1) Local terminal print (target)
                display_status(
                    history_size,           // samples in previous second
                    current_freq,           // LED Hz
                    avg,                    // averaged light level (V)
                    dips_in_last_second,    // dips found in previous second
                    &_lastSecondsSample,    // timing jitter stats for light samples
                    samples,                // history samples from previous second
                    history_size);

                // 2) Host live update via UDP helper (12346 port)
                udp_helper_send_status(
                    history_size,           // samples in previous second
                    current_freq,           // LED Hz
                    avg,                    // averaged light level (V)
                    dips_in_last_second,    // dips found in previous second
                    &_lastSecondsSample,    // timing jitter stats for light samples
                    samples,                // history samples from previous second
                    history_size);
                
                free(samples);
            }
        }
    }
    // Perform cleanup
    cleanup_resources();
    return 0;
}