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
    chord *mode;
    int order;
public:
    arp();
    int  setProgression(unsigned int p);
    void play();
	void updateArp(uint8_t baseNoteVal,uint8_t baseOctaveVal,uint8_t octaveShiftVal,uint8_t stepsVal,uint16_t indelayVal,uint8_t modenumVal,uint8_t orderVal);
    void midibegin();
    void synthbegin();
    uint8_t modenum;
};

int* createChord(notes root, chord_types i, int *notes_array, unsigned short *sh1, unsigned short *sh2);
short midiByNote (notes note, short octave);
//synth nanosynth;

