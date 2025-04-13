Console I/O Library
===================

This library contains functions and definitions to facilitate output to
console (terminal) devices.

These functions include:

* ANSI escape sequences to control color, cursor appearance, etc.
* Functions to determine if stdout or stderr is to a terminal window
* Functions to determine if a terminal is ANSI-capable
* Functions to determine the terminal size

Additionally, this repository defines a ProgressMeter object that utilizes
some of the above utility and ANSI-related functions to render a progress
meter in a console application.
