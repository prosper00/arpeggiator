//Pin defs

	//Analogs
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
		#define OLEDSCA        D3 // ***fixme:need to move to D2***
		#define OUT            D3 //    pin CHB=D3 for Synth output
		#define LEDPin         5

//Globals
	arp a(C, 5, 2, 6, 200, 0 /*ionian*/, 0);
	volatile bool button_pressed = 0;
	volatile int ButtonVal = 1;
	U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(OLEDSCA, OLEDSDA);
	unsigned long time_last = 0, time_now = 0;

//Helper Macros
#define sbi(var, mask)	((var) |= (uint8_t)(1 << (mask)))
#define cbi(var, mask)	((var) &= (uint8_t)~(1 << (mask)))
