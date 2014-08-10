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
bool robotIsOn = false;
// ======================================================================
// Setup
// ======================================================================

void setup(){
   int error;
   
   Serial.begin(57600);
   // Setup gamepad (clock, command, attention, data) pins
   error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);  
   
   
}
 
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
      
      if (ps2x.ButtonPressed(PSB_CIRCLE)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_CROSS)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_SQUARE)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_TRIANGLE)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_PAD_UP){
        
      }
      
      if (ps2x.ButtonPressed(PSB_PAD_DOWN)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_PAD_RIGHT)){
        
      }
      
      if (ps2x.ButtonPressed(PSB_PAD_LEFT)){
        
      }
    }
    
    
  }

}
