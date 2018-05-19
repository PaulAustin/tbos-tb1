/*============================================================================
 Title	: Spi0.c
 Desc	: SPI on USART0
 2017-05-21	DaraiusH
/ ===========================================================================*/

#include "em_usart.h"
#include "Hardware.h"
#include "SpiRegisterMap.h"

#define SPICHAR_TIMEOUT_ms	90
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

	// Get the incoming Char
	inByte = (int8_t)(USART0->RXDATA & 0xFF);

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
			gRMap.ASet(gReg, gParam);
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
