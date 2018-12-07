/*
Copyright (c) 2018 Trashbots, Inc. - SDG

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Hardware.h"
#include "BQ24195.h"

// Register Index
enum
{
	reg00,
	reg01,
	reg02,
	reg03,
	reg04,
	reg05,
	reg06,
	reg07,
	reg08,	//RO
	reg09,	//RO
	reg0A,	//RO
	regBQMAX
};

typedef struct
{
	uint8_t regaddr;
	uint8_t regval;
} BQregs_t;

BQregs_t m_BQreg[regBQMAX] =
{	// Power up defaults
	// regAddr, regVal(reset)
	{0x00, 0x30},
	{0x01, 0x1B},
	{0x02, 0x60},
	{0x03, 0x11},
	{0x04, 0xB2},
	{0x05, 0x9A},
	{0x06, 0x03},
	{0x07, 0x4B},
	{0x08, 0x00},		// Read-Only
	{0x09, 0x00},		// Read-Only
	{0x0A, 0x23},		// Read-Only
};

// I2C Slave Address
#define BQADDRESS7		0x6B			// 7 bit I2C address from datasheet
#define BQADDRESS8		(BQADDRESS7<<1)	// bits[7..1]=0x6B + bit[0]=Rd/Wr#

// Register Defaults
static uint8_t m_Buf[2];  // All registers are 8 bit one byte for addr, one for value.

int BQReadRegister(int reg)
{
	int rc = I2C0_ReadReg(BQADDRESS8, m_BQreg[reg].regaddr, m_Buf, 1);
	if (rc -= rcOK) {
		return m_Buf[0];
	} else {
		return 0;
	}
}

void BQWriteRegister(int reg, int value)
{
	I2C0_WriteReg(BQADDRESS8, m_BQreg[reg].regaddr, m_Buf, 1);
}

/*----------------------------------------------------------------------------
Name: BQ_Init
Desc: Initialize the BQ24195
Outs: Return rcXXX
/ ---------------------------------------------------------------------------*/
void BQ_Init(void)
{
	uint8_t rc;

	// Do a communications test by reading Register REG0A
	rc = I2C0_ReadReg(BQADDRESS8, m_BQreg[reg0A].regaddr, m_Buf, 1);
	if (rc==rcOK) {
		if (m_Buf[0] == m_BQreg[reg0A].regval) {
			return;
		}
	}
}

/*----------------------------------------------------------------------------
BQ24195 5V MANAGEMENT
/ ---------------------------------------------------------------------------*/
static int bg5VTimeoutCount = 0;
static int bq5VEnabled = false;
#define BQ_WATCHDOG_MAX   (10)

// BQ_5VUsagePing - Indicate the 5V system is needed. This also includes
// HBridge that uses the 5V system.
void BQ_5VUsagePing()
{
	// Bump 5V timeout counter to max
	bg5VTimeoutCount = BQ_WATCHDOG_MAX;
	if (!bq5VEnabled) {
		BQ_5VEnable(true);
	}
}

// BQ_5VCheckTimeout - See if the 5V system is still needed. This is
// typically checked every second. The subs system is disabled once idle
// for a while (based on BQ_WATCHDOG_MAX)
void BQ_5VCheckTimeout()
{
	// If the motors are not running now, definitely a good time to check.
	// They might have run a short period that is not seen here, but each time
	// they are turned on the watch dog is reset to its max value.
	if (bg5VTimeoutCount > 0) {
		bg5VTimeoutCount--;
	} else if (bq5VEnabled) {
		BQ_5VEnable(false);
	}
}

// BQ_5VEnable - Enable or disable the 5V boost converter for motors and servos.
// charging can only happen when the converter is disabled.
void BQ_5VEnable(bool enable)
{
	uint8_t val;

	// See design PDF - Section 8.3.3.1 Autonomous Charging Cycle
	// the 5V boost is enabled when ...
	// REG01: bits[5:4]=CHG_CONFIG[1:0] = 11 for OTG
	//									  01 for charge battery

	#define CHG_CONFIG_MASK				(0x03<<4)
	#define CHG_CONFIG_OTG 			(0x03<<4)
	#define CHG_CONFIG_ChargeBat 	(0x01<<4)

	val = BQReadRegister(reg01);

	val &= ~CHG_CONFIG_MASK;
	if (enable) {
		val |= CHG_CONFIG_OTG;
	} else {
		val |= CHG_CONFIG_ChargeBat;
	}

	BQWriteRegister(reg01, val);

	// The H-Bridge uses the 5V system so they are logically connected.
	// though if only servos are used this would not be used.
	GPIO_Write(MPWR_ENA, enable ? 1 : 0);
	bq5VEnabled = enable;
}

/*----------------------------------------------------------------------------
Name: BQ_WDenable
Desc: Watchdog
Ins	: enable	0=off 1=on
Outs: Return rcXXX
/ ---------------------------------------------------------------------------*/
void BQ_WatchdogEnable(bool enable)
{
	// the WD is enabled/disabled when ...
	// REG05: bits[5:4]=WATCHDOG[1:0] = 00 for disable
	//									01 for 40s
	#define WATCHDOG				(0x03<<4)
	#define WATCHDOG_DISABLE 		(0x00<<4)
	#define WATCHDOG_ENABLE 		(0x01<<4)

	int val = BQReadRegister(reg05);

	val &= ~WATCHDOG;
	if (enable) {
		val |= WATCHDOG_ENABLE;
	} else {
		val |= WATCHDOG_DISABLE;
	}

	BQWriteRegister(reg05, val);
}

/*----------------------------------------------------------------------------
Name: BQ_IinLim
Desc: Set the Input current (from USB) limit
Ins	: mA	100ma...3000mA
But the Rlim resistor of 330 ohms sets the max to 1.6A
Outs: Return rcXXX
/ ---------------------------------------------------------------------------*/
void BQ_IinLim(uint16_t mA_Bits)
{
	int val = BQReadRegister(reg00);

	val &= ~kBQ_IinLimit_Mask;
	val |= mA_Bits;

	BQWriteRegister(reg00, val);
}

/*----------------------------------------------------------------------------
Name: BQ_ChargeStatus
Desc: Return charging status
Ins	:
Outs: 00-disabled, 01-pre-charge, 10-fast-charge, 11-charging-done
/ ---------------------------------------------------------------------------*/
uint8_t BQ_ChargeStatus()
{
	#define CHARGE_STATE			(0x03<<4)

	int val = BQReadRegister(reg08);
	return val & CHARGE_STATE;
}

