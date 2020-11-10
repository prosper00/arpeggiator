# arpeggiator
Arduino MIDI arpeggiator, based on original work found here: https://github.com/raszhivin/arpeggiator

![Prototype](20201110_152001.jpg?raw=true "Prototype")


## Done:
- built up the circuit on a protoboard
- bugfixes
- tweaked some preferences and pin assignments
- modified for use with an LGT8F328p LQFP32 board (but should work with a Nano as well). 
- Added support for an OLED display (courtesty of the superb 8g2 library: https://github.com/olikraus/u8g2/wiki)

## Todo:
- Display cool stuff on the OLED
- Tidy up my code. It's a bit of a mess at the moment
- Make the code and library non-blocking, timer/state machine based (remove the use of delay()
- Polyphonic output options... incorporate up to ~4 simultaneous voices

## Aspirational Todo:
- Implement an on-board synthesizer to generate our own tones (thinking of ripping off https://github.com/nathanRamaNoodles/Noodle-Synth )
- Some form of filtering on board, probably incorporating a CD4051 and opamp. Maybe a programmable vibrato/reverb/chorus/distortion?
- ADSR Envelope control?
- Add an onboard amplifier & speaker
- Create/upload schematics
