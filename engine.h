#include <MIDI.h>
typedef enum {C, CD, D, DD, E, F, FD, G, GD, A, AD, B} notes;
typedef enum {maj, minor, dim, aug} chord_types;

typedef struct {
    short Shift;
    chord_types chord_type;
} chord;

class arp {
private:
    notes baseNote;
    short baseOctave;

    short octaveShift;
    unsigned short steps;
    unsigned int indelay;
    unsigned int progression;
    int order;
    chord *mode;
public:
    arp();
    arp(notes bn, short bo, short os, unsigned short st, unsigned int d, unsigned m, unsigned int p);
    void setupArp(short bn, short bo, short os, unsigned short st, unsigned int d, int m, unsigned imode);
    int  setProgression(unsigned int p);
    void play();
    void midibegin();
    unsigned int modenum; //so that we can figure out and display the currently set mode
    unsigned int raw; //temporary. Used for debug
};

int* createChord(notes root, chord_types i, int *notes_array, unsigned short *sh1, unsigned short *sh2);
short midiByNote (notes note, short octave);
