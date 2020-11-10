
#include <TimerOne.h>
#include "engine.h"

#include <U8x8lib.h>


arp a(C, 5, 2, 6, 200, c_harmonic, 0);
volatile bool button_pressed = 0;
volatile int ButtonVal =1;

#define octaveShiftpin A0
#define baseNotepin A1
#define indelaypin A2
#define stepspin A3
#define baseOctavepin A4
#define modepin A5
#define orderpin A6
#define OLEDSDA D4
#define OLEDSCA D3

#define LEDPin 5

U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(OLEDSCA, OLEDSDA);


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

    if (!(digitalRead(6))) {
      button_pressed = true;
      ButtonVal = 1;
      return;
    }
    if (!(digitalRead(7))) {
      button_pressed = true;
      ButtonVal = 2;
      return;
    }
    if (!(digitalRead(8))) {
      button_pressed = true;
      ButtonVal = 3;
      return;
    }
    if (!(digitalRead(9))) {
      button_pressed = true;
      ButtonVal = 4;
      return;      
    }
    if (!(digitalRead(10))) {
      button_pressed = true;
      ButtonVal = 5;
      return;
    }
    if (!(digitalRead(11))) {
      button_pressed = true;
      ButtonVal = 6;
      return;
    }
    if (!(digitalRead(12))) {
      button_pressed = true;
      ButtonVal = 7;
      return;
    }
}

void setup()
{
  analogReference(DEFAULT);
  analogReadResolution(10);
  
  a.midibegin();
 
  // LED pin
  pinMode(LEDPin, OUTPUT);
  
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
    
  button_pressed = false;
 
  Timer1.initialize(20000);          // initialize timer1, and set a 1/10 second period
  Timer1.attachInterrupt(readPoties); // We will read potis and buttons values within timer interrupt

  pinMode(OLEDSDA,OUTPUT);
  pinMode(OLEDSCA,OUTPUT);
  digitalWrite(OLEDSDA,0);
  digitalWrite(OLEDSCA,0);
  u8x8.begin();
  u8x8.setPowerSave(0);
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
  u8x8.drawString(0,0,"Arpeggiator 6000");
/*  switch(a.mode){
    case 0:
      u8x8.drawString(0,1,"ionian");
      break;
    case 1:
      u8x8.drawString(0,1,"dorian");
      break;
    case 2:
      u8x8.drawString(0,1,"phrygian");
      break;
    case 3:
      u8x8.drawString(0,1,"lydian");
      break;
    case 4:
      u8x8.drawString(0,1,"mixolydian");
      break;
    case 5:
      u8x8.drawString(0,1,"aeolian");
      break;
    case 6:
      u8x8.drawString(0,1,"harmonic");
      break;
    case 7:
      u8x8.drawString(0,1,"locrian");
      break;
  }*/
  u8x8.refreshDisplay(); 

}
