//--------------------------------------------------------------
//-- Locomotion of the pith-yaw-12 configuration
//-- The robot consist of 12 modules in total, 6 are pitching and 6 yawing
//-- The structure is P - Y - P - Y - P - Y ...
//---------------------------------------------------------------
//-- There are 4 boards controlling the robot, each one in charge of
//--  4 modules:
//--  Group 1-4:  P  -  Y  -  P  - Y(board 1-4)
//----------------------------------------------------------------
//-- Changing the constant BOARDS and M it can be used for longer
//-- snakes
//--------------------------------------------------------------
//-- (c) Juan Gonzalez-Gomez (Obijuan), Jul-2013
//-- Heavily modified by Peter Leng Aug-2014
//-- GPL license
//--------------------------------------------------------------
//-- Library
#include <Servo.h>
#include <Oscillator.h>


//----------------------------------------------------------------------
//-- Constants that can be changed for you needs

//-- Total Number of board
const int BOARDS = 4;

//-- Modules per board
const int M = 4;

//-- Board number (1 - BOARDS). Generate the firmware to download in the board
const int board = 1;

//-- Servo pins for different board. Change pins here NEED to be in order
const int servoPins[M] = {1,2,3,4};


//-----------------------------------------------------------------------

//-- Different periods for the oscillations (ms)
const int T0 = 8000;
const int T1 = 1400;
const int T2 = 3000;

//--                   0    1    2     3    4
const int T[] =       {T0,  T1,  T2,   T2,  T0};
const int Av[] =      {0,   40,  20,   20,  80};
const int Ah[] =      {0,   0,   40,   40,  80};
const int phd_v[] =   {0,   120, 120,  120,  0};
const int phd_h[] =   {0,   0,   120,  60,   0};
const int phd_vh[] =  {0,   0,   0,    90,  90};
const int ph_ini[] =  {0,   0,   0,    0,   180};

//-- Description of the locomotion gaits
//--  0: Stopped
//--  1: Moving in a straight line
//--  2: Moving sideways
//--  3: Rotating
//--  4: Rolling

//-- Total number of modules in the snake
const int totalNumMod = M * BOARDS;

//-- Number of elements in the sequence
int seq_size = sizeof(Av)/sizeof(int);
int seq = 0;  //-- Sequence counter

//-- Declare the oscillators controlled by the current board
Oscillator osc[M];

//-- Parameters of the global snake. They are calculated from the 
//-- global parameters Av, Ah, phd_v, phd_h and ph_ini
int snake_A[totalNumMod];   //-- Amplitude
int snake_ph[totalNumMod];  //-- Phase
int snake_T[totalNumMod];   //-- Period

//-- Get the global snake parameters from the current gait
void global_snake(int seq)
{
  //-- Calculate the global snake
  for (int i = 0; i< totalNumMod; i++) {
    
    //-- Even modules:
    if (i % 2 == 0) {
      snake_A[i] = Av[seq];
      
      snake_ph[i] = ph_ini[seq] + (i/2) * phd_v[seq];
    }
    //-- Odd modules
    else {
      snake_A[i] = Ah[seq];
      snake_ph[i] = ph_ini[seq] + phd_vh[seq] + (i-1)/2 * phd_h[seq];
    }
    
    //-- The period is the same for all the modules
    snake_T[i] = T[seq];
    
  }
}

//-- Map the snake parameters into the board oscillators
void map_snake()
{
  //-- Map the snake parameters into the oscillators of the board
  for (int i = 0; i < M; i++) {
    osc[i].SetO(0);                           //-- Offset is always 0
    osc[i].SetT(snake_T[ (board-1)*M + i ]);  //-- Map the period
    osc[i].SetA(snake_A[ (board-1)*M + i ]);   //-- Map the amplitude
    osc[i].SetPh(DEG2RAD( snake_ph[(board-1)*M + i ] )); //-- Map the phase
  }
}


void setup()
{
  //-- Attach the oscillators to the servos
  for (int i = 0; i < M; i++){
    osc[i].attach(servoPins[i]);
  }

  //-- Calculate the snake parameters for the initial gait
  global_snake(0);
  
  //-- Configure the oscillators of the current board
  map_snake();
}

void loop()
{
  //-- Refresh the oscillators
  for (int i = 0; i < M; i++)
    osc[i].refresh(); 
    
    
    /* For Changing gaits
    
    //-- Point to the next sequence
    seq = (seq + 1) % seq_size;
    
    //-- Calculate the snake parameters for the initial gait
    global_snake(seq);
  
    //-- Configure the oscillators of the current board
    map_snake();
    
    */    
}


