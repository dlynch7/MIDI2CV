// sources: http://www.notesandvolts.com/2012/01/fun-with-arduino-midi-input-basics.html
// https://sourceforge.net/projects/arduinomidilib/?source=typ_redirect
// 

#include <MIDI.h>  // Add Midi Library
#include <Wire.h>//Include the Wire library to talk I2C
#include <Adafruit_MCP4725.h>

//This is the I2C Address of the MCP4725, by default (A0 pulled to GND).
//Please note that this breakout is for the MCP4725A0. 
#define MCP4725_ADDR 0x60   
//For devices with A0 pulled HIGH, use 0x61

Adafruit_MCP4725 dac;

#define LED 13    // Arduino Board LED is on Pin 13
#define GATE 12 // Use Pin 12 to output GATE signal
#define GATECHK 1 // Use Pin A1 to read the voltage of the GATE signal, used for debugging
#define VEL 6 // Use Pin 6 to output velocity signal (velVal), pwm frequency 960 Hz (?)

//int pitches[61] = 
//{
//     0,   68,  137,  205,  273,  341,  410,  478, 
//   546,  614,  683,  751,  819,  887,  956, 1024, 
//  1092, 1161, 1229, 1297, 1365, 1434, 1502, 1570, 
//  1638, 1707, 1775, 1843, 1911, 1980, 2048, 2116, 
//  2185, 2253, 2321, 2389, 2458, 2526, 2594, 2662, 
//  2731, 2799, 2867, 2935, 3004, 3072, 3140, 3209, 
//  3277, 3345, 3413, 3482, 3550, 3618, 3686, 3755, 
//  3823, 3891, 3959, 4028, 4096
//};

int pitches[61] = 
{
     0,   70,  142,  214,  286,  358,  419,  500, 
   571,  642,  713,  786,  860,  933, 1006, 1080, 
  1153, 1226, 1299, 1372, 1445, 1518, 1591, 1664, 
  1737, 1808, 1880, 1952, 2024, 2096, 2168, 2240,
  2310, 2380, 2452, 2525, 2600, 2670, 2740, 2810,
  2880, 2950, 3020, 3090, 3160, 3230, 3300, 3370,
  3440, 3510, 3580, 3650, 3720, 3790, 3860, 3930,
  4000, 4070
};

// Below is my function that will be called by the Midi Library
// when a MIDI NOTE ON message is received.
// It will be passed bytes for Channel, Pitch, and Velocity
void MyHandleNoteOn(byte channel, byte pitch, byte velocity) {
  int DACnum = 0;
  int LEDval = LOW;
  int GATEval = LOW;
  int gateVoltage = 0; // used for debugging
  int velVal = 0;
  
  DACnum = pitches[pitch-36]; //-36 to transpose bottom of CASIO keyboard to C0.
  dac.setVoltage(DACnum, false);
  //Serial.print("channel: ");
  //Serial.print(channel);
  LEDval = HIGH;
  GATEval = HIGH;
  velVal = 2*velocity;
//  velVal = HIGH;
  if (velocity == 0) {//A NOTE ON message with a velocity = Zero is actualy a NOTE OFF
    LEDval = LOW;
    GATEval = LOW;
//    velVal = LOW;
    velVal = 0;
    //digitalWrite(LED,LOW);//Turn LED off
    //digitalWrite(GATE,LOW);//Turn LED off
    // DO SOMETHING FOR TRIGGER=OFF AND GATE=OFF;
  }
  digitalWrite(LED,LEDval);  //Turn LED on
  digitalWrite(GATE,GATEval);  //Turn GATE on
  analogWrite(VEL,velVal); // Write velocity signal
//  digitalWrite(VEL,velVal); // Write velocity signal
//  gateVoltage = analogRead(GATECHK); // voltage of GATE signal, used for debugging
  //gateVoltage = digitalRead(3); // voltage of GATE signal, used for debugging
  
  // Print information
  Serial.print(", pitch: ");
  Serial.print(pitch-36); //-36 to transpose bottom of CASIO keyboard to C0.
  Serial.print(", DACnum: ");
  Serial.print(DACnum);
  Serial.print(", velocity: ");
  Serial.print(velocity);
  Serial.print(", gate: ");
  Serial.print(GATEval);
  Serial.print(", velVal: ");
  Serial.println(velVal);
  Serial.print(", velVoltage: ");
  gateVoltage = analogRead(GATECHK); // voltage of GATE signal, used for debugging
  Serial.println(gateVoltage);
}

void setup() {
  pinMode(LED, OUTPUT); // Set LED pin to output
  pinMode(GATE, OUTPUT); // Set GATE pin to output
  pinMode(VEL, OUTPUT); // Set GATE pin to output
  MIDI.begin(MIDI_CHANNEL_OMNI); // Initialize the Midi Library.
// OMNI sets it to listen to all channels.. MIDI.begin(2) would set it
// to respond to channel 2 notes only.
  MIDI.setHandleNoteOn(MyHandleNoteOn); // This is important!! This command
  // tells the Midi Library which function I want called when a Note ON command
  // is received. in this case it's "MyHandleNoteOn".
  Wire.begin();
  dac.begin(0x60);
}

void loop() { // Main loop
  MIDI.read(); // Continually check what Midi Commands have been received.
}
