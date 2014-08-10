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


// =====================================================================
//Constants that can be changed for you needs

//Total Number of board
const int BOARDS = 4;

//Modules per board
const int M = 4;

//Board number (1 - BOARDS). Generate the firmware to download in the slave boards
const int board = 1;

//Servo pins for different board. Change pins here NEED to be in order
const int servoPins[M] = {1,2,3,4};


// ======================================================================
//Different periods for the oscillations (ms)
const int T0 = 8000;
const int T1 = 1400;
const int T2 = 3000;

// --                   0    1    2     3    4
const int T[] =       {T0,  T1,  T2,   T2,  T0};
const int Av[] =      {0,   40,  20,   20,  80};
const int Ah[] =      {0,   0,   40,   40,  80};
const int phd_v[] =   {0,   120, 120,  120,  0};
const int phd_h[] =   {0,   0,   120,  60,   0};
const int phd_vh[] =  {0,   0,   0,    90,  90};
const int ph_ini[] =  {0,   0,   0,    0,   180};

//Description of the locomotion gaits
//--  0: Robot is turned off
//--  1: Linear progression
//--  2: Moving sideways
//--  3: Rotating
//--  4: Rolling

//Total number of modules in the snake
const int totalNumMod = M * BOARDS;

//Number of elements in the sequence
int seq_size = sizeof(Av)/sizeof(int);
int seq = 0;  //Sequence counter

//Declare the oscillators controlled by the current board
Oscillator osc[M];

//Parameters of the global snake. They are calculated from the 
//global parameters Av, Ah, phd_v, phd_h and ph_ini
int snake_A[totalNumMod];   //-- Amplitude
int snake_ph[totalNumMod];  //-- Phase
int snake_T[totalNumMod];   //-- Period

//Get the global snake parameters from the current gait
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

// ======================================================================
// Setup
// ======================================================================

void setup()
{
  Serial.begin(57600);
  //-- Attach the oscillators to the servos
  for (int i = 0; i < M; i++){
    osc[i].attach(servoPins[i]);
  }

  //-- Calculate the snake parameters for the initial gait
  global_snake(0);
  
  //-- Configure the oscillators of the current board
  map_snake();
}

// ======================================================================
// Loop
// ======================================================================

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


