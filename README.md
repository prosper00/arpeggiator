# arpeggiator
Arduino MIDI arpeggiator, based on original work found here: https://github.com/raszhivin/arpeggiator

![Prototype](20201110_152001.jpg?raw=true "Prototype")
![Prototype Back](20201114_165149.jpg?raw=true "Back Side")
![Prototype Speaker](20201117_184812.jpg?raw=true "with speaker") 
![Schematic](arpeggiator.png?raw=true "Schematic")

### CURRENT STATUS: WORKING (monophonic only)
- Base Octave pot is not currently doing anything. 'Order' pot crashes at bottom limit.
- Base Note doesn't seem to work with waveshapes other than SQUARE?!?

## Done:
- built up the circuit on a protoboard
- tweaked some preferences and pin assignments
- modified for use with an LGT8F328p LQFP32 board (but should work with a Nano as well). 
- Added support for an OLED display (u8g2 library: https://github.com/olikraus/u8g2/wiki)
- Required libraries: MIDI Library by Forty Seven Effects // u8g2 by olikraus
- ADC tweaks to speed up reading the pots - handle this task asynchronously in an interrupt.
- Eliminate the use of TimerOne (we'll need the timer1 peripheral for our synth)
- Setup the ADC for continuous operation, and generate an interrupt each time it samples a value. Also use this same ISR to read our buttons.
- Set up a crude state machine and timer interrupt for handling timesharing. 
- move setupArp() out of the library and into main - parsing pots is UI stuff. The library should only expect the processed values.
- Re-wired the OLED to use D2 instead of D3 (our Synth needs D3 as its output)
- Quick and dirty amplifier onboard with a TDA7056B.
- Fixed amplifier wiring. Implemented volume control and a ~5kHz LPF. Getting some hiss though
- Drew up a schematic
- Incorporated 'synth' library to generate the tones. Not working correctly. Need to troubleshoot
- created a Makefile. Depends on an arduino-cli tool, and arduino environment. 'make' to compile, 'make flash' to upload. See contents for comments.
- Tidied things up, killed a bunch of cosmetic compiler warnings. Added #defines in engine.cpp to select the output method.
- Amplifier improvements - fix volume control knob, add a LPF to kill the PWM frequencies
- Updated synth.h to work correctly at CPU clocks other than 16MHz (32MHz tested).
- Seems my voltage is fluctuating, and causing the potentiometer values to fluctuate.... fix that (add a regulator?)
- Made the synth ISR non-blocking (allow nested interrupts). Fixed glitching audio
- Set ADC prescaler back to 16, seems to help stabilize fluctuating pot values. No idea why.
- configurable MIDI, tone(), or synth output (see defines in engine.cpp)

## Todo:
- Make the code and library non-blocking, timer/state machine based (remove the use of delay() in engine.cpp )
- Polyphonic output options... incorporate up to ~4 simultaneous voices
- Still some bugs in ADC ISR - something timing-related is screwing with the muxer, and making it difficult to tell which reading is coming from which pin, especially if it's left free-running (I think the ISR is taking too many cycles. Using digitalRead is certainly to blame. Need to switch to port reads in the ISR)
- Updating the display takes 80ms! Can I speed this up? Do I need to? May not be an issue if it's interruptable, and the important parts of the engine/synth are interrupt-driven. Using hardware i2c is probably not an option, as SDA/SCL are analog inputs. Display is currently disabled, I can't afford the cycles.
- Amplifier improvements - do something about the hiss and USB power supply noise. A proper ground should help.
- Synchronize synthesize note 'length' with the tempo (indelay) value of the sequencer.

## Aspirational Todo (ideas):
- Some form of filtering on board, probably incorporating a CD4051 and opamp. Maybe a programmable vibrato/reverb/chorus/distortion?
- ADSR Envelope control (in hardware)? (https://github.com/katspaugh/arduino-modular-synth/tree/master/ADSRduino)
- STM32 port? (do I need more i/o's and processor power?)
- karplus-strong string synth (https://github.com/katspaugh/arduino-modular-synth/tree/master/arduino-karplus-strong)
- Understand and embrace Mozzi for synthesis? (ugh. Wading through Mozzi source tree on github hurts my head.)
- MIDI input - take our inputs from a MIDI keyboard instead of the buttons
- Controls for: polyphony settings; envelope, waveshape, and modulation selection (but, we're almost out of pins on the Nano)
