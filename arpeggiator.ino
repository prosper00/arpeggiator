#include "lib/MIDI.h"     //Handle MIDI IO
#include "engine.h"       //arpeggiator engine functions
#include <U8x8lib.h>      //Needed to drive our OLED
#include "lib/synth.h"    //Our synthesizer module
#include "arpeggiator.h"  //Our defines, globals, macros, etc.

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

  scanPots(); //initialize ADC and scan the pots and buttons

//Initialize libraries
  a.midibegin();
  u8x8.begin();
  u8x8.setPowerSave(0);
}

void loop()
{
    if (button_pressed)
    {
      a.setProgression(ButtonVal-1);
      button_pressed = false;
      
      digitalWrite(LEDPin, HIGH);
      a.play();
      digitalWrite(LEDPin, LOW);
    }

    time_last = time_now;
    time_now = millis();
    unsigned long time_since = time_now - time_last;
    if(time_since > 20){ //do this every 20ms
      a.setupArp();
      updateDisplay();
      scanPots(); //Scan pot values again.
    }
}

void updateDisplay(){
  u8x8.setFont(u8x8_font_artossans8_r);
  
  u8x8.setInverseFont(1); u8x8.setCursor(0,0); u8x8.print(F("Arpeggiator 6000")); u8x8.setInverseFont(0);
  
  u8x8.setCursor(0,1); u8x8.print(F("Mode: "));
  u8x8.setCursor(3,3); u8x8.print(a.modenum);
  char buff[10];
  sprintf(buff, "%4d", a.imode);
  u8x8.drawString(5,3,buff);
  
  u8x8.setCursor(6,1);
  switch(a.modenum){    //write out the currently selected mode
    case 0:
      u8x8.print(F("ionian    "));
      u8x8.setCursor(0,2);
      u8x8.print(F("Natural Major"));
      break;
    case 1:
      u8x8.print(F("dorian    "));
      u8x8.setCursor(0,2);
      u8x8.print(F("Minor        "));
      break;
    case 2:
      u8x8.print(F("phrygian  "));
      u8x8.setCursor(0,2);
      u8x8.print(F("Minor        "));
      break;
    case 3:
      u8x8.print(F("lydian    "));
      u8x8.setCursor(0,2);
      u8x8.print(F("Major        "));
      break;
    case 4:
      u8x8.print(F("mixolydian"));
      u8x8.setCursor(0,2);
      u8x8.print(F("Major        "));
      break;
    case 5:
      u8x8.print(F("aeolian   "));
      u8x8.setCursor(0,2);
      u8x8.print(F("Natural Minor"));
      break;
    case 6:
      u8x8.print(F("harmonic  "));
      u8x8.setCursor(0,2);
      u8x8.print(F("---          "));
      break;
    case 7:
      u8x8.print(F("locrian   "));
      u8x8.setCursor(0,2);
      u8x8.print(F("Minor*       "));
      break;
  }
  u8x8.refreshDisplay();
}

void scanPots(){
//Initialize ADC peripheral, and scan all 7 channels
/* ***************************************************************************************************************
 * We're setting up the ADC with a prescaler of 8. This gives us a clock of 4MHz on a 32MHz LGT8F chip.
 * According to the datasheet, up to 3MHz is OK for full resolution, but we're only using 10 bits, so
 * the slightly faster rate seems to work OK. Might not work well on an ATmega, but, setting a prescaler of 
 * 32 shouldn't break anything.
 * 
 * At a prescaler value of 8, we get:
 * ADCLK=F_CPU/8 = 4MHz
 * 15 cycles per conversion, = 267k conversions/second (each conversion = 1 interrupt)
 * 267k conversions / 7 potentiometers = 38095 scans per second
 * 1/38095 = 26.25us to scan all 7 pots.
 * 
 * After scanning all 7 pots, we disable the ADC so that it's not generating unnecessary interrupts
 * Don't forget to re-enable it next time you want to scan the pots (easiest way is to call setupADC() again
 ******************************************************************************************************************/

  ADCSRA = ((1 << ADPS0) | (1 << ADPS1) | (0 << ADPS2));   // prescaler to 8. Might need a bigger value on atmega
  ADMUX = (0 | (1<<REFS0) | 1<<ADLAR);   // Set Voltage reference to Avcc (5v), starting at A0, left-aligned.
  DIDR0 = 0x1F; //Disable digital input registers on analog inputs A0-A5 
  ADCSRB &= ~( (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0)); //Select free running conversion.
  ADCSRA |= ((1 << ADEN) | (1 << ADIE) | (1 << ADATE)); //Turn on ADC, Enable interrupts, enable automatic triggering
  ADCSRA |= (1 << ADSC); //start conversion
}

ISR(ADC_vect){
    uint8_t tmp;
    tmp = ADMUX-1;          // read the value of previous ADMUX register
    tmp &= 0x0F;            // AND the first 4 bits (value of ADC pin being used) 
    ADMUX++;                // add 1 to ADMUX to read the next pin next time
    switch(tmp){  //Update the raw value of the pot that we just read
      case octaveShiftpin:
        a.os = ADC>>6; //>>6 means we only keep 10-bits of data. For atmega compatibility. 
        return;
      case baseNotepin:
        a.bn = ADC>>6;
        return;
      case indelaypin:
        a.d = ADC>>6;
        return;
      case stepspin:
        a.st = ADC>>6;
        return;
      case baseOctavepin:
        a.bo = ADC>>6;
        return;
      case modepin:
        a.imode = ADC>>6;
        return;
      case orderpin:
        for(int i = 0; i<7; i++){ // read out our buttons
          if (!(digitalRead(buttons[i]))){
            button_pressed = true;
            ButtonVal = i+1;
            break;
          }
        }
        a.m = ADC>>6;
        ADCSRA &= ~(1 << ADEN); /* We've read all pots. Disable ADC. We need to re-enable 
                                 * it via scanPots() next time we want to scan the pots */
        return;
    }
}
