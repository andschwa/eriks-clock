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
const int display_address = 0x70;
const int serial_rate = 9600;

void display_setup() {
  matrix.setBrightness(brightness);
  matrix.begin(display_address);
}

void display_double(double num) {
  int expanded = int(100*num);  // Get first four digits into an int
  matrix.writeDigitNum(0, (expanded / 1000) % 10, false);
  matrix.writeDigitNum(1, (expanded / 100) % 10, true);  // Draw dot
  matrix.drawColon(false);
  matrix.writeDigitNum(3, (expanded / 10) % 10, false);
  matrix.writeDigitNum(4, expanded % 10, false);
  matrix.writeDisplay();
}

void setup() {
  display_setup();
}

void loop() {
  // get new number
  double num = 12.34;
  display_double(num);
  delay(wait);
}
