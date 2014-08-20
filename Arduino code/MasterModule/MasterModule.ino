//=====================================================================
// Project: SnakeRobot V1.0
// Description: Master module of the Snake Robot.
// This sends control data from a PS2 controller to the
// slave modules via I2C, 
// 
// Compiled with Arduino Version 1.0.5 
// Date: 8/8/2014
//
// Programmer: Peter Leng
// MIT Licence
// =====================================================================

// Libraries
#include <PS2X_lib.h>
#include <Wire.h>
// Constants
#define CRAWLMODE 0 
#define CAMERAMODE 1 // Let half of the robot rise and let the user freely move that part

// Pins
#define SLAVE_1 10
#define SLAVE_2 11
#define SlAVE_3 12
#define SlAVE_4 13

#define PS2_CLK 5
#define PS2_CMD 4
#define PS2_SEL 3
#define PS2_DAT 2
 
#define TOTALBOARDS 4 //Total Number of boards
#define MPERBOARD 4 //Modules per board
#define TOTALMODS TOTALBOARDS*MPERBOARD // Total modules in the snake  
// Objects
PS2X ps2x;

// Variables
static bool robotIsOn = false;
static byte controlMode;

// ======================================================================
// Setup
// ======================================================================

void setup(){
   int error;
   
   // Begin the I2C interface
   wire.begin();
   // Setup gamepad (clock, command, attention, data) pins
   error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);
   
}
 
// ======================================================================
// Loop
// ======================================================================

void loop(){
   
  //Try to receive information from ps2 controller
  ps2x.read_gamepad();
  
  //Verify that the read_gamepad succeeded
  if ((ps2x.Analog(1) & 0xf0) == 0x70){
   
    //If start button is pressed either turn off he robot or turn it on
    if (ps2x.ButtonPressed(PSB_START)){
      if (robotIsOn){
        // TURN ROBOT OFF   
      }
      else {
        robotIsOn = true;
      }
    }
    //If robot is on then do the following
    if (robotIsOn){
      
      // Camera mode
      if (ps2x.ButtonPressed(PSB_L1)){
        if (controlMode != CAMERAMODE){
          controlMode = CAMERAMODE;
        }
        else {
          controlMode = CRAWLMODE;
        }
      }
      
      if (ps2x.ButtonPressed(PSB_R1)){
        // Not yet decided
      }
      
      if (ps2x.ButtonPressed(PSB_CIRCLE)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_CROSS)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_SQUARE)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_TRIANGLE)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_PAD_UP)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_PAD_DOWN)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_PAD_RIGHT)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_PAD_LEFT)){
        
      }
      
      // crawlMode
      if (controlMode == CRAWLMODE){
        // Send PS2 stick data to all boards
        for (int i = 0; i < TOTALBOARDS; i++){
          wire.beginTransmission(i);
          // sending "S" so slaves would know what kind of data 
          // they would get next.
          // The stick data are in this ORDER
          wire.write("S");
          wire.write(ps2x.Analog(PSS_LY));
          wire.write(ps2x.Analog(PSS_LX));
          wire.write(ps2x.Analog(PSS_RY));
          wire.write(ps2x.Analog(PSS_RX));
          wire.endTransmission();
        }
        
      }
      
      if (controlMode == CAMERAMODE){
        // Send some data
      
      }
    }
    
    
  }

}
