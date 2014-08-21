//=====================================================================
// Project: SnakeRobot V1.0
// Description: Slave module of the Snake Robot.
// This receives the PS2 controller data from the master modules, and
// the movements.
// 
// Compiled with Arduino Version 1.0.5 
// Date: 8/8/2014
//
// Programmer: Peter Leng
// Starting template used from the ArduSnake gait example py6.ino
// By: Juan Gonzalez-Gomez (Obijuan)
// MIT Licence
// =====================================================================



// Libraries
#include <Servo.h>
#include <Oscillator.h>
#include <Wire.h>
// Constants
#define TOTALBOARDS 4 //Total Number of boards
#define MPERBOARD 4 //Modules per board
#define BOARD 1 //Specify which board(Arduino) you are uploading to
#define TOTALMODS TOTALBOARDS*MPERBOARD // Total modules in the snake  

// Different periods for the oscillations (ms)
#define T0  8000
#define T1  1400
#define T2  3000

// ======================================================================

// Servo pins for different modules. The pins here needs to be in order.
const int servoPins[MPERBOARD] = {1,2,3,4};

// --                   0    1    2     3    4    5
const int T[] =       {T0,  T1,  T1,   T2,   T2,  T0};
const int Av[] =      {0,   40, -40,   20,   20,   80};
const int Ah[] =      {0,   0,   0,    40,   40,   80};
const int phd_v[] =   {0,   120, 120,  120,  120,  0};
const int phd_h[] =   {0,   0,   0,    120,  60,   0};
const int phd_vh[] =  {0,   0,   0,    0,    90,   90};
const int ph_ini[] =  {0,   0,   0,    0,    0,    180};

// Description of the locomotion gaits
//--  0: Robot is turned off
//--  1: Linear progression (forward)
//--  2: Linear progression (backwards)
//--  3: Moving sideways
//--  4: Rotating
//--  5: Rolling

// Number of elements in the sequence
int seq_size = sizeof(Av)/sizeof(int);
int seq = 0;  //Sequence counter

// Declare the oscillators controlled by the current board
Oscillator osc[MPERBOARD];

// Parameters of the global snake. They are calculated from the 
// global parameters Av, Ah, phd_v, phd_h and ph_ini
int snake_A[TOTALMODS];   //-- Amplitude
int snake_ph[TOTALMODS];  //-- Phase
int snake_T[TOTALMODS];   //-- Period

// PS2 Controller stick positions
static byte leftY;
static byte leftX;
static byte rightY;
static byte rightX;

// Get the global snake parameters from the current gait
void global_snake(int seq){
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
void map_snake(){
  // Map the snake parameters into the oscillators of the board
  for (int i = 0; i < MPERBOARD; i++) {
    osc[i].SetO(0);                           //-- Offset is always 0
    osc[i].SetT(snake_T[ (BOARD-1)*MPERBOARD + i ]);  //-- Map the period
    osc[i].SetA(snake_A[ (BOARD-1)*MPERBOARD + i ]);   //-- Map the amplitude
    osc[i].SetPh(DEG2RAD( snake_ph[(BOARD-1)*MPERBOARD + i ] )); //-- Map the phase
  }
}

void receiveEvent(){
  if (wire.available){
    if (wire.read == "S"){
      // Has to read in this order
      leftY = wire.read;
      leftX = wire.read;
      rightY = wire.read;
      rightX = wire.read;
    }
    if (wire.read == "I"){
      for (int i = 0; i < MPERBOARD; i++)
         osc[i].Play(); 
    }
    if (wire.read == "O"){
      for (int i = 0; i < MPERBOARD; i++)
         osc[i].Stop(); 
    }
  }
}

void performChanges(){
  // Stick value    0-255
  // The stick is neutral at 127
  if (leftY == 127) {
    global_snake(0);
    map_snake();
  }
  // The stick value is negative when moved up
  else if (leftY < 0){
    global_snake(1);
    map_snake();
  }
  else{
    global_snake(2);
    map_snake();
  }
}

// ======================================================================
// Setup
// ======================================================================

void setup(){
  // Begin the I2C interface
  wire.begin(BOARD);
  // Attach the oscillators to the servos
  for (int i = 0; i < MPERBOARD; i++){
    osc[i].attach(servoPins[i]);
  }

  // Calculate the snake parameters for the initial gait
  global_snake(0);
  
  // Configure the oscillators of the current board
  map_snake();
}

// ======================================================================
// Loop
// ======================================================================

void loop(){
  // Refresh the oscillators
  for (int i = 0; i < MPERBOARD; i++)
    osc[i].refresh(); 
 
  
  // Received data from master so go to this function
  wire.onReceive(receiveEvent);
  
  performChanges();
    
}


