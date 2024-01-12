/*
   Typical pin layout used:
   -------------------------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino     Raspberry Pico
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin         Pin
   -------------------------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST         GP11
   SPI SS      SDA(SS)      10            53        D10        10               10          GP8
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16          GP19
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14          GP16
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15          GP18
*/

/*
Libraries needed:
- USBHID by Various
- MFRC522 by GithubCommunity
*/

/* iPad Pro key release issue:
Add delay(20) in /Users/m/Library/Arduino15/packages/arduino/hardware/mbed_rp2040/4.0.2/libraries/USBHID/src/USBKeyboard.cpp

if (!send(&report)) {
        _mutex.unlock();
        return false;
    }

    delay(20);

    report.data[1] = 0;
    report.data[3] = 0;

*/

#include <SPI.h>
#include <MFRC522.h>
#include "PluggableUSBHID.h"
#include "USBKeyboard.h"

USBKeyboard Keyboard;

#define RST_PIN 11  // Pico //9          // Configurable, see typical pin layout above
#define SS_PIN 8    // Pico //10         // Configurable, see typical pin layout above


MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

long code = 0;
long last_code = 0;
long card = 0;

void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC

  SPI.begin();                        // Init SPI bus
  mfrc522.PCD_Init();                 // Init MFRC522
  delay(4);                           // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  //Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  card = 0;

  if (mfrc522.PICC_IsNewCardPresent()) {
    card = 1;
  } else {
    if (mfrc522.PICC_IsNewCardPresent())
      card = 1;
    else {
      if (card == 1)
        Keyboard.printf("x");
      card = 0;
    }
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  if (card == 1) {
    long code = 0;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      code = ((code + mfrc522.uid.uidByte[i]) * 10);  // Nun werden wie auch vorher die vier Blöcke ausgelesen und in jedem Durchlauf wird der Code mit dem Faktor 10 „gestreckt“. (Eigentlich müsste man hier den Wert 1000 verwenden, jedoch würde die Zahl dann zu groß werden.
    }
    Serial.println(code);

    // if code then key
    if (code == 736490) {
      Keyboard.printf("A");
    } else if (code == 1273520) {
      Keyboard.printf("B");
    } else if (code == 2128480) {
      Keyboard.printf("C");
    } else if (code == 2164270) {
      Keyboard.printf("D");
    } else if (code == 553570) {
      Keyboard.printf("E");
    } else if (code == 2186790) {
      Keyboard.printf("F");
    } else if (code == 1793450) {
      Keyboard.printf("G");
    } else if (code == 1674360) {
      Keyboard.printf("H");
    }

    mfrc522.PICC_HaltA();

    last_code = code;
  } else if (card == 0) {
    //Serial.print("No Card: ");
  }

  delay(500);
}
