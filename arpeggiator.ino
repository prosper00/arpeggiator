#include <TimerOne.h>
#include "engine.h"
#include <U8x8lib.h>
#include <MIDI.h>
#include <NoodleSynth.h>

//Pin defs
#define octaveShiftpin A0
#define baseNotepin    A1
#define indelaypin     A2
#define stepspin       A3
#define baseOctavepin  A4
#define modepin        A5
#define orderpin       A6
#define OLEDSDA        D4
#define OLEDSCA        D3 //need to move to D2
#define OUT            D3 //pin CHB=D3 for Synth output
#define LEDPin         5
uint8_t buttons[7] { //pin assignments for our digital buttons
  6, //button0
  7, //button1
  8, //button2
  9, //button3
  10,//button4
  11,//button5
  12 //button6
};


//Globals
arp a(C, 5, 2, 6, 200, 0 /*ionian*/, 0);
volatile bool button_pressed = 0;
volatile int ButtonVal =1;
U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(OLEDSCA, OLEDSDA);

/*  
  MusicWithoutDelay voice1(rttl1);
  MusicWithoutDelay voice2(rttl2);
  MusicWithoutDelay voice3(rttl3);
*/

void setup()
{
//Initialize GPIOs  
  pinMode(LEDPin,  OUTPUT);
  for(int i=0; i<7; i++){
    pinMode(buttons[i], INPUT_PULLUP);
  }
  pinMode(OLEDSDA, OUTPUT);
  pinMode(OLEDSCA, OUTPUT);
  digitalWrite(OLEDSDA,0);
  digitalWrite(OLEDSCA,0);

//Initialize ADC
  analogReference(DEFAULT);  //Pots report a value from 0 to Vcc
  analogReadResolution(10);  //Use 10-bit precision (for boards with >10 bit ADC's_

//Initialize libraries
  a.midibegin();
  u8x8.begin();
  u8x8.setPowerSave(0);
  Timer1.initialize(20000);          // initialize timer1
  Timer1.attachInterrupt(readPoties); // We will read potis and buttons values within timer interrupt

/*
  voice1.begin(CHB, TRIANGLE, ENVELOPE0, 0);   //CHB means pin 3 on Arduino UNO, CHA means pin 11
  voice2.begin(CHB, TRIANGLE, ENVELOPE2, 0);
  voice3.begin(CHB, TRIANGLE, ENVELOPE2, 0);
*/
}

void loop()
{
    if (button_pressed)
    {
      a.setProgression(ButtonVal-1);
      button_pressed = false;
      
      // Switch on LED
      digitalWrite(LEDPin, HIGH);
      a.play();
      
      // Switch off LED
      digitalWrite(LEDPin, LOW);
    }
  
  u8x8.setFont(u8x8_font_artossans8_r);
  u8x8.setInverseFont(1);
  u8x8.drawString(0,0,(PSTR("Arpeggiator 6000")));
  u8x8.setInverseFont(0);
  u8x8.drawString(0,1,(PSTR("Mode: ")));
  u8x8.setCursor(3,3);
  u8x8.print(a.modenum);
  
  char buff[10];
  sprintf(buff, "%4d", a.raw);
  u8x8.drawString(5,3,buff);
  
  switch(a.modenum){
    case 0:
      u8x8.drawString(6,1,(PSTR("ionian    ")));
      u8x8.drawString(0,2,(PSTR("Natural Major")));
      break;
    case 1:
      u8x8.drawString(6,1,(PSTR("dorian    ")));
      u8x8.drawString(0,2,(PSTR("Minor        ")));
      break;
    case 2:
      u8x8.drawString(6,1,(PSTR("phrygian  ")));
      u8x8.drawString(0,2,(PSTR("Minor        ")));
      break;
    case 3:
      u8x8.drawString(6,1,(PSTR("lydian    ")));
      u8x8.drawString(0,2,(PSTR("Major        ")));
      break;
    case 4:
      u8x8.drawString(6,1,(PSTR("mixolydian")));
      u8x8.drawString(0,2,(PSTR("Major        ")));
      break;
    case 5:
      u8x8.drawString(6,1,(PSTR("aeolian   ")));
      u8x8.drawString(0,2,(PSTR("Natural Minor")));
      break;
    case 6:
      u8x8.drawString(6,1,(PSTR("harmonic  ")));
      u8x8.drawString(0,2,(PSTR("---          ")));
      break;
    case 7:
      u8x8.drawString(6,1,(PSTR("locrian   ")));
      u8x8.drawString(0,2,(PSTR("Minor*       ")));
      break;
  }
  u8x8.refreshDisplay(); 
}

void readPoties()
{
    int baseNote=analogRead(baseNotepin);
    int baseOctave=analogRead(baseOctavepin);
    int octaveShift=analogRead(octaveShiftpin);
    int steps=analogRead(stepspin);
    int indelay=analogRead(indelaypin);
    int order=analogRead(orderpin);
    int mode=analogRead(modepin);
    
    a.setupArp(baseNote, baseOctave, octaveShift, steps, indelay, order, mode);

    for(int i = 0; i<7; i++){
      if (!(digitalRead(buttons[i]))){
        button_pressed = true;
        ButtonVal = i+1;
        break;
      }
    }
}
