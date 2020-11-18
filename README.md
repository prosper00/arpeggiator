# arpeggiator
Arduino MIDI arpeggiator, based on original work found here: https://github.com/raszhivin/arpeggiator

![Prototype](20201110_152001.jpg?raw=true "Prototype")
![Prototype Back](20201114_165149.jpg?raw=true "Back Side")


## Done:
- built up the circuit on a protoboard
- bugfixes
- tweaked some preferences and pin assignments
- modified for use with an LGT8F328p LQFP32 board (but should work with a Nano as well). 
- Added support for an OLED display (courtesty of the superb u8g2 library: https://github.com/olikraus/u8g2/wiki)
- ADC tweaks to speed up reading the pots - handle this task asynchronously in an interrupt.
- Required libraries: MIDI Library by Forty Seven Effects // u8g2 by olikraus
- Eliminate the use of TimerOne (we'll need the timer1 peripheral for our synth)
- Setup the ADC for continuous operation, and generate an interrupt each time it samples a value. Also use this same ISR to read our buttons.
- Set up a crude state machine and timer interrupt for handling timesharing. 
- move setupArp() out of the library and into main - parsing pots is UI stuff. The library should only expect the processed values.
- Re-wired the OLED to use D2 instead of D3 (our Synth needs D3 as its output)
- Quick and dirty amplifier onboard with a TDA7056B.
- Fixed amplifier wiring. Implemented volume control and a ~5kHz LPF. Getting some hiss though

## Todo:
- Make the code and library non-blocking, timer/state machine based (remove the use of delay() in engine.cpp )
- Polyphonic output options... incorporate up to ~4 simultaneous voices
- Implement an on-board synthesizer to generate our own tones (thinking of ripping off https://github.com/dzlonline/the_synth )
- Still some bugs in ADC ISR - something timing-related is screwing with the muxer, and making it difficult to tell which reading is coming from which pin, especially if it's left free-running (I think the ISR is taking too many cycles. Using digitalRead is certainly to blame. Need to switch to port reads in the ISR)
- Updating the display takes 80ms! Can I speed this up? Do I need to? May not be an issue if it's interruptable, and the important parts of the engine/synth are interrupt-driven. Using hardware i2c is probably not an option, as SDA/SCL are analog inputs
- Amplifier improvements - fix volume control knob, add a LPF to kill the PWM frequencies

## Aspirational Todo (ideas):
- Some form of filtering on board, probably incorporating a CD4051 and opamp. Maybe a programmable vibrato/reverb/chorus/distortion?
- ADSR Envelope control (in hardware)? (https://github.com/katspaugh/arduino-modular-synth/tree/master/ADSRduino)
- Create/upload schematics
- STM32 port? (do I need more i/o's and processor power?)
- karplus-strong string synth (https://github.com/katspaugh/arduino-modular-synth/tree/master/arduino-karplus-strong)
- Understand and embrace Mozzi for synthesis?
