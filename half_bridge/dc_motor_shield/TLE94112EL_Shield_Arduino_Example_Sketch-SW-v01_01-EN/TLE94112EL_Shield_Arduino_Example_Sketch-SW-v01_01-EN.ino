/* 
 *  Sketch for DC Motor Shield with TLE94112EL 
 *  Date of creation: 2017-05-30
 *  This example can be downloaded at http://www.infineon.com/shields-for-arduino
 */

#include <SPI.h>

const int EN = 8;                       // Set pin 8 to control Enable pin of TLE94112
const int CSN = 10;                     // Set pin 10 (CSN1) to control CSN of TLE94112 (default)
// Note: CSN2 (pin 12) can also be used to control CSN of the TLE94112: R7 must be desoldered, and a 0 Ohm resistor must be soldered on R8 pad
// Refer to the user manual at http://www.infineon.com/shields-for-arduino

// Addresses of the control registers
const byte HB_ACT_1_CTRL = B0000011;    // Activation/deactivation of  HB1-4
const byte HB_ACT_2_CTRL = B1000011;    // Activation/deactivation of  HB5-8
const byte HB_ACT_3_CTRL = B0100011;    // Activation/deactivation of  HB9-12
const byte HB_MODE_1_CTRL = B1100011;   // Control mode (PWM enable, PWM channel) of HB1-4
const byte HB_MODE_2_CTRL = B0010011;   // Control mode (PWM enable, PWM channel) of HB5-8
const byte HB_MODE_3_CTRL = B1010011;   // Control mode (PWM enable, PWM channel) of HB9-12
const byte PWM_CH_FREQ_CTRL = B0110011; // PWM clock frequency and frequency modulation of the oscillator
const byte PWM1_DC_CTRL = B1110011;     // Duty cycle of PWM Channel 1
const byte PWM2_DC_CTRL = B0001011;     // Duty cycle of PWM Channel 2
const byte PWM3_DC_CTRL = B1001011;     // Duty cycle of PWM Channel 3
const byte FW_OL_CTRL = B0101011;       // LED mode for HS1/2 and active free-wheeling of HB1-6
const byte FW_CTRL = B1101011;          // Active free-wheeling of HB7-12
const byte CONFIG_CTRL = B1100111;      // Device ID

// Address of the status registers
const byte SYS_DIAG_1 = B00011011;      // Global Status Register
const byte SYS_DIAG_2 = B01011011;      // Overcurrent HB1-4
const byte SYS_DIAG_3 = B00111011;      // Overcurrent HB5-8
const byte SYS_DIAG_4 = B01111011;      // Overcurrent HB9-12
const byte SYS_DIAG_5 = B00000111;      // Open load HB1-4
const byte SYS_DIAG_6 = B01000111;      // Open load HB5-8
const byte SYS_DIAG_7 = B00100111;      // Open load HB9-12

const byte WRITE = 0b10000000; // Mask for write commands to control registers
const byte CLEAR = 0b10000000; // Mask for clear commands to status registers

void setup() {
  // Setup fo the parameters for SPI communication, set CSN and EN pins as outputs and set EN to High  
  Serial.begin(9600);                     // sets the baud rate for communication with the computer to 9600 bauds
  SPI.begin();                            // Initializes the SPI bus
  SPI.setBitOrder(LSBFIRST);              // Least significant bit is sent first 
  SPI.setClockDivider(SPI_CLOCK_DIV16);   // SPI clock frequency set to 1 MHz
  SPI.setDataMode(SPI_MODE1);             // Set SPI clock polarity and clock phase; CPOL = 0, CPHA = 1
  pinMode(CSN, OUTPUT);                   // sets CSN as output
  pinMode(EN, OUTPUT);                    // sets EN as output
  digitalWrite(EN, HIGH);                 // TLE94112 set in active mode (ENABLE pin = high)
}

void loop()
{
  activateMotor1_6();                    // Motors 1-6 are activated for 1s, then braked for 300 ms and activated in the reverse direction for 1s and braked for 300 ms 
  activateMotor1_PWM();                  // Motor 1 is activated for 2s with 90% duty cycle / 100 Hz, for 2s with 20% duty cycle, then braked for 300 ms
}

void activateMotor1_PWM()
{
    // In this function activates the motor connected between OUT1 and OUT2 in PWM
    // at 90% duty cycle for 2s: HS1 ON with 90% duty cycle for 2s / 100 Hz, LS2 ON
    // at 20% duty cycle for 2s: HS1 ON with 20% duty cycle for 2s / 100 Hz, LS2 ON
    // then, Motor1 is braked to GND for 300 ms (LS1 and LS2 are ON)
    writeRegister(HB_MODE_1_CTRL,0b1);      // Map HB1 to PWM channel 1
    writeRegister(PWM_CH_FREQ_CTRL,0b10);   // Set Frequency of PWM channel 1 to 100 Hz
    writeRegister(PWM1_DC_CTRL,0b11100110); // Set duty cycle of PWM channel to 90 %
    writeRegister(HB_ACT_1_CTRL,0b0110);    // Activate HS1/LS2
    delay(2000);                            // Wait 2000 ms
    writeRegister(PWM1_DC_CTRL,0b00110011); // Set duty cycle of PWM channel to 20 %
    delay(2000);                            // Wait 2000 ms
    writeRegister(HB_ACT_1_CTRL,0b0101);    // Activate LS1/LS2
    delay(300);                             // Wait 300 ms
    
    writeRegister(HB_MODE_1_CTRL,0b0);      // HB1 is not mapped to PWM channel 1
  }

void activateMotor1_6()
{
    // The motors 1-6 are activated in one direction for 1s
    // Then the motors 1-6  are braked to GND for 300 ms
    // Then the motors 1-6 are activated in the other direction for 1s
    // Then the mtors 1-6 are braked to GND for 300 ms
    
    writeRegister(HB_ACT_1_CTRL, 0b10011001); // LS1,LS3,HS2,HS4 activated    : Motors 1&2 are turned on
    writeRegister(HB_ACT_2_CTRL, 0b10011001); // LS5,LS7,HS6,HS8 activated    : Motors 3&4 are turned on 
    writeRegister(HB_ACT_3_CTRL, 0b10011001); // LS9,LS11,HS10,HS12 activated : Motors 5&6 are turned on 
    delay(1000);                              // wait 1000 ms
    writeRegister(HB_ACT_1_CTRL, 0b01010101); // LS1,LS2,LS3,LS4 activated    : Motors 1&2 are braked to GND
    writeRegister(HB_ACT_2_CTRL, 0b01010101); // LS5,LS6,LS7,LS8 activated    : Motors 3&4 are braked to GND
    writeRegister(HB_ACT_3_CTRL, 0b01010101); // LS9,LS10,LS11,LS12 activated : Motors 5&6 are braked to GND
    delay(300);                               // wait 300 ms
    writeRegister(HB_ACT_1_CTRL, 0b01100110); // LS2,LS4,HS1,HS3 activated    : Motors 1&2 are turned on in the reverse direction
    writeRegister(HB_ACT_2_CTRL, 0b01100110); // LS6,LS8,HS5,HS7 activated    : Motors 3&4 are turned on in the reverse direction
    writeRegister(HB_ACT_3_CTRL, 0b01100110); // LS10,LS12,HS9,HS11 activated : Motors 5&6 are turned on in the reverse direction
    delay(1000);                              // wait 1000 ms
    writeRegister(HB_ACT_1_CTRL, 0b01010101); // LS1,LS2,LS3,LS4 activated    : Motors 1&2 are braked to GND
    writeRegister(HB_ACT_2_CTRL, 0b01010101); // LS5,LS6,LS7,LS8 activated    : Motors 3&4 are braked to GND
    writeRegister(HB_ACT_3_CTRL, 0b01010101); // LS9,LS10,LS11,LS12 activated : Motors 5&6 are braked to GND
    delay(300);                               // wait 300 ms
}

int writeRegister(byte address, byte data)
{
/*   This function sends SPI commands to the address  "address" with the data "data"
 *   The data sent by the TLE94112EL to the Arduino Uno (SDO) and the data sent by the Arduino Uno to the TLE94112EL (SDI) are reported in the
 *   Serial Monitor (CTRL + SHIFT + M)
 */
  address = address | WRITE ;
  digitalWrite(CSN, LOW);               // take the CSN pin low to select the chip:
  byte byte0 = SPI.transfer(address);   // send address byte to the TLE94112 and store the received byte to byte 0
  byte byte1 = SPI.transfer(data);      // send data byte to the TLE94112 and store the received byte to byte 1
  digitalWrite(CSN, HIGH);              // take the CSN pin high to terminate the SPI frame
  Serial.println();                     // insert a carriage return to the serial monitor

  Serial.print("SDI:\t");
  printByte(address);                   // write address to the serial monitor
  printByte(data);                      // write data to the serial monitor
  Serial.print("SDO:\t");
  printByte(byte0);                     // write byte0 (Global Satus Register of the TLE94112EL) to the serial monitor
  printByte(byte1);                     // write byte1 (Data Byte sent by the TLE9412EL) to the serial monitor
  int result = ( ((int)byte0 << 8) | (int)byte1 ); // concatenates byte0 and byte1 in result
  return (result);
}

int readRegister(byte address)          // This function reads a register and returns the data sent by the TLE94112EL. Data sent and received by the TLE94112EL are displayed on the serial monitor
{
  digitalWrite(CSN, LOW);               // take the CSN pin low to select the chip:
  byte byte0 = SPI.transfer(address);   // send Low Byte
  byte byte1 = SPI.transfer(0);         // send High Byte
  digitalWrite(CSN, HIGH);              // take the CSN pin high to de-select the chip:
  Serial.println();
  
  Serial.print("SDI:\t");
  printByte(address);                   // write address to the serial monitor
  printByte(0);                         // write data to the serial monitor
  Serial.print("SDO:\t");
  printByte(byte0);                     // write byte0 (Global Satus Register of the TLE94112EL) to the serial monitor
  printByte(byte1);                     // write byte1 (Data Byte sent by the TLE9412EL) to the serial monitor
  int result = ( ((int)byte0 << 8) | (int)byte1 ); // concatenates byte0 and byte1 in result
  return (result);
}

int clearRegister(int address)          // This function clears a register and returns the data sent by the TLE94112EL. Data sent and received by the TLE94112EL are displayed on the serial monitor
{
  address = address | CLEAR ;
  digitalWrite(CSN, LOW);               // take the CSN pin low to select the chip:
  byte byte0 = SPI.transfer(address);
  byte byte1 = SPI.transfer(0);
  digitalWrite(CSN, HIGH);              // take the CSN pin high to de-select the chip:
  Serial.println();
  
  Serial.print("SDI:\t");
  printByte(address);                   // write address to the serial monitor
  printByte(0);                         // write data to the serial monitor
  Serial.print("SDO:\t");               
  printByte(byte0);                     // write byte0 (Global Satus Register of the TLE94112EL) to the serial monitor
  printByte(byte1);                     // write byte1 (Data Byte sent by the TLE9412EL) to the serial monitor
  int result = ( ((int)byte0 << 0) | (int)byte1 ); // concatenates byte0 and byte1 in result
  return (result);
}

void printByte(byte printByte)          // This function writes to the serial monitor the value of the byte printByte in binary form 
{
  for (int i = 7; i >= 0; i--)
  {
    Serial.print(bitRead(printByte, i));
  }
  Serial.print("\t");
}
