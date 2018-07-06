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

#include "em_usart.h"
#include "Hardware.h"
#include "SpiRegisterMap.h"
#include "Timer.h"

#define TIMER_SPICHAR 		0
#define SPICHAR_TIMEOUT_ms 	90
#define SPICHAR_ACK			0xAC

RegisterMap gRMap;
AValue		gDummyValue;

void RegisterMap::Init()
{
	for (int i = 0; i < kRM_Count; i++) {
		_registers[i] = &gDummyValue;
	}
}

// super simple protocol to get things working
// Write start with a high bit set.
// Following bytes don't have high bit set.
// no need for time out, not yet any way.

enum {
	psCommand = 0,
	psSetReg = 1,
	psGetReg = 2
};

static ValueReaderWriter gV8RW;
static int gReg = 0;
static int gParam = 0;
static int gPacketState = psCommand;
volatile static unsigned int gIntOut = 0;
static bool s_enumflag=0;
static int byteAmount = 0;
static int bytesRemaining = 0;
static int value = 0;

//static char  outHack[] = {0x00, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26};

extern "C" void USART0_RX_IRQHandler(void)
{

	int	inByte;
	if (!(USART0->STATUS & USART_STATUS_RXDATAV)) {
		// this is not a "RX Data Valid" interrupt
		return;
	}

	//Checks if the timer has gone over 50 ms
	//GPIO_Write(IO6,1);
	if(Time_isTimeOut(TIMER_SPICHAR,SPICHAR_TIMEOUT_ms))
	{
		gPacketState = psCommand;
		USART0->CMD |= USART_CMD_CLEARTX | USART_CMD_CLEARRX;	// Clear the TX and RX FIFO's
	}
	//GPIO_Write(IO6, 0);
	//Restart the timer
	Time_StartTimer(TIMER_SPICHAR);

	inByte = (int8_t)(USART0->RXDATA & 0xFF);	// Get the incoming

	switch(gPacketState)
	{
	case psCommand:
		gReg = (int)inByte;
		if (gReg > 0 && gReg < kRM_Count) {
			gPacketState = psSetReg;
			s_enumflag = 1;
		}
		else if (gReg < 0 && gReg > -kRM_Count) {
			gPacketState = psGetReg;
			gIntOut = 0;
			s_enumflag = 1;
		}
		else {
			gReg = 0;
		}
		break;

	case psSetReg:
		if(s_enumflag)
		{
			if (inByte == 0)
			{
				bytesRemaining = 1;
				byteAmount = 1;
				value = 0;
			}
			else if (inByte == EVT8_Int16)
			{
					value = 0;
					bytesRemaining = 2;
					byteAmount = 2;
			}
			else if (inByte == EVT8_Int32)
			{
					value = 0;
					bytesRemaining = 4;
					byteAmount = 4;
			}
			else
			{
				// unknown enum
			}
			s_enumflag = 0;
		}
		else {
			if(bytesRemaining > 0)
			{
				inByte = (inByte & 0x00ff);
				value = value | (inByte << (8 * (byteAmount-bytesRemaining)));
				bytesRemaining--;
			}
			if(bytesRemaining == 0){
				gParam = value;
				gRMap.ASet(gReg, gParam);
				gReg = gParam = 0;
				bytesRemaining = byteAmount = 0;
				gPacketState = psCommand;
				gIntOut = SPICHAR_ACK;
			}
		}
		break;

	case psGetReg:
		if (s_enumflag){
			// Get the low byte ready to go no matter what the enum is
			gIntOut = gRMap._registers[-gReg]->Get();
			// Enum for how many more bytes are coming
			switch(inByte)
			{
			case 0:
				bytesRemaining = 1;
				break;
			case EVT8_Int16:
				bytesRemaining = 2;
				break;
			case EVT8_Int32:
				bytesRemaining = 4;
				break;
			default:
				gIntOut = SPICHAR_ACK;
				gPacketState = psCommand;
				break;
			}
			s_enumflag=0;
		}
		else {
			bytesRemaining--;
			if (bytesRemaining)
				gIntOut = gIntOut >> 8;
			else {
				gIntOut = SPICHAR_ACK;
				gPacketState = psCommand;
			}
		}
		break;
	}


	USART0->TXDATA = (int8_t) (gIntOut & 0xff);
	USART_IntClear (USART0, USART_IF_RXDATAV);
	//GPIO_Write(IO7, 0);
}




