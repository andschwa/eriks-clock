/*
  Basic code to display a desired number on an Adafruit 7 segment
  display.

  File: clock.ino
  Author: Andrew Schwartzmeyer

  I2C CLK, DAT
  UNO Analog #5, #4
  Leonardo Digital #3, #2
  Mega Digital #21, #20

  Interrupts 0, 1
  Uno Digital #2, #3
  Leonardo Digital #3, #2
  Mega Digital #2, #3, #21, #20, #19, #18
  attachInterrupt(interrupt, function, mode) i.e. not the pin number
 */

// Includes
#include <Wire.h>

#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();

// Constants

const int interrupt = 0;

const int wait = 1000;
const int serial_rate = 9600;

const uint8_t brightness = 12;
const int display_address = 0x77;

const int millis_in_minute = 60000;

// Globals :(

unsigned long new_time = 0;
unsigned long old_time = 0;
unsigned long period = 0;
double bpm = 0.0;
double avg = 0.0;

// Prototypes
void display_setup();
void display_double(double num);
void swing_ISR();

// Main code

void display_setup() {
  Serial.print("Display setup\n");
  /* matrix.setBrightness(brightness); */
  /* Serial.print("Past brightness\n"); */
  matrix.begin(display_address);
  Serial.print("Past bein\n");
}

void display_double(double num) {
  Serial.print("Displaying double\n");
  int expanded = int(100*num);  // Get first four digits into an int
  matrix.writeDigitNum(0, (expanded / 1000) % 10, false);
  matrix.writeDigitNum(1, (expanded / 100) % 10, true);  // Draw dot
  matrix.drawColon(false);
  matrix.writeDigitNum(3, (expanded / 10) % 10, false);
  matrix.writeDigitNum(4, expanded % 10, false);
  matrix.writeDisplay();
}

void swing_ISR() {
  new_time = millis();
  period = (new_time - old_time) / 2;
  old_time = new_time;
  bpm = double(millis_in_minute) / double(period);
  Serial.print("Interrupted\n");
}

void setup() {
  Serial.begin(serial_rate);
  Serial.print("Setup\n");
  display_setup();
  Serial.print("Past display setup\n");
  attachInterrupt(interrupt, swing_ISR, FALLING);
  Serial.print("Interrupt attached\n");
}

void loop() {
  Serial.print("Loop\n");
  bpm = 12.34;
  display_double(bpm);
  delay(wait);
}
