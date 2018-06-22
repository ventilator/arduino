/* Pro Micro Test Code
 by: Nathan Seidle
 modified by: Jim Lindblom
 SparkFun Electronics
 date: September 16, 2013
 license: Public Domain - please use this code however you'd like.
 It's provided as a learning tool.

This code is provided to show how to control the SparkFun
 ProMicro's TX and RX LEDs within a sketch. It also serves
 to explain the difference between Serial.print() and
 Serial1.print().
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>




//Register variables

unsigned char OCR1AL_reg = 0x88; //output compare low 8 bits for Counter 1
unsigned char OCR1AH_reg = 0x89; //output compare high 8 bits for Counter 1

unsigned char TCCR1B_reg = 0x81; //Timer/Counter0 Control Register B 
unsigned char TCCR1A_reg = 0x80; //Timer/Counter0 Control Register A

unsigned char TIMSK1_reg = 0x6F; //Timer/Counter0 Interrupt Mask Register

unsigned char TIFR1_reg = 0x36; //Timer/Counter0 Interrupt Flag Register

unsigned char TCNT1H_reg = 0x85; //Timer/Counter0 Interrupt Flag Register
unsigned char TCNT1L_reg = 0x84; //Timer/Counter0 Interrupt Flag Register




unsigned char SREG_reg = 0x5F; //Status register used for disabling and enabling global interrupts

int stopeverything = 0;

void setup()
{

Serial.begin(9600); //This pipes to the serial monitor

//resetting the Timer/Counter1
 (*(volatile unsigned char*)(TCNT1H_reg)) = 0;
 (*(volatile unsigned char*)(TCNT1L_reg)) = 0; 
 
 //disabling all global interrupts
 (*(volatile unsigned char *)(SREG_reg)) = 0b00000000;

//defining prescalar
 (*(volatile unsigned char *)(TCCR1B_reg)) = 0b01001101; //the last 3 bits set to 101 will make the clock scalar 1024;
 //setting up CTC mode
 (*(volatile unsigned char *)(TCCR1A_reg)) = 0b10000000; //the last 3 bits set to 101 will make the clock scalar 1024;

//defining the output compare value which is calculated to be 3906 assuming 8MHz clock speed: OCRNA = (fClk_I/O)/(2*N*fOCRNA) where N is the prescalar fOCRNA is the compare value and fClk_I/O is the clock speed
 //the value calculated for fOCRNA to be 1Hz is ~3096
 //(*(volatile unsigned char *)(OCR1AH_reg)) = 0b00001111; //writing to high first which will write to a temp register first
 //(*(volatile unsigned char *)(OCR1AL_reg)) = 0b01000010; //writing to low will write both high and low in the same clock cycle

//defining the output compare value which is calculated to be 3906 assuming 8MHz clock speed: OCRNA = (fClk_I/O)/(2*N*fOCRNA) where N is the prescalar fOCRNA is the compare value and fClk_I/O is the clock speed
 //the value calculated for fOCRNA to be 0.1Hz is ~30960
 (*(volatile unsigned char *)(OCR1AH_reg)) = 0b11110001; //writing to high first which will write to a temp register first
 //(*(volatile unsigned char *)(OCR1AH_reg)) = 0b11111111; //writing to high first which will write to a temp register first
 (*(volatile unsigned char *)(OCR1AL_reg)) = 0b11100000; //writing to low will write both high and low in the same clock cycle
 //(*(volatile unsigned char *)(OCR1AL_reg)) = 0b11111111; //writing to low will write both high and low in the same clock cycle




(*(volatile unsigned char *)(TIMSK1_reg)) = 0b00100010; //writing so that output compare A is set up

//enable global interrupts
 (*(volatile unsigned char*)(SREG_reg)) = 0b10000000;







}

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
 Serial.println("interrupt!");
 stopeverything=1;
}




void loop()
{
 Serial.println(
 (*(volatile unsigned char*)TCNT1H_reg << 8) | (*(volatile unsigned char*)TCNT1L_reg),BIN);
 //Serial.println("");
 if(stopeverything==1){
 Serial.println("Coming from interrupt!!");
 delay(2000);
 stopeverything=0;
 }
}
