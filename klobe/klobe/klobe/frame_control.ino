/*
 * generates frames
 * writes them to the strip
 * 
 */
//-----------------------------------LED Stuff -------------------------
#include "FastLED.h"
// How many leds are in the strip?
#define NUM_LEDS 60
// Data pin that led data will be written out over
#define DATA_PIN 3
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];
//-----------------------------------LED Stuff -------------------------
 
// config this according to your animation
int steps_per_revolution = 1000; 

void setup_led_stripe(byte pin_led_clock,byte pin_led_data) {

// This function sets up the ledsand tells the controller about them
      FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
}

// This function runs over and over, and is where you do the magic to light
// your leds.
void loop_LEDs() {
   // Move a single white led 
   for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
      // Turn our current led on to white, then show the leds
      leds[whiteLed] = CRGB::White;

      // Show the leds (only one of which is set to white, from above)
      FastLED.show();

      // Wait a little bit
      delay(100);

      // Turn our current led back to black for the next loop around
      leds[whiteLed] = CRGB::Black;
   }
}
