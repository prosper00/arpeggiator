//Pin defs

/* analog pins - these definitions aren't actually used, 
 * the analog inputs are hardcoded in ISR(ADC_vect)
 * #define octaveShiftpin A0
 * #define baseNotepin    A1
 * #define indelaypin     A2
 * #define stepspin       A3
 * #define baseOctavepin  A4
 * #define modepin        A5
 * #define orderpin       A6
 */
#define OLEDSDA        D4
#define OLEDSCA        D3 // ***fixme:need to move to D2***
#define OUT            D3 //    pin CHB=D3 for Synth output
#define LEDPin         5
uint8_t buttons[7] { //pin assignments for our digital buttons
  6, // first button
  7, // second button
  8, // ...
  9, //
  10,//
  11,//
  12 // sixth button
};

//Globals
arp a(C, 5, 2, 6, 200, 0 /*ionian*/, 0);
volatile bool button_pressed = 0;
volatile int ButtonVal = 1;
volatile unsigned int ADCvalue;
U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(OLEDSCA, OLEDSDA);

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
