/*
 *	Arduino library to control Infineon's DC Motor Control Shield with TLE94112
 *
 *	The shield contains twelve independent halfbridges, 
 *	so it can drive up to 6 indipendent (+5 cascaded) bidirectional DC motor(s). 
 *	Each halfbridge provides a high-Voltage (nominal 5.5-18 V) tristate output,  
 *	which is also capable of PWM with 3 different frequencies. 
 *
 *	Have a look at the datasheet for more information. 
 */
 
/*!	\file TLE94112.cpp
 *	\brief This file defines functions and predefined instances from TLE94112.h
 */

#include "TLE94112.h"
#include "./util/tle94112_conf.h"

//SPI address commands
#define TLE94112_CMD_WRITE			0x80;
#define TLE94112_CMD_CLEAR			0x80;
//standard pin assignment
#define TLE94112_PIN_CS1			10
#define TLE94112_PIN_CS2			9
#define TLE94112_PIN_EN				8
#define TLE94112_CS_RISETIME		2

#define TLE94112_STATUS_INV_MASK	(Tle94112::TLE_POWER_ON_RESET)

//predefined instances
Tle94112 tle94112(&SPI, TLE94112_PIN_CS1, TLE94112_PIN_EN);
Tle94112 tle94112_2(&SPI, TLE94112_PIN_CS2, TLE94112_PIN_EN);


Tle94112::Tle94112(void)
{
	Tle94112(&SPI, TLE94112_PIN_CS1, TLE94112_PIN_EN);
}

Tle94112::Tle94112(SPIClass *bus, uint8_t cs, uint8_t en)
{
	mEnabled = FALSE;
	mBus = bus;
	mCsPin = cs;
	mEnPin = en;
}

void Tle94112::begin(void)
{
	mEnabled = TRUE;
	mBus->begin();
	mBus->setBitOrder(LSBFIRST); 
	mBus->setClockDivider(SPI_CLOCK_DIV16);
	mBus->setDataMode(SPI_MODE1);
	pinMode(mEnPin, OUTPUT);
	pinMode(mCsPin, OUTPUT);
	digitalWrite(mCsPin, HIGH);
	digitalWrite(mEnPin, HIGH);
	init();
}

void Tle94112::end(void)
{
	mEnabled = FALSE;
	digitalWrite(mCsPin, HIGH);
	digitalWrite(mEnPin, LOW);
}

void Tle94112::configHB(HalfBridge hb, HBState state, PWMChannel pwm)
{
	configHB(hb, state, pwm, 0);
}

void Tle94112::configHB(HalfBridge hb, HBState state, PWMChannel pwm, uint8_t activeFW)
{
	if(mEnabled == TRUE) 
	{
		configHB(static_cast<uint8_t>(hb), 
					static_cast<uint8_t>(state),
					static_cast<uint8_t>(pwm), 
					activeFW );
	}
}

void Tle94112::configHB(uint8_t hb, uint8_t state, uint8_t pwm, uint8_t activeFW)
{
	uint8_t reg = mHalfBridges[hb].stateReg;
	uint8_t mask = mHalfBridges[hb].stateMask;
	uint8_t shift = mHalfBridges[hb].stateShift;
	writeReg(reg, mask, shift, state);
	
	reg = mHalfBridges[hb].pwmReg;
	mask = mHalfBridges[hb].pwmMask;
	shift = mHalfBridges[hb].pwmShift;
	writeReg(reg, mask, shift, pwm);
	
	reg = mHalfBridges[hb].fwReg;
	mask = mHalfBridges[hb].fwMask;
	shift = mHalfBridges[hb].fwShift;
	writeReg(reg, mask, shift, pwm);
}

void Tle94112::configPWM(PWMChannel pwm, PWMFreq freq, uint8_t dutyCycle)
{
	if(mEnabled == TRUE) 
	{
		configPWM(static_cast<uint8_t>(pwm), 
					static_cast<uint8_t>(freq),
					dutyCycle );
	}
}

void Tle94112::configPWM(uint8_t pwm, uint8_t freq, uint8_t dutyCycle)
{
	uint8_t reg = mPwmChannels[pwm].freqReg;
	uint8_t mask = mPwmChannels[pwm].freqMask;
	uint8_t shift = mPwmChannels[pwm].freqShift;
	writeReg(reg, mask, shift, freq);
	
	reg = mPwmChannels[pwm].dcReg;
	mask = mPwmChannels[pwm].dcMask;
	shift = mPwmChannels[pwm].dcShift;
	writeReg(reg, mask, shift, dutyCycle);
}

uint8_t Tle94112::getSysDiagnosis()
{
	uint8_t ret = readStatusReg(SYS_DIAG1);
	return ret^TLE94112_STATUS_INV_MASK;
}

uint8_t Tle94112::getSysDiagnosis(DiagFlag mask)
{
	return getSysDiagnosis(static_cast<uint8_t>(mask));
}

uint8_t Tle94112::getSysDiagnosis(uint8_t mask)
{
	uint8_t ret = readStatusReg(SYS_DIAG1, mask, 0);
	return ret^(TLE94112_STATUS_INV_MASK&mask);
}

uint8_t Tle94112::getHBOverCurrent(HalfBridge hb)
{
	return getHBOverCurrent(static_cast<uint8_t>(hb));
}

uint8_t Tle94112::getHBOverCurrent(uint8_t hb)
{
	uint8_t reg = mHalfBridges[hb].ocReg;
	uint8_t mask = mHalfBridges[hb].ocMask;
	uint8_t shift = mHalfBridges[hb].ocShift;
	return readStatusReg(reg, mask, shift);
}

uint8_t Tle94112::getHBOpenLoad(HalfBridge hb)
{
	return getHBOpenLoad(static_cast<uint8_t>(hb));
}

uint8_t Tle94112::getHBOpenLoad(uint8_t hb)
{
	uint8_t reg = mHalfBridges[hb].olReg;
	uint8_t mask = mHalfBridges[hb].olMask;
	uint8_t shift = mHalfBridges[hb].olShift;
	return readStatusReg(reg, mask, shift);
}

void Tle94112::clearErrors()
{
	clearStatusReg(SYS_DIAG1);
	clearStatusReg(OP_ERROR_1_STAT);
	clearStatusReg(OP_ERROR_2_STAT);
	clearStatusReg(OP_ERROR_3_STAT);
	clearStatusReg(OP_ERROR_4_STAT);
	clearStatusReg(OP_ERROR_5_STAT);
	clearStatusReg(OP_ERROR_6_STAT);
}

void Tle94112::writeReg(uint8_t reg, uint8_t mask, uint8_t shift, uint8_t data)
{
	uint8_t address = mCtrlRegAddresses[reg];
	uint8_t toWrite = mCtrlRegData[reg] & (~mask);
	toWrite |= (data << shift) & mask;
	mCtrlRegData[reg] = toWrite;
	
	address = address | TLE94112_CMD_WRITE;
	digitalWrite(mCsPin, LOW);
	uint8_t byte0 = mBus->transfer(address);
	uint8_t byte1 = mBus->transfer(toWrite);
	digitalWrite(mCsPin, HIGH);
	delay(TLE94112_CS_RISETIME);
}

uint8_t Tle94112::readStatusReg(uint8_t reg)
{
	//read the whole register
	return readStatusReg(reg, 0xFF, 0);
}

uint8_t Tle94112::readStatusReg(uint8_t reg, uint8_t mask, uint8_t shift)
{
	uint8_t address = mStatusRegAddresses[reg];
	
	digitalWrite(mCsPin, LOW);
	uint8_t byte0 = SPI.transfer(address);
	uint8_t received = SPI.transfer(0xFF);	//send dummy byte while receiving
	digitalWrite(mCsPin, HIGH); 
	delay(TLE94112_CS_RISETIME);
	
	received = (received & mask) >> shift;
	
	return received;
}

void Tle94112::clearStatusReg(uint8_t reg)
{
	uint8_t address = mStatusRegAddresses[reg];

	address = address | TLE94112_CMD_CLEAR;
	digitalWrite(mCsPin, LOW);
	uint8_t byte0 = mBus->transfer(address);
	uint8_t byte1 = mBus->transfer(0);	//clear register by writing 0x00
	digitalWrite(mCsPin, HIGH);
	delay(TLE94112_CS_RISETIME);
}

