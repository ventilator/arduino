/*
 * generates frames
 * writes them to the strip
 * 
 */
//-----------------------------------LED Stuff -------------------------
#include "FastLED.h"
// How many leds are in the strip?
#define NUM_LEDS 60
//// Data pin that led data will be written out over
#define DATA_PIN 3
//// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define CLOCK_PIN 4



// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];
//-----------------------------------LED Stuff -------------------------
 
// config this according to your animation

int steps_per_revolution = 4; //devide globe in 1000 frames //more like 20, no more time left
volatile int current_step = 0; // current frame

void setup_led_stripe() {

// This function sets up the ledsand tells the controller about them
      FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
}

volatile int currently_on_led = 0;
// at the beginning of a revolution, this function is called from an interrupt
void reset_frame_loop() {
  current_step = 0;     // volatile variable
  currently_on_led = 0; // volatile variable
}

// This function runs over and over, and is where you do the magic to light
// your leds.

void next_LED_frame() {
  current_step++; //keep track of frames
  
  // Move a single white led 
  // Turn our current led on to white, then show the leds
  leds[currently_on_led] = CRGB::White;
  
  // Show the leds (only one of which is set to white, from above)
  FastLED.show();
  
  // Turn our current led back to black for the next loop around
  leds[currently_on_led] = CRGB::Black;
  currently_on_led = (currently_on_led+1) % NUM_LEDS;
  
}
