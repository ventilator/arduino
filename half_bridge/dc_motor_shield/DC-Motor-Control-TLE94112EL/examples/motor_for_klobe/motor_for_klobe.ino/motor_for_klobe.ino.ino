#include <TLE94112.h>
#include <TLE94112_Motor.h>

// The TLE94112 has only got 3 PWM channels
// There is no one left for motor4
// This means, we cannot control its speed
Tle94112Motor motor1(tle94112, tle94112.TLE_PWM1);
Tle94112Motor motor2(tle94112, tle94112.TLE_PWM2);


void setup()
{
  // call begin for the TLE94112 firs
  tle94112.begin();
  // motor1 is connected to 2 halfbridges on each side
  motor1.connect(tle94112.TLE_HB1, motor1.HIGHSIDE);
  motor1.connect(tle94112.TLE_HB2, motor1.HIGHSIDE);
  motor1.connect(tle94112.TLE_HB3, motor1.LOWSIDE);
  motor1.connect(tle94112.TLE_HB4, motor1.LOWSIDE);

  // motor2 is the LED and will mirror motor1
  motor2.connect(tle94112.TLE_HB5, motor2.HIGHSIDE);
  motor2.connect(tle94112.TLE_HB6, motor2.LOWSIDE);

  //change the motors PWM frequency (just in case you will ever need it...)
  motor1.setPwmFreq(tle94112.TLE_FREQ80HZ);
  motor2.setPwmFreq(tle94112.TLE_FREQ80HZ);

  //when configuration is done, call begin to start operating the motors
  motor1.begin();
  motor2.begin();
  //after calling begin(), the motors are coasting. 
  
}



void loop()
{
  // start the motor and let it run backwards on half speed
  motor1.start(-127);
  motor2.start(-127);

  delay(4000);

  //accelerate motor1 to full speed
  motor1.setSpeed(-255);
  motor2.setSpeed(-255);

  delay(4000);

  //stop all motors (you can use the parameter to set the force which stops and holds them. standard and maximum is 255)
  motor1.stop(255);
  motor2.stop(255);

  delay(500);

  //motor1 and motor2 can also run the other way
  motor1.start(255);
  motor2.start(255);

  delay(4000);

  //ok, that was enough
  motor1.stop();
  motor2.stop();

  delay(500);
}
