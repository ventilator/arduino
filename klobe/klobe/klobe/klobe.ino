/*
 * setup up serial communication for debugging
 * setup up interrups
 */
#include <TimerOne.h>

// essential pins 
byte pin_rpm_sensor = 2;
//byte pin_led_clock = 4; // see rame control
//byte pin_led_data = 5;  // see rame control
// debug pins (output pins for debugging on oscilloscope)
byte pin_revolution_output = 6; //triggers if sensor reads one revolution
byte pin_revolution_LED = LED_BUILTIN; //same as above but output is blinking bultin LED
byte pin_next_frame = 7; // triggers whenever a new frame is send to the stripe (should be a multiple of every revolution)
byte pin_recalculation_rpm = 10; // triggers whenever a new rpm is calculated (for debugging time start)

// interrupt related 
volatile byte revolution_state = LOW; // for debugging output
volatile int rotations = 0; // counting rotations


void setup() {
  //setup communication
  Serial.begin(9600);
  Serial.print("init ");   
  
  // setup input pins
  pinMode(pin_rpm_sensor, INPUT);
  
//  // setup output pins
//  pinMode(pin_led_clock, OUTPUT);
//  pinMode(pin_led_data, OUTPUT);
  
  // setup debug pins (output pins)
  pinMode(pin_revolution_output, OUTPUT);
  pinMode(pin_revolution_LED, OUTPUT);
  pinMode(pin_next_frame, OUTPUT);
  pinMode(pin_recalculation_rpm, OUTPUT);
  
  // setup rpm sensor interrupt
  attachInterrupt(digitalPinToInterrupt(pin_rpm_sensor), revolution_interrupt, FALLING);

  setup_led_stripe();

  // communicate end of setup
  Serial.println("done."); 
}

void loop() {
  // save rotations and calculate current rpm
  noInterrupts();
  int copy_rotations = rotations;
  interrupts();
  calc_rpm(rotations);
  //delay(10); //we need to update only every second but no delays in the loop (disturbes interrupts)

  //testing LEDs, later on this will be called by a timed interrupt
  //loop_LEDs(); //if Hz calculation incorrecet, LEDs routine causes problems
}
