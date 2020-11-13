# arpeggiator
Arduino MIDI arpeggiator, based on original work found here: https://github.com/raszhivin/arpeggiator

![Prototype](20201110_152001.jpg?raw=true "Prototype")


## Done:
- built up the circuit on a protoboard
- bugfixes
- tweaked some preferences and pin assignments
- modified for use with an LGT8F328p LQFP32 board (but should work with a Nano as well). 
- Added support for an OLED display (courtesty of the superb u8g2 library: https://github.com/olikraus/u8g2/wiki)
- ADC tweaks to speed up reading the POTS - handle this task asynchronously in an interrupt.
- Required libraries: TimeOne by Paul Stoffregen // MIDI Library by Forty Seven Effects // u8g2 by olikraus
- Eliminate the use of TimerOne (we'll need the timer1 peripheral for our synth):
- Setup the ADC for continuous operation, and generate an interrupt each time it samples a value. Also use this same ISR to read our buttons.

## Todo:
- Make the code and library non-blocking, timer/state machine based (remove the use of delay() )
- Polyphonic output options... incorporate up to ~4 simultaneous voices
- Implement an on-board synthesizer to generate our own tones (thinking of ripping off https://github.com/dzlonline/the_synth )
- Add an onboard amplifier & speaker

## Aspirational Todo (ideas):
- Some form of filtering on board, probably incorporating a CD4051 and opamp. Maybe a programmable vibrato/reverb/chorus/distortion?
- ADSR Envelope control (in hardware)? (https://github.com/katspaugh/arduino-modular-synth/tree/master/ADSRduino)
- Create/upload schematics
- STM32 port? (do I need more i/o's and processor power?)
- karplus-strong string synth (https://github.com/katspaugh/arduino-modular-synth/tree/master/arduino-karplus-strong)
- Understand and embrace Mozzi for synthesis?
