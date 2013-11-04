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

const int sample_size = 120;

const int wait = 1200;
const int serial_rate = 9600;

const uint8_t brightness = 15;
const int display_address = 0x70;

// Globals :(

volatile unsigned long prior_time = 0;

volatile unsigned long sample = 0;
volatile unsigned long sample_times[sample_size];
volatile int sample_index = 0;

volatile unsigned long cumulative_average = 0;
volatile unsigned long cumulative_index = 0;

// Prototypes

void display_setup();
void display_double(double num);
void swing_ISR();
unsigned long get_sample_total();
unsigned long get_rolling_average();

// Main code

void display_setup() {
  /* matrix.setBrightness(brightness); */
  /* Serial.print("Past brightness\n"); */
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

void swing_ISR() {
  sample = millis() - prior_time;
  prior_time = millis();  // millis doesn't change in an ISR
  // millis will, however, overflow and reset to 0 in 49.7103 days

  sample_times[sample_index] = sample;
  sample_index = (sample_index + 1) % sample_size;

  cumulative_average = (((cumulative_index * cumulative_average) + sample) /
			cumulative_index + 1);
  cumulative_index += 1;
}

unsigned long get_sample_total() {
  unsigned long total = 0;
  for (int i = 0; i < sample_size; i++) {
    total += sample_times[i];
  }
  return total;
}

unsigned long get_rolling_average() {
  return (get_sample_total() / ((cumulative_index < sample_size)
			       ? cumulative_index : sample_size));
}

void setup() {
  display_setup();

  attachInterrupt(interrupt, swing_ISR, FALLING);

  for (int i = 0; i < sample_size; i++) {
    sample_times[i] = 0;
  }

  delay(wait);
}

void loop() {
  double bpm = 60000 / double(get_rolling_average());
  display_double(bpm);
}
