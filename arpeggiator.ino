#include "engine.h"       //arpeggiator engine functions
#include <U8x8lib.h>      //Needed to drive our OLED
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

//initialize ADC and scan the pots and buttons
  scanPots();

//Initialize libraries
  a.midibegin();
  a.synthbegin();
  u8x8.begin();
  u8x8.setPowerSave(0);
  
/* Initialize Timer0 interrupts every 1ms. 
  // timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" ISR*/
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void loop()
{
  /*unsigned long time_now = millis();
  if((time_now - time_last) > 400){ //do this every 200ms
    time_last = time_now;
    updateDisplay(); //this takes 80ms :(
  }*/
  
  if (button_pressed)
  {
    a.setProgression(ButtonVal-1);
    button_pressed = false;
    digitalWrite(LEDPin, HIGH);
    a.play();
    digitalWrite(LEDPin, LOW);
  }
}

//handle display output
void updateDisplay(){
  u8x8.setFont(u8x8_font_artossans8_r);
  
  u8x8.setInverseFont(1); u8x8.setCursor(0,0); u8x8.print(F("Arpeggiator 6000")); u8x8.setInverseFont(0);
  
  u8x8.setCursor(0,1); u8x8.print(F("Mode: "));
  u8x8.setCursor(3,3); u8x8.print(a.modenum);
  char buff[10];
  sprintf(buff, "%4d", pots[modepin]);
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

//Initialize ADC peripheral, and start scanning all 7 channels
void scanPots(){
/* ***************************************************************************************************************
 * We're setting up the ADC with a prescaler of 128. This gives us a clock of 250kHz on a 32MHz LGT8F chip.
 * According to the datasheet, up to 3MHz is OK for full resolution, but, each time it's finished scanning
 * a pin, it's going to generate an interrupt that we have to handle. Too many updates is unnecessary, and 
 * handling too many interrupts is just wasting cpu cycles.
 * 
 * At a prescaler value of 128, we get:
 * ADCLK=F_CPU/128 = 250Hz
 * 15 cycles per conversion, = 16.7k conversions/second (each conversion = 1 interrupt)
 * 16.7k conversions / 7 potentiometers = 2381 scans per second
 * 1/2381 = 420us to scan all 7 pots. Really, this is still a good 100x as much as we NEED.
 * Therefore, we're going to disable the ADC after each 7 scans in the ISR, and leave
 * the loop to re-start scanning via scanPots(); when needed.
 * 
 * At a prescaler of 32, we'd take 4 times as long to read the pots, but should be more stable on atmegas
 * 
 * After scanning all 7 pots, we disable the ADC so that it's not generating unnecessary interrupts
 * Don't forget to re-enable it next time you want to scan the pots (easiest way is to call setupADC() again
 ******************************************************************************************************************/
 
  ADCSRA = ((1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2));   // prescaler to 128
  ADMUX = (0 | (1<<REFS0) | 1<<ADLAR);   // Set Voltage reference to Avcc (5v), starting at A0, left-aligned.
  DIDR0 = 0x1F; //Disable digital input registers on analog inputs A0-A5 
  ADCSRB &= ~( (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0)); //Select free running conversion.
  ADCSRA |= ((1 << ADEN) | (1 << ADIE) | (1 << ADATE)); //Turn on ADC, Enable interrupts, enable automatic triggering
  ADCSRA |= (1 << ADSC); //start conversion
}

// Processes the raw pot readings, and sends them to the arpegiator library
void mapPots(){
  uint8_t  baseNote    = MAP(pots[baseNotepin],    0, 252, 11, 0);
  uint8_t  baseOctave  = MAP(pots[baseOctavepin],  0, 252, 7,  0);
  uint8_t  octaveShift = MAP(pots[octaveShiftpin], 0, 252, 7,  0);
  uint8_t  steps       = MAP(pots[stepspin],       0, 252, 8,  1);
  uint16_t indelay     = pots[indelaypin] * 2;
  uint8_t  modenum     = MAP(pots[modepin],        0, 252, 7,  0);
  uint8_t  order       = MAP(pots[orderpin],       0, 252, 0,  4);
  
  //pass the mapped pot values to the engine:
  a.updateArp(baseNote,baseNote,octaveShift,steps,indelay,modenum,order);
}

//ISR to handle scanning each pot sequentially
ISR(ADC_vect){
  uint8_t tmp;
  tmp = ADMUX;            // read the value of previous ADMUX register
  tmp &= 0x0F;            // AND the first 4 bits (value of ADC pin being used) 
  ADMUX++;                // add 1 to ADMUX to read the next pin next time
  pots[tmp]=ADC>>8;       // read the value of the pot we just scanned
  if(tmp==orderpin){        // if we've read the last pot
    ADCSRA &= ~(1 << ADEN); // disable ADC.
    for(int i = 0; i<7; i++){ // read out our buttons
      if (!(digitalRead(buttons[i]))){
        button_pressed = true;
        ButtonVal = i+1;
      }
    }
  }
}

// This interrupt is called once a millisecond.
ISR(TIMER0_COMPA_vect){
  static uint16_t interval=0;
  interval++;
  if(interval>=100){  //every hundredth time here, do this:
    interval=0;
    mapPots();  // parse the raw poteniometer readings and update the engine
    scanPots(); // start the ADC again, to re-scan the pots.
  }
}
