#include "M5CoreInk.h"
//#include <TimerOne.h>
#include <Ticker.h> // ESP compatible timer

// https://github.com/raszhivin/arpeggiator
#include "engine.h"

// default arpeggio settings
int baseNote = 0;
int baseOctave = 5;
unsigned int octaveShift = 2;
unsigned int steps = 4;
int bpm = 6;
int indelay = bpm * 1000 / 60 * 10;
unsigned int mode = c_harmonic;
int order = 0;

arp a((notes)baseNote, baseOctave, octaveShift, steps, indelay, mode, order);
//arp a(C, 5, 2, 6, 200, c_harmonic, 0);
bool button_pressed;
int ButtonVal;

// M5 Core UI
const int menulength = 7;
char* menuitems [menulength] = {"Base Note", "Base Octave", "Octave Shift", "Steps", "BPM", "Mode", "Order"};
int menuvalues [menulength] = {baseNote, baseOctave, octaveShift, steps, bpm, mode, order};
int menuindex = 0;
char *chordnames [8] = {"ionian", "dorian", "phrygian", "lydian", "mixolydian", "aeolian", "harmonic", "locrian"};
char *notenames[12] = {"C", "CD", "D", "DD", "E", "F", "FD", "G", "GD", "A", "AD", "B"};

boolean play = false;

Ink_Sprite InkPageSprite(&M5.M5Ink);

#define LEDPin 10

// Synchronization: choose one of two possible options:
//#define EXT_SYNC
#define INT_SYNC // does not work, manually set to int

Ticker Timer1_;

// read M5 buttons
void readButtons()
{
  if ( M5.BtnUP.wasPressed()) setValuesUI("up");
  if ( M5.BtnDOWN.wasPressed()) setValuesUI("down");
  if ( M5.BtnMID.wasPressed()) setValuesUI("mid_pressed");
  if ( M5.BtnEXT.wasPressed()) setValuesUI("ext_pressed");
  if ( M5.BtnPWR.wasPressed()) {
    //setValuesUI("power");
    InkPageSprite.clear();
    //InkPageSprite.pushSprite();
    InkPageSprite.drawString(100-16, 100-8, "***");
    InkPageSprite.pushSprite();
    delay(1000);
    InkPageSprite.clear();
    InkPageSprite.pushSprite();
    M5.shutdown();
  }
  M5.update();
}

// set UI and arpeggio after reading buttons
void setValuesUI(char* str) {

  if (str == "up")
    menuvalues[menuindex] += 1;
  else if (str == "down")
    menuvalues[menuindex] -= 1;
  else if (str == "mid_pressed") {
    if (menuindex < menulength - 1)
      menuindex ++;
    else
      menuindex = 0;
  }
  else if (str == "ext_pressed") {
    play = !play;
    //Serial.println("ext");
  }
  else if (str == "init")
    Serial.println("init");
  else if (str == "power")
    InkPageSprite.clear();
    

  Serial.printf("%s %d %d", str, menuindex, menuvalues[menuindex]);
  Serial.printf("\r\n");

  baseNote = constrain(menuvalues[0], 0, 11);
  baseOctave = constrain(menuvalues[1], 0, 7);
  octaveShift = constrain(menuvalues[2], 0, 7);
  steps = constrain(menuvalues[3], 1, 6);
  bpm = constrain(menuvalues[4], 1, 20);
  indelay = 60 * 1000 / (bpm * 10);
  mode = constrain(menuvalues[5], 0, 7);
  order = constrain(menuvalues[6], 0, 3);
  Serial.println(indelay);

  //button_pressed = true;

  a.setupArp((notes)baseNote, baseOctave, octaveShift, steps, indelay, mode, order);

  // UI

  InkPageSprite.clear();
  
  for (int i = 0; i < menulength; i++) {

    char strvalue[64];
    sprintf(strvalue, "%d", menuvalues[i]);

    if (i == 0) // int to note
      sprintf(strvalue, "%s", notenames[menuvalues[i]]);
    else if (i == 4) // bpm * 10
      sprintf(strvalue, "%d", menuvalues[i]*10);

    //Serial.printf("%d", i);
    //Serial.printf("\r\n");
    //Serial.printf(menuitems[i]);
    //Serial.printf("\r\n");

    //Ink_eSPI_font_t* fontPtr = &AsciiFont24x48; // set alternative font
    InkPageSprite.drawString(10, 10 + 25 * i, menuitems[i]);
    InkPageSprite.drawString(180, 10 + 25 * i, strvalue);
  }

  //InkPageSprite.FillRect(1, 10 + 25*menuindex, 5, 10, 0); // Rect left of active menu item
  InkPageSprite.FillRect(0, 10 + 25*menuindex-2, 200, 1, 0); // Line above and under active menu item
  InkPageSprite.FillRect(0, 10 + 25*menuindex+14, 200, 1, 0);

  // play / pause
  if (play == true)
    InkPageSprite.drawString(100-4, 180, ">");
  else
    InkPageSprite.drawString(100-4, 180, "#");

  InkPageSprite.pushSprite();
  delay(100);
}

void setup()
{
  M5.begin();

  if ( !M5.M5Ink.isInit())
  {
    Serial.printf("Ink Init failed");
    while (1) delay(100);
  }
  M5.M5Ink.clear();
  delay(1000);
  //creat ink refresh Sprite
  if ( InkPageSprite.creatSprite(0, 0, 200, 200, true) != 0 )
  {
    Serial.printf("Ink Sprite creat failed");
  }
  //InkPageSprite.drawString(50, 50, "***");
  //InkPageSprite.pushSprite();
  //delay(1000);
  //InkPageSprite.clear();


  a.midibegin();
  // Arduino Timer
  //Timer1.initialize(200000);          // initialize timer1, and set a 1/10 second period
  //Timer1.attachInterrupt(readPoties); // We will read potis and buttons values within timer interrupt
  // ESP Timer
  Timer1_.attach_ms(100, readButtons);

  // LED pin
  pinMode(LEDPin, OUTPUT);

  //a.play();

  setValuesUI("init"); // draw UI
}

void loop()
{
  /*
  if (button_pressed)
  {
    int ButtonVal = random(8);
    //a.setProgression(ButtonVal - 1);
    button_pressed = false;

    // Switch on LED
    digitalWrite(LEDPin, HIGH);
    a.play();

    // Switch off LED
    digitalWrite(LEDPin, LOW);

    //Serial.printf("%d", baseNote);
  }
  */

  if (play == true)
    a.play(); // continouus playing
}
