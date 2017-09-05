# CHIP8 Interpreter
C++ implementation of the CHIP8 virtual machine used in the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. Graphics and keyboard inputs are handled using the SFML Library.


## About CHIP8
The origianl CHIP8 system is interpreted programming language (run on a CHIP8 vitural machine) and was made to allow video games to be more easily programmed for 8-bit microcomputers.

CHIP8 has the follow specs:

*4KB of RAM, from location 0x000 (0) to 0xFFF (4095)
*16 8-bit data registers
*Register I is 16 bits wide (used with memory operations)
*A stack to store return addresses from subroutines
*A delay timer for delays used in ROMs
*A sound timer that outputs one tone when the timer is not zero 
*A hex keypad
*64×32 display resolution 
*35 opcodes

## Motivation
The motivation behind this project was to get an understanding of the basics of hardware emulation. Even though the CHIP8 is an interpreted computer language rather than physical hardware, it is common to create a CHIP8 "emulator" as a beginners project to emulation.

## How to run
To run the program, download the project and navigate the the release folder and run the exe (windows only). The rom, font and sound folders must be in the release folder. Running from the exe will run the game Wipeoff. To run other roms, the loadROM(string) file must be changed in main.cpp and recompiled. 

## How to play
The original CHIP8 system used a hex keypad to control the system. 

An orignal hex keypad uses the following keys:

1 2 3 C
4 5 6 D
7 8 9 D
A 0 B F

This interpreter uses the following keys on a standard qwerty keyboard:

 1 2 3 4
 q w e r           
 a s d f 
 z x c v
 
 Different ROMs use different keys to control the game. 
 
## How to develop
This project was developed in Visual Studio 2015 using the SFML libraries. The SFML dlls need to be included in the project. Following the link will provide all the steps to compile the SFML libraries in Visual Studio
 - http://www.sfml-dev.org/tutorials/2.4/start-vc.php
 
## Screenshots
### Blinky
![Blinky](https://github.com/bryonkucharski/CHIP8Interpreter/blob/master/CHIP8/pictures/Blinky.PNG)

### Pong 
![Pong](https://github.com/bryonkucharski/CHIP8Interpreter/blob/master/CHIP8/pictures/Pong.PNG)

### Space Invaders
![Space Invaders](https://github.com/bryonkucharski/CHIP8Interpreter/blob/master/CHIP8/pictures/Space%20Invaders.PNG)

### Tetris
![Tetris](https://github.com/bryonkucharski/CHIP8Interpreter/blob/master/CHIP8/pictures/Tetris.PNG)

### Wipeoff
![Wipeoff](https://github.com/bryonkucharski/CHIP8Interpreter/blob/master/CHIP8/pictures/WIPEOFF.PNG)

## References

*https://en.wikipedia.org/wiki/CHIP-8#cite_note-memi-4
*http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0.0
*http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
*http://www.codeslinger.co.uk/pages/projects/chip8.html
