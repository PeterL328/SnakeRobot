//--------------------------------------------------------------
//-- Locomotion of the PYP (Pitch-Yaw-Pitch) minimal configuration
//-- The robot consists of multiply modules. 
//-- The even numbered modules are the pitch and the odd numbered modules are the yaw.
//--
//--  When the button is pressed, the locomotion gaits is changed
//--------------------------------------------------------------
//-- (c) Juan Gonzalez-Gomez (Obijuan), Feb-2012  
//-- Updated by Peter Leng and Michael Lam, July/12/2014
//-- GPL license
//--------------------------------------------------------------
#include <Servo.h>
#include <Oscillator.h>

//-- Only needed if using the skymega board
#include "skymega.h"

//-- Declare two oscillators
Oscillator osc[8];

//-- Different periods for the oscillations
const int T0 = 4000;
const int T1 = 1400;

//--                   0    1   2     3    4
const int T[] =       {T0, T1,  T1,  T1,  T0 };
const int Av[] =      {0,  40,  20,  30,  80};
const int Ah[] =      {0,   0,  20,  30,  80};
const int phd_v[] =   {0,  120, 0,   180, 0};
const int phd_h[] =   {0,   0,  90,   90, 90};
const int phd_ini[] = {0,   0,  0,    0, 180};

//-- Description of the locomotion gaits
//--  0: No locomotion. The robot is stopped. No oscillations (because the amplitude is 0)
//--  1: Moving in a straight line. Only the motors at the ends are oscillating
//--  2: Moving sideways
//--  3: Rotating
//--  4: Rolling

//-- Number of elements in the sequence
int seq_size = sizeof(Av)/sizeof(int);
int seq = 0;  //-- Sequence counter

//-- Servo pin numbers
int servos[] = {13,12,11,10,9,8,7,6};
int numServo = 8;


//////////////////////////////////////////////////////////////////////////////////
void setup()
{
  //-- Attach the oscillators to the two servos
  //-- For arduino, you can use the pin number instead of SERVO2
  
  for (int i = 0; i < numServo; i++){
  osc[i].attach(servos[i]);
  }   
  // Test LED. For arduino uno use the pin 13
  pinMode(LED, OUTPUT);      
  
  //-- Test button. For arduino uno use a pin number
  pinMode(BUTTON, INPUT);
  
  //-- Activate the button pull-up resistor
  digitalWrite(BUTTON, HIGH); 
  

  //-- Configure the oscillators
  //-- Oscillators
  
  //-- Different offsets for each servo
  int phaseDiff[] = {0,phd_h[1],phd_v[1],2*phd_h[1],2*phd_v[1],3*phd_h[1],3*phd_v[1]};
  //-- Different amplitudes for each servo
  int amplitude[] = {Av[1],Ah[1],Av[1],Ah[1],Av[1],Ah[1],Av[1],Ah[1]};
  
  // Configure each servo with it's own phi difference, amplitude and period.
  for (int i = 0;i< 8;i++){
    osc[i].SetPh(DEG2RAD(phd_ini[1]) + phaseDiff[i]);
    osc[i].SetO(0);
    osc[i].SetA(amplitude[i]);
    osc[i].SetT(T[1]);
  }
}


//-- This function returns true when the test button
//-- is pressed
bool button_clicked()
{
  
  static int button=1;
  static int oldbutton=1;
  
  //-- Save the previous button state
  oldbutton=button;
  
  // read the button
  button = digitalRead(BUTTON);

  //-- Detect a change in the button
  if (button!=oldbutton) {
    delay(20); //-- for debouncing
    if (button==PRESSED)
      return true;
  } 
  
  return false;
}
////////////////////////////////////////////////////////////////////////////////
void loop()
{
  //-- Refresh the oscillators
  for (int i=0; i<8; i++)
    osc[i].refresh(); 
    

}




