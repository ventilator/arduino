const byte ledPin = 24;
const byte tickPin = 8;
const byte interruptPin = 2;
volatile byte state = LOW;
volatile int rotations = 0;
int rotations_last_calculation = 0;
unsigned long millis_last_calculation = 0;
float current_rpm = 0;
float current_Hz = 0;
int steps_per_revolution = 360;
float current_ms_per_step;


void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(tickPin, OUTPUT);
  pinMode(interruptPin, INPUT);
  millis_last_calculation = millis();
  Serial.begin(115200);
  Serial.println("init done.\n");
  attachInterrupt(digitalPinToInterrupt(interruptPin), count, FALLING);
  
}

void loop() {
  //detachInterrupt(digitalPinToInterrupt(interruptPin)); //detach during calculations and stuff
  
  calc_rpm(rotations);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), count, FALLING);
}

float calc_rpm(int total_rotations) {
  unsigned long current_time = millis();
  if (current_time >= (millis_last_calculation + 1000)) { //if a second has passed
    //Serial.println(current_time);
    int current_rotations = total_rotations - rotations_last_calculation;
    current_Hz = (current_rotations*1000) / (current_time-millis_last_calculation);
    current_rpm = current_Hz * 60;  
    current_ms_per_step = 1/current_Hz/steps_per_revolution*1000;
    
    rotations_last_calculation = total_rotations;  
    message_rpm_via_serial(current_Hz);
    millis_last_calculation = current_time;    
  }
  message_rpm(current_rpm);
  return current_rpm;  
}

void message_rpm(float rpm)
{
  digitalWrite(ledPin, state);
  digitalWrite(tickPin, state);
}

void message_rpm_via_serial(float rpm)
{
  Serial.print(current_ms_per_step);
  Serial.print("ms per step | Hz: ");
  Serial.println(rpm);
}

void count() {
  state = !state;
  rotations++;
}
