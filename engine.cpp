#include "engine.h"

/***********************************************************/
/** #defines to select which output mode to use:
/** TONE_OUT - use arduino's TONE fuction
/** SYNTH_OUT - use our onboard polyphonic synthesizer
/** MIDI_OUT - use serial MIDI output
/***********************************************************/ 
//#define TONE_OUT
#define SYNTH_OUT
//#define MIDI_OUT

#ifdef SYNTH_OUT
#include "synth.h"
synth nanosynth;
#endif

#ifdef MIDI_OUT
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

//C Ionian      { C maj;  D min;  E min;  F maj;  G maj;  A min;  B dim  }
chord ionian[7]     = {{0, maj},    {2, minor}, {4, minor}, {5, maj},   {7, maj},   {9, minor}, {11, dim}};

//C Dorian      { C min;  D min;  D♯ maj; F maj;  G min;  A dim;  A♯ maj  }
chord dorian[7]     = {{0, minor},  {2, minor}, {3, maj},   {5, maj},   {7, minor}, {9, dim},   {10, maj}};

//C Phrygian    { C min;  C♯ maj; D♯ maj; F min;  G dim;  G♯ maj; A♯ min  }
chord phrygian[7]   = {{0, minor},  {1, maj},   {3, maj},   {5, minor}, {7, dim},   {8, maj},   {10, minor}};

//C Lydian      { C maj;  D maj;  E min;  F♯ dim; G maj;  A min;  B min   }
chord lydian[7]     = {{0, maj},    {2, maj},   {4, minor}, {6, dim},   {7, maj},   {9, minor}, {11, minor}};

//C Mixolydian  { C maj;  D min;  E dim;  F maj;  G min;  A min;  A♯ maj  }
chord mixolydian[7] = {{0, maj},    {2, minor}, {4, dim},   {5, maj},   {7, minor}, {9, minor}, {10, maj}};

//C Aeolian     { C min;  D dim;  D♯ maj; F min;  G min;  G♯ maj; A♯ maj  }
chord aeolian[7]    = {{0, minor},  {2, dim},   {3, maj},   {5, minor}, {7, minor}, {8, maj},   {10, maj}};

//C harmonic    { C min;  D dim;  D♯ aug; F min;  G maj;  G♯ maj; B dim   }
chord harmonic[7]   = {{0, minor},  {2, dim},   {3, aug},   {5, minor}, {7, maj},   {8, maj},   {11, dim}}; 

//C Locrian     { C dim;  C♯ maj; D♯ min; F min;  F♯ maj; G♯ maj; A♯ min  }
chord locrian[7]    = {{0, dim},    {1, maj},   {3, minor}, {5, minor}, {6, maj},   {8, maj},   {10, minor}};

chord *all_chords[8] = {ionian, dorian, phrygian, lydian, mixolydian, aeolian, harmonic, locrian};


// Arrange the N elements of ARRAY in random order.  
void shuffle(uint8_t *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < (n - 1); i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void arp::updateArp(uint8_t baseNoteVal,uint8_t baseOctaveVal,uint8_t octaveShiftVal,uint8_t stepsVal,uint16_t indelayVal,uint8_t modenumVal,uint8_t orderVal){
	baseNote    = (notes)baseNoteVal;
	baseOctave  = (short)baseOctaveVal;
	octaveShift = (short)octaveShiftVal;
	steps       = (unsigned short)stepsVal;
	indelay     = (unsigned int)indelayVal;
	order       = (int)orderVal;
	mode        = all_chords[modenumVal];
	modenum		= modenumVal;
}

int arp::setProgression(unsigned int p)
{
    if (p<8)
    {
        progression = p;
        return 0;
    }
    else
        return -1;
}

int* createChord(notes root, chord_types i, int *notes_array, unsigned short *sh1, unsigned short *sh2)
{
    *sh1=0; *sh2=0;
    int s1, s2;
    switch (i) {
        case maj:   s1 = 4; s2 = 7; break;
        case minor: s1 = 3; s2 = 7; break;
        case dim:   s1 = 3; s2 = 6; break;
        case aug:   s1 = 4; s2 = 8; break;
    }
    notes_array[0] = root;
    notes_array[1] = (root+s1)%12;
    if (root+s1>11) *sh1=1; // octave shift
    notes_array[2] = (root+s2)%12;
    if (root+s2>11) *sh2=1; // octave shift
}

short midiByNote (notes note, short octave)
{
    if ((octave < -1)||(octave > 8))
        return -1;

    return (octave+1)*12 + (int)note;
}

void arp::play()
{
    int i;
    short n, shift, oct_shift=0, bn;
    unsigned short sh1, sh2, notes_added=0;
    notes curNote[3];
    uint8_t notestoplay[15] = {0,0,0,0,0,0,0,0,0,0};
    // Play base note
    if ((int)baseNote + (int)mode[progression].Shift > 11)
        baseOctave++;
    i = (baseNote + mode[progression].Shift)%12;
    bn = midiByNote ((notes)i, baseOctave);
    memset(notestoplay, sizeof(notestoplay), 0);
    
    // Create chord (notes root, intervals i, int *notes_array)
    createChord((notes)i, mode[progression].chord_type, (int*)curNote, &sh1, &sh2);

    // Create the progression
    if ((order == 0)||(order==2)||(order==3))
      for (i=0; i<steps; i++)
      {
          shift = i%3;
          oct_shift = i/3;
          if (shift == 1)
              oct_shift+=sh1;
          if (shift == 2)
              oct_shift+=sh2;
          notestoplay[notes_added] = midiByNote (curNote[shift], octaveShift+oct_shift);
          notes_added++;
      }
    if ((order == 1)||(order==2)||(order==2))
      for (i=steps-1; i>=0; i--)
      {
          shift = i%3;
          oct_shift = i/3;
          if (shift == 1)
              oct_shift+=sh1;
          if (shift == 2)
              oct_shift+=sh2;
          notestoplay[notes_added] = midiByNote (curNote[shift], octaveShift+oct_shift);
          notes_added++;
      }

    if (order==3)
      shuffle(notestoplay, notes_added);
      
      #ifdef MIDI_OUT      
      /*************************************************/
      /**    MIDI OUTPUT:                              */
      /*************************************************/
      MIDI.sendNoteOn(bn, 127, 1);
      /*************************************************/
      #endif

      #ifdef SYNTH_OUT         
      /*************************************************/
      /**    Synth lib OUTPUT:                         */
      /*************************************************/   
      nanosynth.setLength(0,127);
      nanosynth.setLength(1,64);
      nanosynth.mTrigger(0,bn);
      /*************************************************/
      #endif

      #ifdef TONE_OUT
      /*************************************************/
      /**    TONE() OUTPUT:                            */
      /*************************************************/
      tone(3,  (pow (2.0, ((float)bn/12.0)))); //ugly. Converts a midi note number to a frequency value
      delay(indelay*2);
	  noTone(3);
	  /**************************************************/
      #endif

    for (i=0; i<notes_added; i++)
    {
        #ifdef MIDI_OUT
        /**************************************************/
        /**    MIDI OUTPUT:                               */
        /**************************************************/
        MIDI.sendNoteOn(notestoplay[i], 127, 1);
        delay(indelay);
        MIDI.sendNoteOff(notestoplay[i], 0, 1);     // Stop the note
        /**************************************************/ 
        #endif

        #ifdef SYNTH_OUT
        /**************************************************/
        /**     Synth lib OUTPUT:                         */
        /**************************************************/ 
        nanosynth.mTrigger(1,notestoplay[i]);
        delay(indelay);
        /**************************************************/
        #endif
        
        #ifdef TONE_OUT      
        /**************************************************/
        /**    'TONE()' OUTPUT:                           */
        /**************************************************/ 
        tone(3,  (pow (2.0, ((float)(notestoplay[i])/12.0)))); //ugly. Converts a midi note number to a frequency value
        delay(indelay/2);
        noTone(3);
        delay(indelay/4);
        /**************************************************/
        #endif
    }
    
    #ifdef MIDI_OUT
    /**************************************************/
    /**    MIDI OUTPUT:                               */
    /**************************************************/
    MIDI.sendNoteOff(bn, 0, 1); // Stop base note
    /**************************************************/ 
    #endif
    
    #ifdef SYNTH_OUT
    /**************************************************/
    /**    SYNTH OUTPUT:                               */
    /**************************************************/
    nanosynth.setLength(0,0); //turn both voices off
    nanosynth.setLength(1,0);
    /**************************************************/ 
    #endif
}

arp::arp()
{
    int i;
    baseNote = D;
    baseOctave = 5;
    octaveShift = -2;
    steps = 6;
    indelay = 200;
    progression = 0;
    mode = ionian;
}

void arp::midibegin()
{
    #ifdef MIDI_OUT
	MIDI.begin(4);                      // Launch MIDI
    #endif
}

void arp::synthbegin()
{
	#ifdef SYNTH_OUT
	nanosynth.begin(CHB);
	
	//*********************************************************************
	//  Setup all voice parameters in MIDI range
	//  voice[0-3],wave[0-6],pitch[0-127],envelope[0-4],length[0-127],mod[0-127:64=no mod]
	//*********************************************************************
	nanosynth.setupVoice(0,SQUARE,0,ENVELOPE0,100,64);
	nanosynth.setupVoice(1,SINE,0,ENVELOPE1,100,64);
	#endif
}
