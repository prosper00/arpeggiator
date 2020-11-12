#include <TimerOne.h>     //Needed to call our interrupt timer every n seconds
#include "engine.h"       //arpeggiator engine functions
#include <U8x8lib.h>      //Needed to drive our OLED
#include "lib/MIDI.h"         //Handle MIDI IO
//#include "lib/NoodleSynth.h"  //For tone synthesis
#include "the_synth/synth.h"
#include "arpeggiator.h"  //Our defines, globals, macros, etc.

void setup()
{
  noInterrupts();
//Initialize GPIOs  
  pinMode(LEDPin,  OUTPUT);
  for(int i=0; i<7; i++){
    pinMode(buttons[i], INPUT_PULLUP);
  }
  pinMode(OLEDSDA, OUTPUT);
  pinMode(OLEDSCA, OUTPUT);
  digitalWrite(OLEDSDA,0);
  digitalWrite(OLEDSCA,0);

  setupADC();

//Initialize libraries
  a.midibegin();
  u8x8.begin();
  u8x8.setPowerSave(0);
  Timer1.initialize(100000);          // initialize timer1
  Timer1.attachInterrupt(readPoties); // We will read potis and buttons values within timer interrupt
  interrupts();
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
    updateDisplay();
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
  switch(a.modenum){
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

void readPoties(){
    a.setupArp();
}

void setupADC(){
//Initialize LGT8F ADC peripheral

/******* YOU PROBABLY SHOULDN'T DO THIS IF YOURE USING AN ATMEL CHIP INSTEAD OF THE LGT8F*******/
/******* ATMEGA may not work well with ADC clock > 1MHz, and we're using 4MHz            *******/
/******* for 1MHz, use ADCSRA = ((1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0));            *******/

/*see:
 * https://sites.google.com/site/qeewiki/books/avr-guide/analog-input
 * https://arduino.stackexchange.com/questions/45927/arduino-continuously-reading-adc-value-using-interrupt
 * https://bennthomsen.wordpress.com/arduino/peripherals/continuous-adc-capture/
 */ 

  /* *** Datasheet: ***
   * By default, the successive approximation circuitry requires an input clock frequency between 
   * 300kHz and 3MHz to get maximum resolution. If a lower conversion accurancy than 12 bits is 
   * needed, the input clock frequency to the ADC can be higher than 3MHz to get a higher sample rate.
   */
  /* Let's go for 4Mhz and see if that works, b/c no div is available to give 3Mhz, and we don't need much accuracy
   * set ADC prescale to 4-(0b010), and speed up ADC clock to F_CPU/4
   */
  ADCSRA = ((0 << ADEN) | (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)); //ADC conversion clock frequency divider to 8. ADC Clock  32MHz/8 = 4MHz
  DIDR0 = 0xFF; //Disable digital input registers on analogue inputs A0-7
  ADMUX = ((1 << REFS0) | (1 << ADLAR));   // Set Voltage reference to Avcc (5v), Left adjust converted value
  ADMUX = ((1 << ADTS0) | (1 << ADTS2));   // initialize the muxer to pin A0
  ADCSRB &= ~( (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0)); //Select free running. 15 ADC clock cycles per converson ADC sample rate 4000kHz/15 = 266kS/s / 3.75us (or 22.5us for all 6 pots)
  ADCSRA |= ((1 << ADEN) | (1 << ADIE) | (1 << ADATE)); //Turn on ADC, Enable interrupts, enable automatic triggering (ADIF?)
  ADCSRA |= (1 << ADSC); //start conversion
}

ISR(ADC_vect){
    uint8_t tmp;            // temp register for storage of misc data

    tmp = ADMUX;            // read the value of ADMUX register
    tmp &= 0x0F;            // AND the first 4 bits (value of ADC pin being used) 

    ADCvalue = ADCH;        // read the sensor value

    ADMUX++;                // add 1 to ADMUX to read the next pin next time
    switch(tmp){
      case 0: //octaveShift
        a.os = ADCvalue;
        break;
      case 1: //baseNote
        a.bn = ADCvalue;
        break;
      case 2: //indelay
        a.d = ADCvalue;
        break;
      case 3: //steps
        a.st = ADCvalue;
        break;
      case 4: //baseOctave
        a.bo = ADCvalue;
        break;
      case 5: //mode
        a.imode = ADCvalue;
        break;
      case 6: //order
        a.m = ADCvalue;
        ADMUX&= 0xF8;      // clear the last 4 bits to reset the mux to ADC0
        for(int i = 0; i<7; i++){ // read out our buttons
          if (!(digitalRead(buttons[i]))){
            button_pressed = true;
            ButtonVal = i+1;
            break;
          }
        }
        break;
    }
    ADCSRA &= ~(1 << ADEN); //why?
}
