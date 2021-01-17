/**
 * An example of C/MRI inputs and outputs
 * ======================================
 * Sets up pins 2-5 as an outputs with LEDs, and pins 6-9 as inputs with pullups.
 * 
 * 1: Set up a JMRI connection, see hello_world, steps 1-4
 * 2: Open Tools > Tables > Lights and click 'Add'
 * 3: Add a new light at hardware address 1, then click 'Create'.
 * 4: Repeat for hardware address 2, 3, 4 and close the window. Ignore the save message.
 * 5: Click on 'Sensors' and set up new sensors for hardware address 1, 2, 3 and 4.
 * 6: You'll notice the TX and RX LEDs burst into life. This is JMRI polling the state of our sensors.
 * 7: Ground pin 6, you'll see sensor #1 go Active, while the rest are Inactive.
 * 8: Switch to Lights and play around with the State buttons. Congratulations!
 * 
 * Debugging:
 * Open the CMRI > CMRI Monitor window to check what is getting sent and received.
 * With 'Show raw data' turned on the output looks like:
 *    [41 50]  Poll ua=0
 *    [41 52 01 00 00]  Receive ua=0 IB=1 0 0 
 * 
 * 0x41 = 65 = A = address 0
 * 0x50 = 80 = P = poll, i.e. PC asking C/MRI to transmit its state back to PC
 * 
 * 0x41 = 65 = A = address 0
 * 0x52 = 82 = R = receive, i.e. PC receiving state data from C/MRI 
 * 0x01 = 0b00000001 = 1st bit is high
 * 0x00 = 0b00000000 = all other bits off
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <CMRI.h>
#include <Auto485.h>

#define DE_PIN 2
#define numServos 1 //The number of servos connected

// Define the PCA9685 board addresses
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); //setup the board address - defaults to 0x40 if not specified

// Setup serial communication
Auto485 bus(DE_PIN); // Arduino pin 2 -> MAX485 DE and RE pins

//Define CMRI connection 24 inputs and 48 outputs
CMRI cmri0(0, 24, 48, bus); // first SMINI with address 0. SMINI = 24 inputs, 48 outputs
CMRI cmri1(1); //Second SMINI with adress 1. SMINI = 24 inputs, 48 outputs

//table to hold servodata
int Status[numServos]; //Create a table to hold the status of each turnout, signal, etc.
int Throw[numServos]; //Create a table to hold the throw value for each servo
int Close[numServos]; //Create a table to hold the close value for each servo

void setup() {
  Serial.begin(19200); // make sure this matches your speed set in JMRI
  bus.begin(19200);


  
  // set up pins for input och output 2-5 outpu, 6-9 input
  for (int i=2; i<=5; i++) { pinMode(i, OUTPUT); }
  for (int i=6; i<=9; i++) { pinMode(i, INPUT); digitalWrite(i, HIGH); }


  //Initialise PCA9685 board
  pwm.begin();
  pwm.setPWMFreq(50);

  //Servo connection 0 - point motor. duplicate for more servors and change number if not throwing. range 1000-2000
  Throw[0] = 1300;
  Close[0] = 1700;
  
}
char c;
void loop() {
  // 1: build up a packet
  while (Serial.available() > 0)
  {
    c = Serial.read();
    cmri0.process_char(c);
    cmri1.process_char(c);
  }
 

  //Servos
  for(int i = 0; i < numServos; i++) {
    Status[i] = (cmri0.get_bit(i));
    if (Status[i] == 1){
      pwm.writeMicroseconds(i, Throw[i]);
    }
    else {
      pwm.writeMicroseconds(i, Close[i]);
    }
  }
  
  //Outputs
  //comment out or delete lines not required to speed up
  // 2: update outputs
  digitalWrite(2, cmri1.get_bit(0));
  digitalWrite(3, cmri1.get_bit(1));
  //digitalWrite(4, cmri1.get_bit(2));
  //digitalWrite(5, cmri1.get_bit(3));

  //Inputs
  //comment out or delete lines not required to speed up
  // 3: update inputs (invert digitalRead due to the pullups)
  cmri1.set_bit(0, !digitalRead(6));
  //cmri1.set_bit(1, !digitalRead(7));
  //cmri1.set_bit(2, !digitalRead(8));
  //cmri1.set_bit(3, !digitalRead(9));
}
