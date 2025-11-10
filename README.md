# ENSC 351 – Embedded Systems (Fall 2025)

Author: Amytis Saghafi \n
Instructor: Dr. Matthew Stewart
Platform: BeagleY-AI (Debian 13 Target + Debian 13 Host via VM)
Languages: C / CMake / POSIX / pthread
Focus: Real-time embedded programming, HAL design, Linux cross-compilation, multithreading, and hardware I/O control

## Repository Overview

This repository contains my completed programming assignments and system setup guides for ENSC 351: Embedded Systems, taken at Simon Fraser University (School of Engineering Science).
The course emphasizes practical experience with Linux-based embedded systems using the BeagleY-AI single-board computer.

## Reaction Timer

Goal: Develop a joystick-based reaction time game on BeagleY-AI.

### Key Concepts:
Cross-compiled C program using CMake.
Separate HAL modules for LED and Joystick.
Randomized timing and debouncing input.
Real-time response measurement using clock_gettime().
NFS mounting and auto-execution via RFS customization.

### Highlights:
Implemented auto-calibrating joystick thresholds.
Non-blocking LED flashing pattern using nanosleep().
Modular initialization/cleanup functions to ensure safe hardware shutdown.
Captured execution trace over SSH (as1-capture.txt).

## Setup and Hardware Guides

Included technical guides from the course and custom documentation:

- **QuickStartGuide.pdf** : Step-by-step setup of the Debian VM host and BeagleY-AI target. Covers cross-compiler toolchain installation, SSH/NFS configuration, and network verification.
- **Networking.pdf** : Detailed walkthrough for enabling Ethernet-over-USB networking between host and target, internet sharing via virtual machine interfaces, and static IP mapping for development.
- **NFSGuide.pdf** : Teaches how to configure a Linux NFS server and client, export shared folders, and mount them automatically on the BeagleY-AI for seamless host-target file synchronization.
- **LEDGuide.pdf** : Explains control of the BeagleY-AI’s dual-color (red/green) LED through the Linux sysfs interface and via C code. Demonstrates triggers, brightness control, and timed blinking using nanosleep().
- **GPIOGuide.pdf** : Demonstrates how to access digital I/O pins via /sys/class/gpio/ in Linux, including exporting pins, setting direction, reading/writing values, and implementing edge-triggered interrupts in C.
- **SPI_ADC_Guide.pdf** : Outlines setup and communication with an external SPI-connected ADC, enabling analog light-sensor sampling. Used extensively in Assignment 2 for real-time signal acquisition.
- **PWMGuide.pdf** : Covers enabling and configuring pulse-width modulation (PWM) on the BeagleY-AI for driving LEDs or motors. Includes overlay configuration, period/duty adjustment, and real hardware wiring examples.
- **RFS_Customization.pdf** : Shows how to customize the root file system—changing hostname, SSH login message (MOTD), and automatically executing user programs on login via .profile.
- **DebuggingGuide.pdf** :	Comprehensive GDB + VS Code debugging manual, explaining remote debugging through gdbserver, symbol stripping, core-dump analysis, and memory checking using Valgrind.

## Learning Outcomes
Through these assignments and hardware experiments, I developed:
- Expertise in embedded C and POSIX threading on Linux.
- Strong understanding of hardware abstraction layer design and modularity.
- Experience with cross-compiling and deploying via NFS & SSH.
- Familiarity with low-level interfaces (GPIO, SPI, PWM, ADC).
- Debugging skills using gdb and gdbserver in cross-debug configurations.
- Practical experience with real-time data acquisition and signal processing.

