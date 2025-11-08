/*
 * Reaction_Timer.c
 *
 * Description: Reaction timer game for BeagleY-AI.
 *              Measures user's reaction time to visual stimuli.
 *
 * Author: Amytis Saghafi (amytis_saghafi@sfu.ca)
 * Date of last modification: Oct. 2025 
 */

#include <stdio.h>
#include <stdlib.h>  // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <stdbool.h>
#include "hal/led.h"
#include "hal/timing.h"
#include "hal/SPI.h"
#include <time.h>

typedef struct reaction_returns
{
    long long HighScore;
    bool status;
} reaction_returns;

void startup() {  
    /*1. Print a “get ready” message, and do the following four (4) times to flash the BYAI’s LEDs:
    a) Turn on the green LED for 250ms, then turn off.
    b) Turn on the red LED for 250ms, then turn off.*/
    
    printf("\nGet Ready...\n");
    for (int i = 0; i < 4; i++){
        GreenLed_flash(250, 1); // Flash green LED for 250ms, 1 time
        sleepForMs(250);    // Wait for 250ms
        RedLed_flash(250, 1);   // Flash red LED for 250ms, 1 time
        sleepForMs(250);    // Wait for 250ms
    }
}

void promptUser(int direction) {
    if (direction == 0) {
        printf("Press UP now!\n");
        GreenLed_turnOn();
    } else {
        printf("Press DOWN now!\n");
        RedLed_turnOn();
    }
}
bool randomWait(){
    int wait_min_ms = 500;
    int wait_max_ms = 3000;
    
    // got random from the internet
    /* seed RNG so rand() produces different sequences across runs */
    srand(getTimeInMs() % 1000); // seed with current time in ms mod 1000
    /* get a random wait time between 0.5s and 3s */
    int randomWait = wait_min_ms + (rand() % (wait_max_ms - wait_min_ms + 1));

    long long current_time = getTimeInMs();
    while (getTimeInMs() - current_time < randomWait) {
        joystick_values jv = Read_ADC_Values();
        // wait
        if (jv.x > 25 || jv.y > 25 || jv.x < -25 || jv.y < -25) {
            printf("too soon!\n");
            return false; // restart wait if joystick is not centered
            break;
        } 
    }
    return true;

}
reaction_returns getReactionTime(reaction_returns current){
    joystick_values jv;
    long long reaction_time_ms;
    

    // get random direction: 0 for up (green), 1 for down (red)
    /* seed RNG so rand() produces different sequences across runs */
    srand(getTimeInMs() % 1000); // seed with current time in ms mod 1000
    int direction = rand() % 2; // 0 for up and 1 for down

    // prompt user to move joystick in that direction
    promptUser(direction);

    // get start time
    long long startTime = getTimeInMs();

    // wait for user to move joystick or timeout after 5 seconds
    while(getTimeInMs() - startTime < 5000){ // 5 seconds to respond
        
        jv = Read_ADC_Values();
        if (jv.x > 50 || jv.x < -50) {
            printf("User selected to quit.\n");
            current.status = false;
            return current;
        }
        else if ((direction == 0 && jv.y > 50)||(direction == 1 && jv.y < -50)) { // correct direction
            reaction_time_ms = getTimeInMs() - startTime;
            printf("Correct!\n");
            GreenLed_flash(100, 5); // flash the green LED on and off five times in one second
            if (reaction_time_ms < current.HighScore) {
                current.HighScore = reaction_time_ms;
                printf("New record! Fastest reaction time: %lld ms\n", current.HighScore);
            }
            else {
                printf("Your reaction time was %lld ms. Fastest reaction time remains: %lld ms\n", reaction_time_ms, current.HighScore);
            }
            return current;
            break;
        } 
        else if ((direction == 0 && jv.y < -50)||(direction == 1 && jv.y > 50) ){ // wrong direction
            printf("Incorrect direction!.\n");
            RedLed_flash(100, 5); // flash the red LED on and off five times in one second
            return current;
            break;
        } 
    }
    printf("No input within 5 seconds; quitting!\n");
    current.status = false;
    return current;
}
reaction_returns game (reaction_returns current){
    joystick_values jv;

  
    // print startup message and flash LEDs
    

    // ensure joystick is centered before starting
    jv = Read_ADC_Values();
    if (jv.x > 25 || jv.y > 25 || jv.x < -25 || jv.y < -25) {
        printf("Please let go of joystick\n");
    } 
    // do startup
    startup();

    // Wait for random time between 0.5s to 3s
    if (!randomWait()) {
        return current; // restart game if joystick is not centered
    }
    
    // initialize game and get reaction time
    current = getReactionTime(current);

    // cleanup
    GreenLed_cleanup(); 
    RedLed_cleanup();

    return current;
}

int main(){
    
    reaction_returns current = {5000, true};

    printf("When the LEDs light up, press the joystick in that direction! \n (green -> up, red -> down)\n");
    printf("(Press left or right to exit)\n");
    while (current.status == true) {
        current = game(current);
    }

    return 0;
    
    

}
