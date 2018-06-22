/*
 * gets data from an rpm sensor
 * calculates rpm and which frame number to write
 */
// for your information
float current_rpm = 0;
float current_Hz = 0;
float current_ms_per_step;

// variables to calculate rpm
int rotations_last_calculation = 0;
unsigned long millis_last_calculation = 0;


// will be called by each revolution sensor input via interrupt
void revolution_interrupt() {
   revolution_state = !revolution_state;
   // generates output with half the revolution frequency
   digitalWrite(pin_revolution_output, revolution_state);
   digitalWrite(pin_revolution_LED, revolution_state);

   rotations = rotations+1;
}

// for information and debugging
void message_rpm_via_serial(float rpm) {
  Serial.print(current_ms_per_step);
  Serial.print("ms per step | Hz: ");
  Serial.println(rpm);
}

float calc_rpm(int total_rotations) {
  unsigned long current_time = millis();
  if (current_time >= (millis_last_calculation + 1000)) { //if a second has passed
    //Serial.println(current_time);
    int current_rotations = total_rotations - rotations_last_calculation;
    current_Hz = (current_rotations*1000) / (current_time-millis_last_calculation);
    current_rpm = current_Hz * 60;  
    current_ms_per_step = 1/current_Hz/steps_per_revolution*1000;
    //resetup internal timeed interrupt
    setup_timed_interrupt(current_ms_per_step); 
    // reset counter
    rotations_last_calculation = total_rotations;  
    // report new frequency
    message_rpm_via_serial(current_Hz);
    millis_last_calculation = current_time;    
  }
  return current_rpm;  
}

// will be called every x ms to tick frames
void setup_timed_interrupt(float every_ms) {

}


