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

#define SPICHAR_TIMEOUT_ms	90
#define SPICHAR_ACK			0xAC

RegisterMap gRMap;
Value		gDummyValue;

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

static ValueReader gV8Reader;
static int gReg = 0;
static int gParam = 0;
static int gPacketState = psCommand;
static int gIntOut = 0;
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
	if(Time_isTimeout(TIMER_SPICHAR, SPICHAR_TIMEOUT_ms))
	{
		gPacketState = psCommand;
		USART0->CMD |= USART_CMD_CLEARTX | USART_CMD_CLEARRX;	// Clear the TX and RX FIFO's
	}
	//GPIO_Write(IO6, 0);
	//Restart the timer
	Time_StartTimer(TIMER_SPICHAR);

	inByte = (int8_t)(USART0->RXDATA & 0xFF);	// Get the incoming

	if (gV8Reader.ReadV8(inByte)) {
		switch(gPacketState) {
		case psCommand:
			gReg = gV8Reader._v;
			if (gReg > 0 && gReg < kRM_Count) {
				gPacketState = psSetReg;
			} else if (gReg < 0 && gReg > -kRM_Count) {
				gPacketState = psGetReg;
				gIntOut = gRMap._registers[-gReg]->Get();
			} else {
				gReg = 0;
				// stay in command state
			}
			break;
		case psSetReg:
			// Once the value has been fully read
			// write it into the map.
			gParam = gV8Reader.Value();
			gRMap._registers[gReg]->AsyncSet(gParam);
			gReg = gParam = 0;
			gPacketState = psCommand;
			break;
		case psGetReg:
			// Incoming bytes are all single byte values
			// counting down to 0
			if (inByte == 0) {
				// Must have clocked out the last byte.
				// reset the state and load a zero for the next
				// output
				gPacketState = psCommand;
			} else {
				// line up a new byte to send.
				gIntOut = gIntOut >> 8;
			}
			break;
		}
	}

	USART0->TXDATA = (int8_t) (gIntOut & 0xff);
	USART_IntClear (USART0, USART_IF_RXDATAV);
}
