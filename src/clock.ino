/*
  Basic code to display a desired number on an Adafruit 7 segment
  display.

  File: clock.ino
  Author: Andrew Schwartzmeyer
 */

// Includes
#include <Wire.h>

#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_7segment matrix = Adafruit_7segment();

// millis counter
extern volatile unsigned long timer0_overflow_count;
const unsigned long reset_time = 86400000;

// Constants

const int wait = 1000;
const int serial_rate = 9600;
const int minute_millis = 60000;

const uint8_t brightness = 12;
const int display_address = 0x77;

const int interrupt_pin = 2;
volatile int pin_state = LOW;
volatile int state = LOW;

unsigned long new_time = 0;
unsigned long old_time = 0;
unsigned long period = 0;
double bpm = 0.0;
double avg = 0.0;

// Prototypes
void display_setup();
void display_double(double num);
void pendulum_interrupt_call();

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

void pendulum_interrupt_call() {
  new_time = millis();
  period = (new_time - old_time) / 2;
  old_time = new_time;
  bpm = double(minute_millis) / double(period);
  Serial.print("Interrupted\n");
}

void setup() {
  Serial.begin(serial_rate);
  Serial.print("Setup\n");
  display_setup();
  Serial.print("Past display setup\n");
  attachInterrupt(interrupt_pin, pendulum_interrupt_call, FALLING);
  Serial.print("Interrupt attached\n");
}

void loop() {
  Serial.print("Loop\n");
  if (new_time > reset_time) {
    Serial.print("Timer reset\n");
    timer0_overflow_count = 0;  // Reset periodically
  }
  bpm = 12.34;
  display_double(bpm);
  delay(wait);
}
