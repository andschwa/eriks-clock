/*
  Basic code to display a desired number on an Adafruit 7 segment
  display.

  File: clock.ino
  Author: Andrew Schwartzmeyer
          Erik Illum

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

/* Defines */
//#define PHOTOCELL
#define DEBUG

/* Includes */
#include <Wire.h>

#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Adafruit_NeoPixel.h"

/* Constants */
const int LIGHT_THRESHOLD = 400;
const int SAMPLE_SIZE = 60;
const int INTERRUPT = 7;
const uint8_t BRIGHTNESS = 15;
const int DISPLAY_ADDRESS = 0x70;  /* I2C */
const int PRINT_DELAY = 2000;

/* Pins */
const int LEDSTRIP_PIN = 6;  /* digital */
const int PHOTOCELL_PIN = A3;  /* analog */

/* LED strip */
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, LEDSTRIP_PIN, NEO_GRB + NEO_KHZ800);

/* 7-seg display */
Adafruit_7segment matrix = Adafruit_7segment();

/* Globals */
int photocell_reading = 0;
unsigned long print_time = 0;

volatile unsigned long prior_time = 0;

volatile unsigned long sample = 0;
volatile unsigned long sample_times[SAMPLE_SIZE];
volatile int sample_index = 0;

volatile unsigned long cumulative_average = 0;
volatile unsigned long cumulative_index = 0;

/* Prototypes */

void display_setup();
void display_double(double num);
void swing_ISR();
unsigned long get_sample_total();
unsigned long get_rolling_average();

/* Main code */

void display_double(double num) {
  int expanded = int(100 * num);  /* Get first four digits into an int */
  matrix.writeDigitNum(0, (expanded / 1000) % 10, false);
  matrix.writeDigitNum(1, (expanded / 100) % 10, true);  /* Draw dot */
  matrix.drawColon(false);
  matrix.writeDigitNum(3, (expanded / 10) % 10, false);
  matrix.writeDigitNum(4, expanded % 10, false);
  matrix.writeDisplay();
}

void swing_ISR() {
  sample = millis() - prior_time;
#ifdef DEBUG
  Serial.println(sample);
#endif
  if (sample < 500) return;
  prior_time = millis();  /* millis doesn't change in an ISR */
  /* millis will, however, overflow and reset to 0 in 49.7103 days */

  sample_times[sample_index] = sample;
  sample_index = (sample_index + 1) % SAMPLE_SIZE;

  cumulative_average = (((cumulative_index * cumulative_average) + sample) /
			cumulative_index + 1);
  cumulative_index += 1;
  return;
}

unsigned long get_sample_total() {
  unsigned long total = 0;
  for (int i = 0; i < SAMPLE_SIZE; i++) total += sample_times[i];
  return total;
}

unsigned long get_rolling_average() {
  return (get_sample_total() / ((cumulative_index < SAMPLE_SIZE)
			       ? cumulative_index : SAMPLE_SIZE));
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  matrix.begin(DISPLAY_ADDRESS);
  strip.begin();
  strip.show();

  attachInterrupt(INTERRUPT, swing_ISR, FALLING);

  for (int i = 0; i < SAMPLE_SIZE; i++) sample_times[i] = 0;

  delay(1200);
  print_time = millis();
}

void loop() {
//#ifdef PHOTOCELL
  photocell_reading = analogRead(PHOTOCELL_PIN);
#ifdef DEBUG
  Serial.print("Analog reading = ");
  Serial.println(photocell_reading);
#endif

//  if (photocell_reading < LIGHT_THRESHOLD) {
    colorWipe(strip.Color(255, 255, 255), 60);
//  } else {
//    colorWipe(strip.Color(0, 0, 0), 60);
//  }
//#endif

  if (millis() > print_time) {
    double bpm = 60000 / (double)get_rolling_average();
    print_time = millis() + PRINT_DELAY;
    display_double(bpm);
  }
}
