#include "lib/MIDI.h"
typedef enum {C, CD, D, DD, E, F, FD, G, GD, A, AD, B} notes;
typedef enum {maj, minor, dim, aug} chord_types;

typedef struct {
    short Shift;
    chord_types chord_type;
} chord;

class arp {
private:
public:
    arp();
    arp(notes bn, short bo, short os, unsigned short st, unsigned int d, unsigned m, unsigned int p);
    void setupArp();
    int  setProgression(unsigned int p);
    void play();
    void midibegin();
    //raw pot values:
        volatile unsigned int bn, bo, os, st, d, m, imode;
    //parsed pot values:
		unsigned int modenum, indelay, progression; 
		short baseOctave, octaveShift;
		unsigned short steps;
		int order;
		notes baseNote;
		chord *mode;
};

int* createChord(notes root, chord_types i, int *notes_array, unsigned short *sh1, unsigned short *sh2);
short midiByNote (notes note, short octave);
