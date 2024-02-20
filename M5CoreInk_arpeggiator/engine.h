#include <MIDI.h>
typedef enum {C, CD, D, DD, E, F, FD, G, GD, A, AD, B} notes;
typedef enum {maj, minor, dim, aug} chord_types;

#define c_ionian      0
#define c_dorian      1
#define c_phrygian    2
#define c_lydian      3
#define c_mixolydian  4
#define c_aeolian     5
#define c_harmonic    6
#define c_locrian     7

typedef struct {
    int Shift;
    chord_types chord_type;
} chord;

class arp {
private:
    notes baseNote;
    int baseOctave;

    int octaveShift;
    unsigned int steps;
    //unsigned int mode[6];
    unsigned int indelay;
    unsigned int progression;
    chord *mode;
    int order;
public:
    arp();
    arp(notes bn, int bo, int os, unsigned int st, unsigned int d, unsigned m, unsigned int p);
    void setupArp(int bn, int bo, int os, unsigned int st, unsigned int d, int m, unsigned imode);
    int setProgression(unsigned int p);
    void play();
    void midibegin();
};

int* createChord(notes root, chord_types i, int *notes_array, unsigned int *sh1, unsigned int *sh2);
int midiByNote (notes note, int octave);
