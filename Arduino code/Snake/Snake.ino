//=====================================================================
// Project: SnakeRobot V1.0
// Description: Master module of the Snake Robot.
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
#define TOTALBOARDS 1 //Total Number of boards
#define MPERBOARD 12 //Modules per board
#define BOARD 1 //Specify which board(Arduino) you are uploading to
#define TOTALMODS TOTALBOARDS*MPERBOARD // Total modules in the snake

// Pins for the PS2 controller
#define PS2_CLK 14
#define PS2_CMD 15
#define PS2_SEL 11
#define PS2_DAT 16

// Modes
#define CRAWLMODE 0 
#define CAMERAMODE 1 // Let half of the robot rise and let the user freely move that part

// Speeds (in period)
#define MAXSPEED 700
#define MINSPEED 1600

// Servo pins
const int servoPins[MPERBOARD] = {1,2,3,4,5,6,7,8,9,10,12,13};

// Different periods for the oscillations
int T0 = 4000;
int T1 = 1000;

// Gait settings  0    1
int T[] =        {T0,  T1 };
int Av[] =       {0,   30 };
int Ah[] =       {0,   0  };
int phd_v[] =    {0,   90 };
int phd_h[] =    {0,   90 };
int phd_vh[] =   {0,   0  };
int ph_ini[] =   {0,   0  };

// Description of the locomotion gaits
// 0: Robot is turned off
// 1: Be use for linear progression and side moving
// Others will be added

// Number of elements in the sequence
int seq_size = sizeof(Av)/sizeof(int);
int seq = 0;  //Sequence counter

// Parameters of the global snake. They are calculated from the 
// global parameters Av, Ah, phd_v, phd_h and ph_ini
int snake_A[TOTALMODS];   // Amplitude
int snake_ph[TOTALMODS];  // Phase
int snake_T[TOTALMODS];   // Period

// Declaring objects
PS2X ps2x;
Oscillator osc[MPERBOARD];

// Variables 
static bool robotIsOn = false;
static byte controlMode = CRAWLMODE;
static int crawlSpeed = 0;

// PS2 Controller stick positions
static byte leftY;
static byte leftX;
static byte rightY;
static byte rightX;

// ======================================================================
// Functions
// ======================================================================

// Get the global snake parameters from the current gait
void globalSnake(int seq){
  // Calculate the global snake
  for (int i = 0; i< TOTALMODS; i++) {
    
    // Even modules:
    if (i % 2 == 0) {
      snake_A[i] = Av[seq];
      
      snake_ph[i] = ph_ini[seq] + (i/2) * phd_v[seq];
    }
    // Odd modules
    else {
      snake_A[i] = Ah[seq];
      snake_ph[i] = ph_ini[seq] + phd_vh[seq] + (i-1)/2 * phd_h[seq];
    }
    
    // The period is the same for all the modules
    snake_T[i] = T[seq];
    
  }
}

// Map the snake parameters into the board oscillators
void mapSnake(){
  // Map the snake parameters into the oscillators of the board
  for (int i = 0; i < MPERBOARD; i++) {
    osc[i].SetO(0);                           //-- Offset is always 0
    osc[i].SetT(snake_T[ (BOARD-1)*MPERBOARD + i ]);  //-- Map the period
    osc[i].SetA(snake_A[ (BOARD-1)*MPERBOARD + i ]);   //-- Map the amplitude
    osc[i].SetPh(DEG2RAD( snake_ph[(BOARD-1)*MPERBOARD + i ] )); //-- Map the phase
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
  T1 = crawlSpeed;
  // Change the horizontal amplitude of some servos so we can 
  // achieve side moving 
  // Here we use the absolute value of the leftX value and map
  // it from 0 to 30. 0 making robot crawl completely straight and 
  // 30 making robot moving to the side completely.
  // anythng in between will have combined effects.
  Ah[1] = map(abs(leftX),0,100,0,30); 
}

// This refreshed the servos 
void refresh(){
  // leftY is -1 because that's it's neutral position
  if(leftY == -1 && leftX == 0){
   // Do nothing because in neutral position
  }
  
  // Here the left stick is in the northwest location  
  // so we don't need to reverse the servos
  if (leftY >= 0 && leftX > 0){
    for (int i = 0; i < MPERBOARD; i++){
      osc[i].refresh();
    }
  }
  
  // Here the left stick is in the northeast location 
  // so we only need to reverse the odd servos 
  if (leftY >= 0 && leftX < 0){
    for (int i = 0;i < MPERBOARD; i++){
      if (i%2 == 0){
        osc[i].refresh();
      }
      else{
        osc[i].refresh(true); 
      }
    }
  }
  // Here the left stick is in the southeast location 
  // so we need to reverse all the servos 
  if (leftY < 0 && leftX < 0){
    for (int i = 0; i < MPERBOARD; i++){
      osc[i].refresh(true); 
    }
  }
  // Here the left stick is in the southwest location 
  // so we only need to reverse the even servos 
  if (leftY < 0 && leftX > 0){
    for (int i = 0;i < MPERBOARD; i++){
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
   delay(300);
  // Attach the oscillators to the servos
  for (int i = 0; i < MPERBOARD; i++){
    osc[i].attach(servoPins[i]);
  }

  // Calculate the snake parameters for the initial gait
  globalSnake(0);
  
  // Configure the oscillators of the current board
  mapSnake();
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
        for (int i = 1; i <= TOTALBOARDS; i++){
          for (int i = 0; i < MPERBOARD; i++){
          //Serial.println("Stoping");
          osc[i].Stop();
         delay(50); 
      }
        }
        robotIsOn = false;
      }
      else {
        for (int i = 1; i <= TOTALBOARDS; i++){
          for (int i = 0; i < MPERBOARD; i++){
          //Serial.println("Starting");
          osc[i].Play(); 
          delay(50);
      }
        }
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
        globalSnake(1);
        mapSnake();
        refresh();
        delay(50);
      }
      
      if (controlMode == CAMERAMODE){
       
      
      }
    }
    
  }
}
