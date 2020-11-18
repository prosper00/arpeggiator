//Pin defs
	//Analog pins. Note this won't change the pin assignments, you'd also need to modify ISR(ADC_vect);
		enum{
			octaveShiftpin = 0,  //A0
			baseNotepin =    1,  //A1
			indelaypin =     2,  //A2
			stepspin =       3,  //A3
			baseOctavepin =  4,  //A4
			modepin =        5,  //A5
			orderpin =       6   //A6
		};
	//Digitals
		const uint8_t buttons[7] { //pin assignments for our digital buttons
			6, // first button.  D6
			7, // second button. D7
			8, // ...
			9, //
			10,//
			11,//
			12 // sixth button. D12
		};
		#define OLEDSDA        D4
		#define OLEDSCA        D2
		#define AUDIOOUT       D3 //    pin CHB=D3 for Synth output
		#define LEDPin         5

//Globals
	arp a;
	U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(OLEDSCA, OLEDSDA);
	unsigned long time_last = 0;
	volatile uint8_t pots[7]; //raw pot values:
	volatile bool button_pressed = 0;
	volatile int ButtonVal = 1;

//Helper Macros
#define sbi(var, mask)	((var) |= (uint8_t)(1 << (mask)))
#define cbi(var, mask)	((var) &= (uint8_t)~(1 << (mask)))
   //hopefully this is a little faster than arduino map(). Only lightly optimized, we're
   //not using 'longs'
#define MAP(raw,inlo,inhi,outlo,outhi) ( (raw-inlo)*(outhi-outlo) / (inhi-inlo) + outlo )
