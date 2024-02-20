#include <Arduino.h>
#include "M5CoreInk.h"
#include <MIDI.h>
#include <esp_now.h>
//#include <SoftwareSerial.h>

//SoftwareSerial swSerial(GPIO_NUM_26, GPIO_NUM_36); // RX, TX
//MIDI_CREATE_INSTANCE(SoftwareSerial, swSerial, DIN_MIDI);

struct Serial2MIDISettings : public midi::DefaultSettings
{
  //static const long BaudRate = 31250;
  static const int8_t RxPin  = 26;
  static const int8_t TxPin  = 36;
};

//MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial2, DIN_MIDI, Serial2MIDISettings);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);
//MIDI_CREATE_DEFAULT_INSTANCE();

void setup() {
  M5.begin();
  //USB_MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  delay(300);
}

void loop() {
  //DIN_MIDI.read();o
  MIDI.sendNoteOn(60, 127, 1);
  delay(500);
  MIDI.sendNoteOff(60, 0, 1);
  delay(200);
  MIDI.read();
  /*
  USB_MIDI.sendNoteOn(60, 127, 1);
  delay(100);
  USB_MIDI.sendNoteOff(60, 0, 1);
  delay(100);
  */
}
