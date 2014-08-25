//=====================================================================
// Project: SnakeRobot V1.0
// Description: Code for the Snake Robot.
//
// Compiled with Arduino Version 1.0.5 
// Date: 8/8/2014
//
// Programmer: Peter Leng
// MIT Licence
// =====================================================================

// Libraries
#include <Servo.h>
#include <Oscillator.h>
#include <PS2X_lib.h>

// Board info 
#define TOTALSERVO 12

// Pins for the PS2 controller
#define PS2_CLK 14
#define PS2_CMD 15
#define PS2_SEL 11
#define PS2_DAT 16

// Modes
#define CRAWLMODE 0 
#define CAMERAMODE 1 // Let half of the robot rise and let the user freely move that part

// Speeds (in period)
#define MAXSPEED 750
#define MINSPEED 1600

// Settings for Ah (horizontal amplitude)
#define MAXAH 30
#define MINAH 0

// Controller delay time(ms)
#define CONTROLLERDELAY 300
#define BUTTONDELAY 100
#define CRAWLDELAY 50

// Servo pins (From front to end)
const int servoPins[TOTALSERVO] = {1,2,3,4,5,6,7,8,9,10,12,13};

// Different periods for the oscillations
#define T0  4000
#define T1  1000

// Gait settings  0    1
int T[] =        {T0,  T1 };
int Av[] =       {0,   30 };
int Ah[] =       {0,   0  };
int phd_v[] =    {0,   90 };
int phd_h[] =    {0,   90 };
int phd_vh[] =   {0,   0  };
int phd_ini[] =  {0,   0  };

// Description of the locomotion gaits
// 0: Robot is turned off
// 1: Be use for linear progression and side moving
// Others will be added

// Declaring objects
PS2X ps2x;
Oscillator osc[TOTALSERVO];

// Variables 
static bool robotIsOn = false;
static byte controlMode = CRAWLMODE;
static int crawlSpeed = 0;

// PS2 Controller stick positions
static int leftY;
static int leftX;
static int rightY;
static int rightX;

// ======================================================================
// Functions
// ======================================================================

// Get the global snake parameters from the current gait
void mapSnake(int seq){
  // Calculate the global snake
  for (int i = 0;i< TOTALSERVO;i++){
    
    osc[i].SetO(0);
    
    // Even modules
    if (i%2 == 0){
      osc[i].SetA(Av[seq]);
      osc[i].SetPh(DEG2RAD(phd_ini[seq]) + (i/2) * phd_v[seq]);
    }
    // Odd modules
    else{
      osc[i].SetA(Ah[seq]);
      osc[i].SetPh(DEG2RAD(phd_ini[seq]) + phd_vh[seq] + (i-1)/2 * phd_h[seq]);
    }
    osc[i].SetT(T[seq]);
  }
}

// Read PS2 controller values
void readValues(){
  
  // Read gamepad
  ps2x.read_gamepad(false, 0);
  // Get stick data and map them so we can use them easier
  leftY = map(ps2x.Analog(PSS_LY),255,0,-100,100);
  leftX = map(ps2x.Analog(PSS_LX),255,0,-100,100);
  rightY = map(ps2x.Analog(PSS_RY),255,0,-100,100);
  rightX = map(ps2x.Analog(PSS_RX),255,0,-100,100);
}

// Perform the changes to the state of the robot
void performChanges(){
  
  // Calculate the speed of the robot by using the position of the stick 
  // and maping it to periods
  crawlSpeed = map(int(sqrt((leftY*leftY)+(leftX*leftX))),0,100,MINSPEED,MAXSPEED);
  T[1]= crawlSpeed;
  // Change the horizontal amplitude of all  hornizonal servos so we can 
  // achieve side moving 
  // Here we use the absolute value of the leftX value and map
  // it from 0 to 30. 0 making robot crawl completely straight and 
  // 30 making robot moving to the side completely.
  // anythng in between will have combined effects.
  Ah[1] = map(abs(leftX),0,100,MINAH,MAXAH); 
}

// This refreshes the servos 
void refresh(){
  // leftY is -1 because that's it's neutral position
  if(leftY == -1 && leftX == 0){
   // Do nothing because in neutral position
   mapSnake(0);
   for (int i = 0; i < TOTALSERVO; i++){
      osc[i].refresh();
   }
  }
  
  // Here the left stick is in the northwest location  
  // so we only need to reverse the odd servos
  if (leftY >= 0 && leftX > 0){
    for (int i = 0; i < TOTALSERVO; i++){
      if (i%2 == 0){
        osc[i].refresh();
      }
      else{
        osc[i].refresh(true); 
      }
    }
  }
  
  // Here the left stick is in the northeast location 
  // so we don't need to reverse the servos
  if (leftY >= 0 && leftX < 0){
    for (int i = 0;i < TOTALSERVO; i++){
      osc[i].refresh(); 
    }
  }
  // Here the left stick is in the southeast location 
  // so we need to reverse all the servos 
  if (leftY <= 0 && leftX < 0){
    for (int i = 0; i < TOTALSERVO; i++){
      osc[i].refresh(true); 
    }
  }
  // Here the left stick is in the southwest location 
  // so we only need to reverse the even servos 
  if (leftY < 0 && leftX > 0){
    for (int i = 0;i < TOTALSERVO; i++){
      if (i%2 == 0){
        osc[i].refresh(true);
      }
      else{
        osc[i].refresh();
      }
    }
  }
}



// ======================================================================
// Setup
// ======================================================================

void setup(){
  // For debugging
  // Serial.begin(9600);
  int error;
  
  // Setup gamepad (clock, command, attention, data) pins
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);
  // Give the controller some time
  delay(CONTROLLERDELAY);
  // Attach the oscillators to the servos
  for (int i = 0; i < TOTALSERVO; i++){
      osc[i].attach(servoPins[i]);
  }

  // Snake be neutral
  mapSnake(0);
  
  for (int i = 0; i < TOTALSERVO; i++){
      osc[i].refresh();
  }
}

// ======================================================================
// Loop
// ======================================================================

void loop(){
   
  //Try to receive information from ps2 controller
  readValues();
  
  //Verify that the read_gamepad succeeded
  if ((ps2x.Analog(1) & 0xf0) == 0x70){
   
    //If start button is pressed either turn off he robot or turn it on
    if (ps2x.ButtonPressed(PSB_START)){
      if (robotIsOn){
          for (int i = 0; i < TOTALSERVO; i++){
             //Serial.println("Stoping");
             osc[i].Stop();
          }
         delay(BUTTONDELAY); 
         robotIsOn = false;
      }
      else {
          for (int i = 0; i < TOTALSERVO; i++){
             //Serial.println("Starting");
             osc[i].Play(); 
          }
         delay(BUTTONDELAY);
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
        delay(BUTTONDELAY);
      }
      
      if (ps2x.ButtonPressed(PSB_R1)){

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
        performChanges();
        mapSnake(1);
        refresh();
        delay(CRAWLDELAY);
      }
      
      if (controlMode == CAMERAMODE){
       
      
      }
    }
    
  }
}
