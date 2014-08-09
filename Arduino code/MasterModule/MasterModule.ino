//=====================================================================
// Project: SnakeRobot V1.0
// Description: Master module of the Snake Robot.
// This the sends control data from a PS2 controller to the
// slave modules, 
// 
// Compiled with Arduino Version 1.0.5 
// Date: 8/8/2014
//
// Programmer: Peter Leng
// MIT Licence
// =====================================================================

// Libraries
#include <PS2X_lib>

// Pins
#define SLAVE_1 10
#define SLAVE_2 11
#define SlAVE_3 12
#define SlAVE_4 13

#define PS2_CLK 5
#define PS2_CMD 4
#define PS2_SEL 3
#define PS2_DAT 2
 
// Objects
PS2X ps2x;

// Variables

// ======================================================================
// Setup
// ======================================================================

 void setup(){
   int error;
   
   Serial.begin(57600);
   // Setup gamepad (clock, command, attention, data) pins
   error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);  
   
   
 }
