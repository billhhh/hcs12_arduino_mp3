/*
   author: Bill Wang
   date: 12-05-2016
  Software serial multple serial test

  Receives from the hardware serial, sends to software serial.
  Receives from software serial, sends to hardware serial.

  The circuit:
   RX is digital pin 10 (connect to TX of other device)
   TX is digital pin 11 (connect to RX of other device)

  Note:
  Not all pins on the Mega and Mega 2560 support change interrupts,
  so only the following can be used for RX:
  10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

  Not all pins on the Leonardo support change interrupts,
  so only the following can be used for RX:
  8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

  created back in the mists of time
  modified 25 May 2012
  by Tom Igoe
  based on Mikal Hart's example

  This example code is in the public domain.

*/
#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>

SdFat sd;
SFEMP3Shield MP3player;

int data;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(13, OUTPUT);

  //start the shield
  sd.begin(SD_SEL, SPI_HALF_SPEED);
  MP3player.begin();
}

void loop() { // run over and over

  if (Serial.available()) {
    data = Serial.read();
    digitalWrite(13, HIGH);
    delay(1000);
    Serial.println(data);

    if (data <= 57) {
      //start playing tracks
      MP3player.playTrack(data - 47);
      delay(2000);
    }

    else {
      MP3player.playTrack(data - 55);
      delay(2000);
    }
  }

  else {
    digitalWrite(13, LOW);
  }

}

