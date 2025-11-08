# ENSC 351 - Assignment 1 - Reaction Timer
Written by: Amytis Saghafi
Last Updated: November 8th/ 2025 
## Application Functionality 
Display the welcome message and directions on how to play.
Continuously loop through the following steps:
1. Print a “get ready” message, and do the following four (4) times to flash the BYAI’s LEDs:
    a) Turn on the green LED for 250ms, then turn off.
    b) Turn on the red LED for 250ms, then turn off.
2. If the user is pressing the joystick up or down, tell them “Please let go of joystick” (just once)
and then wait until the joystick is not pressing up or down.
3. Wait a random amount of time between 0.5s and 3.0s.
4. If, after the delay, the user is pressing the joystick up or down, tell them “too soon” and go
back to step 1. No need to check for joystick state during the delay.
5. Picks a random direction (up or down)
    a) Print the direction name to the screen.
    b) Display on the BYAI’s LED:
    If chose “Up”, the turn on the green LED.
    If chose “Down” turn on the red LED.
6. Time how long it takes for the user to press the joystick in any direction.
    If timer > 5s, exit program right away with a message.
7. Process the user’s joystick press:
    a) If the user pressed up or down correctly, then:
    - print a message,
    - if this was the fastest correct response time yet then display that in a message,
    - display this attempt’s response time, and best response time so far (in ms),
    - flash the green LED on and off five times in one second.
    b) If the user pressed up or down incorrectly, print a message and flash the red LED on and
    off five times in one second.
    c) If the user pressed the joystick left or right, print a message and quit.

## BEAGLE-Y-AI PIN CONNECTIONS 

- Joystick Module is connected via SPI
    - select - not used
    - VRx - channel 0
    - VRy - channel 1

## CMAKE
### One Line Commands
- for host:
    ```shell
        ./buildHost.sh
    ```
- for target:
    ```shell
        ./buildTarget.sh
    ```

### Manually Running CMake

To manually run CMake from the command line use:

#### Host (build for the machine you're running on)

    ```shell
    # Regenerate build/ folder and makefiles:
    rm -rf build/         # Wipes temporary build folder
    cmake -S . -B build   # Generate makefiles in build

    # Build (compile & link) the project
    cmake --build build
    ```

#### Target (cross-compile for aarch64)

Use one of these minimal options (do this from the repo root). Remove the old `build/` or `build/CMakeCache.txt` before changing compilers.

    Option A — pass compilers directly to CMake:

        ```shell
        rm -rf build/
        cmake -S . -B build \
        -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
        -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
        cmake --build build
        ```

    Option B — set environment variables for a one-shot configure:

        ```shell
        rm -rf build/
        CC=aarch64-linux-gnu-gcc CXX=aarch64-linux-gnu-g++ cmake -S . -B build
        cmake --build build
        ```
