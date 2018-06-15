/*
 * This is demo for a rotation wheel triggering a sensor via external interrupt
 * Then and internal timed interrupt fires for each degree (frame) within a rotation
 * 
 * This works only on a XMC 4700 Relax Kit
 */

const byte ledPin = 25; //2nd LED
const byte tickPin = 13; //do not use pin 8 or 7, for unknown reason this does not work;
const byte timedtickPin = 7;
const byte interruptPin = 3;
volatile byte state = LOW;
volatile int rotations = 0;
int rotations_last_calculation = 0;
unsigned long millis_last_calculation = 0;
float current_rpm = 0;
float current_Hz = 0;
int steps_per_revolution = 1000;
float current_ms_per_step;

extern "C" {
  void CCU42_0_IRQHandler(void)
  {
    digitalToggle(LED1);
    digitalToggle(timedtickPin);   
  }
}

// works only for periods 0.4ms and lower
void setup_timed_interrupt(float every_ms) {
  XMC_CCU4_SLICE_COMPARE_CONFIG_t pwm_config = {0};
            pwm_config.passive_level = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_HIGH;
            pwm_config.prescaler_initval = XMC_CCU4_SLICE_PRESCALER_1;  // 144 MHZ

  XMC_CCU4_Init(CCU42, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);

  XMC_CCU4_SLICE_CompareInit(CCU42_CC43, &pwm_config);

  XMC_CCU4_EnableClock(CCU42, 3);

  //calc devider. base frequency 144MHz
  int base_f = 144000000;
  int devider = every_ms * base_f/1000;
//  Serial.print("interrupt devider: ");
//  Serial.print(devider);
//  Serial.print(" ");

  //devider = 7200;
  XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU42_CC43, devider); // 144 MHZ / 7200 = 20KHz

  /* Enable compare match and period match events */
  XMC_CCU4_SLICE_EnableEvent(CCU42_CC43, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);

  /* Connect period match event to SR0 */
  XMC_CCU4_SLICE_SetInterruptNode(CCU42_CC43, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);
  
  /* Configure NVIC */

  /* Set priority */
  NVIC_SetPriority(CCU42_0_IRQn, 10U);

  /* Enable IRQ */
  NVIC_EnableIRQ(CCU42_0_IRQn); 
  XMC_CCU4_EnableShadowTransfer(CCU42, (CCU4_GCSS_S0SE_Msk << (4 * 3)));
  XMC_CCU4_SLICE_StartTimer(CCU42_CC43);
}
  



void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(tickPin, OUTPUT);
  pinMode(timedtickPin, OUTPUT);
  
  pinMode(interruptPin, INPUT);
  millis_last_calculation = millis();
  Serial.begin(115200);
  Serial.println("init done.\n");
  attachInterrupt(digitalPinToInterrupt(interruptPin), count, FALLING);

  setup_timed_interrupt(0.1); //for testing. don't go above 0.4ms, doesnt work
}

void loop() {
//  detachInterrupt(digitalPinToInterrupt(interruptPin)); //detach during calculations and stuff
  //delay(20);
  //setup_timed_interrupt(0.4); 
  noInterrupts();
  int copy_rotations = rotations;
  interrupts();
  calc_rpm(rotations);
  delay(10); //we need to update only every second
  
//  attachInterrupt(digitalPinToInterrupt(interruptPin), count, FALLING);
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
  //message_rpm(current_rpm);
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
  digitalToggle(ledPin);
  digitalToggle(tickPin);  
  rotations++;
}
