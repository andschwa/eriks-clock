/*
  Basic code to display a desired number on an Adafruit 7 segment
  display.

  File: clock.ino
  Author: Andrew Schwartzmeyer
 */

#include <Wire.h>

#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();

const int wait = 1000;
const uint8_t brightness = 12;
const int clock_pin = 0x77;
const int serial_rate = 9600;

void setup() {
  matrix.setBrightness(brightness);
  Serial.begin(serial_rate);
  Serial.println("Lets test this thing");
  matrix.begin(clock_pin);
}

void loop() {
  // get new number
  int num = 314159;
  matrix.println(num, DEC);
  matrix.writeDisplay();
  delay(wait);
}
