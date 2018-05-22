#include "em_cmu.h"
#include "em_timer.h"
#include "em_gpio.h"
#include "em_int.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_usart.h"
#include "em_i2c.h"

#include "Hardware.h"
#include "Value.h"

void HW_HFXO_Init(void);
void HW_CMU_Init(void);
void HW_PORTIO_Init(void);
void HW_USART0_Init(void);
void HW_GPIO_Init(void);
void HW_Timer0_Init(void);
void HW_Timer1_Init(void);
void HW_Timer2_Init(void);
void I2C0_Init(void);

I2C_TransferReturn_TypeDef m_I2C_Status;

static const gpioConfig_t gpioC[] =
{	// port, pin mode, out
	{gpioPortA, 0, gpioModeInput, 1},		//	ENC1_QA,
	{gpioPortA, 1, gpioModeInput, 1},		//	ENC1_QB,
	{gpioPortA, 2, gpioModePushPull, 0},	//	MPWR_ENA,
	{gpioPortA, 8, gpioModePushPull, 0},	//	SERVO1,
	{gpioPortA, 9, gpioModePushPull, 0},	//	SERVO2,
	{gpioPortA, 10, gpioModePushPull, 0},	//	SERVO3,
	{gpioPortB, 7, gpioModeInput, 1},		//	ENC2_QA,
	{gpioPortB, 8, gpioModeInput, 1},		//	ENC2_QB,
	{gpioPortB, 11, gpioModePushPull, 0},	//	BEEP,
	{gpioPortB, 13, gpioModeInput, 0},		//	XTP,
	{gpioPortB, 14, gpioModeInput, 0},		//	XTN,
	{gpioPortC, 0, gpioModePushPull, 0},		//	IO6,
	{gpioPortC, 1, gpioModeInput, 0},		//	IO7,
	{gpioPortC, 2, gpioModeInput, 0},		//	IO8,
	{gpioPortC, 3, gpioModeInput, 0},		//	IO9,
	{gpioPortC, 4, gpioModeInput, 0},		//	CHG_STAT,
	{gpioPortC, 8, gpioModePushPull, 0},	//	MOT_NSLEEP,
	{gpioPortC, 9, gpioModePushPull, 0},		//	SPK_EN,
	{gpioPortC, 10, gpioModeInput, 0},		//	CHG_INT,
	{gpioPortC, 14, gpioModeInput, 0},		//	USB_DM,
	{gpioPortC, 15, gpioModeInput, 0},		//	USB_DP,
	{gpioPortD, 4, gpioModePushPull, 0},	//	O4,
	{gpioPortD, 5, gpioModePushPull, 0},	//	O5,
	{gpioPortD, 6, gpioModeWiredAndPullUp, 1},	//	SDA,
	{gpioPortD, 7, gpioModeWiredAndPullUp, 1},	//	SCL,
	{gpioPortE, 10, gpioModeInput, 0},		//	MOSI,
	{gpioPortE, 11, gpioModePushPull, 0},	//	MISO,
	{gpioPortE, 12, gpioModeInput, 0},		//	SCK,
	{gpioPortE, 13, gpioModeInput, 0},		//	P16_CS1,
	{gpioPortF, 2, gpioModePushPull, 0},	//	MOT1_F,
	{gpioPortF, 3, gpioModePushPull, 0},	//	MOT1_R,
	{gpioPortF, 4, gpioModePushPull, 0},	//	MOT2_F,
	{gpioPortF, 5, gpioModePushPull, 0},	//	MOT2_R
};


void HW_Init()
{
	INT_Disable();

	CHIP_Init();			// Chip specific Errata Init

	HW_HFXO_Init();			// Start the external crystal oscillator

	HW_CMU_Init();			// Individual Peripheral Clocks are enabled as needed

	HW_USART0_Init();		// SPI Slave from micro:bit

	I2C0_Init();			// I2C0 port <I2C0.c>

	HW_PORTIO_Init();		// Config pin routing.

	HW_GPIO_Init();			// Setup all GPIO ports/pins

	HW_Timer0_Init();		// Timer Tick at TICK_FREQ, main timer interrupt

	HW_Timer1_Init();		// Use CC2 for Audio Generation, no interrupts

	HW_Timer2_Init();		// Use CC0 CC1 for Servo PWM, no Interrupts

	INT_Enable(); 			// Global interrupt Enable
}

//----------------------------------------------------------------------------
// Name	: HW_HFXO_Init
// Desc	: Setup the HF Crystal Oscillator
//----------------------------------------------------------------------------
void HW_HFXO_Init(void)
{
	CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;

	hfxoInit.boost = _CMU_CTRL_HFXOBOOST_50PCENT;
	hfxoInit.mode =  cmuOscMode_Crystal;

	CMU_HFXOInit(&hfxoInit);

	SystemHFXOClockSet(SYSCLK);	// Set system HFXO frequency
}

//----------------------------------------------------------------------------
// Name	: HW_CMU_Init
// Desc	:
//----------------------------------------------------------------------------
void HW_CMU_Init(void)
{
	// $[High Frequency Clock select]
	/* Using HFXO as high frequency clock, HFCLK */
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	/* Enable peripheral clock */
	CMU_ClockEnable(cmuClock_HFPER, true);
	// [High Frequency Clock select]$

	// $[LF clock tree setup]
	/* No LF peripherals enabled */
	// [LF clock tree setup]$

	// $[Peripheral Clock enables]
	/* Enable clock for I2C0 */
	CMU_ClockEnable(cmuClock_I2C0, true);

	/* Enable clock for USART0 */
	CMU_ClockEnable(cmuClock_USART0, true);

	// [Peripheral Clock enables]$
}
//----------------------------------------------------------------------------
// Name	: HW_Timer0_Init
// Desc	: Setup Timer0 to generate a timer tick ISR
//		  ISR is TIMER0_IRQHandler in <Utils.c>
//----------------------------------------------------------------------------
void HW_Timer0_Init(void)
{
	#define TIMER0_TOP (HFPERCLK/TICK_FREQ)

	TIMER_Init_TypeDef t0init;

	t0init.enable = true,                  		// Start timer upon configuration
	t0init.debugRun = true,   					// Keep timer running even on debug halt
	t0init.prescale = timerPrescale1, 			// Use /1 prescaler.
	t0init.clkSel = timerClkSelHFPerClk,			// Set HF peripheral clock as clock source
	t0init.count2x = false,						// Enable 2X count mode
	t0init.ati = false,							// Enable to make CCPOL always track polarity of inputs
	t0init.fallAction = timerInputActionNone,		// No action on falling edge
	t0init.riseAction = timerInputActionNone,		// No action on rising edge
	t0init.mode = timerModeUp,					// Use up-count mode
	t0init.dmaClrAct = false,						// Not using DMA
	t0init.quadModeX4 = false,					// Not using quad decoder
	t0init.oneShot = false,						// Using continuous, not one-shot
	t0init.sync = false, 							// Not synchronizing timer operation off of other timers

	CMU_ClockEnable(cmuClock_TIMER0, true);   	// Enable TIMER peripheral clock
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);   	// Enable Timer overflow interrupt
    NVIC_SetPriority(TIMER0_IRQn, 1);			// Set priority lower than USART0
	NVIC_EnableIRQ(TIMER0_IRQn);       			// Enable TIMER0 interrupt vector in NVIC
	TIMER_Init(TIMER0, &t0init);       	// Configure and start Timer
	TIMER_TopSet(TIMER0,  TIMER0_TOP);			// Set timer TOP value
}


//----------------------------------------------------------------------------
// Name	: HW_Timer1_Init
// Desc	: Setup Timer1: CC2 to drive the Beeper with a Square Wave
//		  HFPERCLK >> div-4 >> 16 bit counter
//		  Counter wraps at TIMER1_TOP and Toggles the CC2 output
//		  TImer1Top will be changed to get different frequencies
//		  No Interrupt
//----------------------------------------------------------------------------
void HW_Timer1_Init(void)
{
	#define TIMER1_TOP (1000)

	TIMER_Init_TypeDef t1init;
	t1init.enable = false,                  		// Don't Start timer upon configuration
	t1init.debugRun = false,   					// Keep timer running even on debug halt
	t1init.prescale = timerPrescale16, 			// Use /16 prescaler.
	t1init.clkSel = timerClkSelHFPerClk,			// Set HF peripheral clock as clock source
	t1init.count2x = false,						// Enable 2X count mode
	t1init.ati = false,							// Enable to make CCPOL always track polarity of inputs
	t1init.fallAction = timerInputActionNone,		// No action on falling edge
	t1init.riseAction = timerInputActionNone,		// No action on rising edge
	t1init.mode = timerModeUp,					// Use up-count mode
	t1init.dmaClrAct = false,						// Not using DMA
	t1init.quadModeX4 = false,					// Not using quad decoder
	t1init.oneShot = false,						// Using continuous, not one-shot
	t1init.sync = false, 							// Not synchronizing timer operation off of other timers

	CMU_ClockEnable(cmuClock_TIMER1, true);   	// Enable TIMER peripheral clock
	TIMER_IntDisable(TIMER1, TIMER_IF_OF);   	// Disable Timer overflow interrupt
	NVIC_DisableIRQ(TIMER1_IRQn);       		// Disable TIMER1 interrupt vector in NVIC
	TIMER_Init(TIMER1, &t1init);       	// Configure and start Timer

	TIMER_TopSet(TIMER1,  TIMER1_TOP);              		 // Set timer TOP value

	TIMER_InitCC_TypeDef initCC2 = TIMER_INITCC_DEFAULT;

	initCC2.eventCtrl = timerEventEveryEdge;
	initCC2.edge = timerEdgeRising;
	initCC2.prsSel = timerPRSSELCh0;
	initCC2.cufoa = timerOutputActionNone;
	initCC2.cofoa = timerOutputActionToggle;	// Counter overflow >> Toggle output!
	initCC2.cmoa = timerOutputActionNone;
	initCC2.mode = timerCCModeCompare;
	initCC2.filter = 0;
	initCC2.prsInput = false;
	initCC2.coist = 0;
	initCC2.outInvert = 0;

	TIMER_InitCC(TIMER1, 2, &initCC2);

	// -- Route Pins --
	// Module TIMER1 is configured to location 3
	TIMER1->ROUTE = (TIMER1->ROUTE & ~_TIMER_ROUTE_LOCATION_MASK) | TIMER_ROUTE_LOCATION_LOC3;
	// Enable signals CC2
	TIMER1->ROUTE |= TIMER_ROUTE_CC2PEN;
}

//----------------------------------------------------------------------------
// Name	: HW_Timer1_Enable
// Desc	: Enable or Disable Timer1
//----------------------------------------------------------------------------
void HW_Timer1_Enable(bool enable)
{
	TIMER_Enable(TIMER1, enable);
}

//----------------------------------------------------------------------------
// Name	: HW_Timer1_SetFreq
// Desc	: Set the Timer Top value to generate the specified frequency
//		  Ffrg = ScaledClock / (TimerNtop *2)
//			- ScaledClock = (SYSCLOCK/16)
//
//		  Top = SYSCLK/(16*Ffrq*2)
//----------------------------------------------------------------------------
void HW_Timer1_SetFreq(uint16_t freq)
{
	uint32_t top = (SYSCLK/32)/freq;
	TIMER_TopBufSet(TIMER1, top);	// use buffered version
	// TODO buffered version so this mean the couner will reload
	// at the end of this count?
}

//----------------------------------------------------------------------------
// Name	: HW_Timer2_Init
// Desc	: Setup Timer1: CC2 to drive the Beeper with a Square Wave
//		  HFPERCLK >> div-4 >> 16 bit counter
//		  Counter wraps at TIMER1_TOP and Toggles the CC2 output
//		  TImer1Top will be changed to get different frequencies
//		  No Interrupt
//----------------------------------------------------------------------------
void HW_Timer2_Init(void)
{
	#define TIMER2_TOP (0xFFFF)

	TIMER_Init_TypeDef t2init;            		// Setup Timer initialization
	t2init.enable = true,                  		// Start timer upon configuration
	t2init.debugRun = false,   					// Keep timer running even on debug halt
	t2init.prescale = timerPrescale8, 			// prescaler
	t2init.clkSel = timerClkSelHFPerClk,		// Set HF peripheral clock as clock source
	t2init.count2x = false,						// Enable 2X count mode
	t2init.ati = false,							// Enable to make CCPOL always track polarity of inputs
	t2init.fallAction = timerInputActionNone,	// No action on falling edge
	t2init.riseAction = timerInputActionNone,	// No action on rising edge
	t2init.mode = timerModeUp,					// Use up-count mode
	t2init.dmaClrAct = false,					// Not using DMA
	t2init.quadModeX4 = false,					// Not using quad decoder
	t2init.oneShot = false,						// Using continuous, not one-shot
	t2init.sync = false, 						// Not synchronizing timer operation off of other timers

	CMU_ClockEnable(cmuClock_TIMER2, true);   	// Enable TIMER peripheral clock
	TIMER_IntDisable(TIMER2, TIMER_IF_OF);   	// Disable Timer overflow interrupt
	NVIC_DisableIRQ(TIMER2_IRQn);       		// Disable TIMER1 interrupt vector in NVIC
	TIMER_Init(TIMER2, &t2init);       	// Configure and start Timer

	TIMER_TopSet(TIMER2,  TIMER2_TOP);			// Set timer TOP value

	TIMER_InitCC_TypeDef initCC = TIMER_INITCC_DEFAULT;

	initCC.eventCtrl = timerEventRising;
	initCC.edge = timerEdgeNone;
	initCC.prsSel = timerPRSSELCh0;
	initCC.cufoa = timerOutputActionNone;
	initCC.cofoa = timerOutputActionNone;
	initCC.cmoa = timerOutputActionNone;
	initCC.mode = timerCCModePWM;
	initCC.filter = 0;
	initCC.prsInput = false;
	initCC.coist = 0;
	initCC.outInvert = 0;

	// Same init for CC0 CC1 CC2
	TIMER_InitCC(TIMER2, 0, &initCC);
	TIMER_InitCC(TIMER2, 1, &initCC);
	TIMER_InitCC(TIMER2, 2, &initCC);

	// Write default PW's to 1000us
	//HW_Timer2_SetPW_us(SERVO1_CC, 1000);
	//HW_Timer2_SetPW_us(SERVO2_CC, 1000);
	//HW_Timer2_SetPW_us(SERVO3_CC, 1000);

	// -- Route Pins --
	// Module TIMER2 is configured to location 0
	TIMER2->ROUTE = (TIMER2->ROUTE & ~_TIMER_ROUTE_LOCATION_MASK) | TIMER_ROUTE_LOCATION_LOC0;

	// Enable signals CC0, CC1, CC2
	TIMER2->ROUTE |= TIMER_ROUTE_CC0PEN | TIMER_ROUTE_CC1PEN | TIMER_ROUTE_CC2PEN;
}

//----------------------------------------------------------------------------
// Name	: HW_Timer2_Enable
// Desc	: Enable or Disable Timer1
//----------------------------------------------------------------------------
void HW_Timer2_Enable(bool enable)
{
	TIMER_Enable(TIMER2, enable);
}

//----------------------------------------------------------------------------
// Name	: HW_Timer2_SetPeriod_ms
// Desc	: Set the Timer PWM Period
//		  Period = TOP * (1/ScaledClock)
//		  TOP = ScaledClock * Period
//		  TOP = (SYSCLK/8) * Period
//----------------------------------------------------------------------------
void HW_Timer2_SetPeriod_us(uint16_t per)
{
	uint16_t top;
	top = per * (SYSCLK/(8*1000000L));
	TIMER_TopBufSet(TIMER2, top);	// Set timer TOP value
}

//----------------------------------------------------------------------------
// Name	: HW_Timer2_SetPW_us
// Desc	: Set the Timer CCx Compare Count Value (CCV) to generate the pulsewidth
//		  PW = CCV / ScaledClock
//		  PW = CCV * 8 / SYSCLOCK
//
//		  CCV = PW * SYSCLK / 8
// Ins	: ch	0, 1, 2 for CCx.  Use SERVOx_CC
//		  pw	pulse width in usec
//----------------------------------------------------------------------------
void HW_Timer2_SetPW_us(uint8_t ch, uint16_t pw)
{
	uint32_t ccv = pw * (SYSCLK/(8*1000000L));

	TIMER_CompareBufSet(TIMER2, ch, ccv);
}


//----------------------------------------------------------------------------
// Name	: HW_GPIO_Init
// Desc	: Setup all GPIO based on the gpioC struct
//----------------------------------------------------------------------------
void HW_GPIO_Init(void)
{
	uint8_t i;

	CMU_ClockEnable(cmuClock_GPIO, true);     // Enable GPIO peripheral clock

	// Configure GPIO pins
	for (i=0; i < COUNT_OF(gpioC); i++)
	{
		GPIO_PinModeSet(gpioC[i].port, gpioC[i].pin, gpioC[i].mode, gpioC[i].out);
	}
}

//----------------------------------------------------------------------------
// Name	: GPIO_Write
// Desc	: Write a GPIO based on its ID
// Ins	: id	GPIO ID as defined in <hardware.h>
//		  out	0, 1, 2=toggle
//----------------------------------------------------------------------------
void GPIO_Write(uint8_t id, uint8_t out)
{
	switch(out)
	{
	case 0:		// OFF
		GPIO_PinOutClear(gpioC[id].port, gpioC[id].pin);
		break;
	case 1:		// ON
		GPIO_PinOutSet(gpioC[id].port, gpioC[id].pin);
		break;
	case 2:		// Toggle
		GPIO_PinOutToggle(gpioC[id].port, gpioC[id].pin);
		break;
	}
}

//----------------------------------------------------------------------------
// Name	: GPIO_Read
// Desc	: Read a GPIO based on its ID
// Ins	: id	GPIO ID as defined in <hardware.h>
//----------------------------------------------------------------------------
bool GPIO_Read(uint8_t id)
{
	return( GPIO_PinInGet(gpioC[id].port, gpioC[id].pin) );
}

//================================================================================
// HW_PORTIO_Init
//================================================================================
void HW_PORTIO_Init(void) {

	// $[Port A Configuration]

	/* Pin PA8 is configured to Push-pull */
	GPIO->P[0].MODEH = (GPIO->P[0].MODEH & ~_GPIO_P_MODEH_MODE8_MASK)
			| GPIO_P_MODEH_MODE8_PUSHPULL;

	/* Pin PA9 is configured to Push-pull */
	GPIO->P[0].MODEH = (GPIO->P[0].MODEH & ~_GPIO_P_MODEH_MODE9_MASK)
			| GPIO_P_MODEH_MODE9_PUSHPULL;

	/* Pin PA10 is configured to Push-pull */
	GPIO->P[0].MODEH = (GPIO->P[0].MODEH & ~_GPIO_P_MODEH_MODE10_MASK)
			| GPIO_P_MODEH_MODE10_PUSHPULL;
	// [Port A Configuration]$

	// $[Port B Configuration]
	// [Port B Configuration]$

	// $[Port C Configuration]
	// [Port C Configuration]$

	// $[Port D Configuration]

	/* Pin PD6 is configured to Open-drain with pull-up and filter */
	GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE6_MASK)
			| GPIO_P_MODEL_MODE6_WIREDANDPULLUPFILTER;

	/* Pin PD7 is configured to Open-drain with pull-up and filter */
	GPIO->P[3].MODEL = (GPIO->P[3].MODEL & ~_GPIO_P_MODEL_MODE7_MASK)
			| GPIO_P_MODEL_MODE7_WIREDANDPULLUPFILTER;
	// [Port D Configuration]$

	// $[Port E Configuration]

	/* Pin PE10 is configured to Input enabled */
	GPIO->P[4].MODEH = (GPIO->P[4].MODEH & ~_GPIO_P_MODEH_MODE10_MASK)
			| GPIO_P_MODEH_MODE10_INPUT;

	/* Pin PE11 is configured to Push-pull */
	GPIO->P[4].MODEH = (GPIO->P[4].MODEH & ~_GPIO_P_MODEH_MODE11_MASK)
			| GPIO_P_MODEH_MODE11_PUSHPULL;

	/* Pin PE12 is configured to Input enabled */
	GPIO->P[4].MODEH = (GPIO->P[4].MODEH & ~_GPIO_P_MODEH_MODE12_MASK)
			| GPIO_P_MODEH_MODE12_INPUT;

	/* Pin PE13 is configured to Input enabled */
	GPIO->P[4].MODEH = (GPIO->P[4].MODEH & ~_GPIO_P_MODEH_MODE13_MASK)
			| GPIO_P_MODEH_MODE13_INPUT;
	// [Port E Configuration]$

	// $[Port F Configuration]
	// [Port F Configuration]$

	// $[Route Configuration]

	/* Module I2C0 is configured to location 1 */
	I2C0->ROUTE = (I2C0->ROUTE & ~_I2C_ROUTE_LOCATION_MASK)
			| I2C_ROUTE_LOCATION_LOC1;

	/* Enable signals SCL, SDA */
	I2C0->ROUTE |= I2C_ROUTE_SCLPEN | I2C_ROUTE_SDAPEN;

	/* Module TIMER1 is configured to location 3 */
	TIMER1->ROUTE = (TIMER1->ROUTE & ~_TIMER_ROUTE_LOCATION_MASK)
			| TIMER_ROUTE_LOCATION_LOC3;

	/* Enable signals CC2 */
	TIMER1->ROUTE |= TIMER_ROUTE_CC2PEN;

	/* Enable signals CC0, CC1, CC2 */
	TIMER2->ROUTE |= TIMER_ROUTE_CC0PEN | TIMER_ROUTE_CC1PEN
			| TIMER_ROUTE_CC2PEN;

	/* Enable signals CLK, CS, RX, TX */
	USART0->ROUTE |= USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_RXPEN
			| USART_ROUTE_TXPEN;
	// [Route Configuration]$

}

//================================================================================
// USART0_enter_DefaultMode_from_RESET
//================================================================================
void HW_USART0_Init(void) {

	// $[USART_InitAsync]
	// [USART_InitAsync]$

	// $[USART_InitSync]
	USART_InitSync_TypeDef initsync = USART_INITSYNC_DEFAULT;

	initsync.baudrate = 115200;
	initsync.databits = usartDatabits8;
	initsync.master = 0;
	initsync.msbf = 1;
	initsync.clockMode = usartClockMode0;
#if defined( USART_INPUT_RXPRS ) && defined( USART_TRIGCTRL_AUTOTXTEN )
	initsync.prsRxEnable = 0;
	initsync.prsRxCh = usartPrsRxCh0;
	initsync.autoTx = 0;
#endif

	USART_InitSync(USART0, &initsync);
	// [USART_InitSync]$

	// $[USART_InitPrsTrigger]
	USART_PrsTriggerInit_TypeDef initprs = USART_INITPRSTRIGGER_DEFAULT;

	initprs.rxTriggerEnable = 0;
	initprs.txTriggerEnable = 0;
	initprs.prsTriggerChannel = usartPrsTriggerCh0;

	USART_InitPrsTrigger(USART0, &initprs);
	// [USART_InitPrsTrigger]$

	/* Setup receive interrupt */
	USART0->CMD = USART_CMD_CLEARRX;
	NVIC_ClearPendingIRQ (USART0_RX_IRQn);
	NVIC_EnableIRQ(USART0_RX_IRQn);
	USART_IntEnable(USART0, USART_IEN_RXDATAV);
}

//----------------------------------------------------------------------------
// Name	: I2C0_Init
// Desc	: I2C0
//----------------------------------------------------------------------------
void I2C0_Init(void)
{
	I2C_Init_TypeDef init = I2C_INIT_DEFAULT;

	// Enable clock for I2C0 */
	CMU_ClockEnable(cmuClock_I2C0, true);

	// In some situations, after a reset during an I2C transfer, the slave
	// device may be left in an unknown state. Send 9 clock pulses to
	// set slave in a defined state.
	for (uint8_t i = 0; i < 9; i++) {
		///PFA GPIO_Write(SCL, 1);
		///PFA GPIO_Write(SCL, 0);
	}

	// Module I2C0 is configured to location 1
	I2C0->ROUTE = (I2C0->ROUTE & ~_I2C_ROUTE_LOCATION_MASK)	| I2C_ROUTE_LOCATION_LOC1;

	// Enable signals SCL, SDA
	I2C0->ROUTE |= I2C_ROUTE_SCLPEN | I2C_ROUTE_SDAPEN;

	// Setup the Peripheral
	init.enable = true;
	init.master = true;
	init.refFreq = 0; // not used
	init.freq = I2C_FREQ_STANDARD_MAX;
	init.clhr = i2cClockHLRStandard;
	I2C_Init(I2C0, &init);

	// No Interrupts
	NVIC_ClearPendingIRQ(I2C0_IRQn);
	NVIC_DisableIRQ(I2C0_IRQn);
}

//----------------------------------------------------------------------------
// Name	: I2C0_ReadReg
// Desc	: Read a Register
// Ins	: addr
//		  reg	Register Address in I2C slave
//		  pBuf	Read Data returned here
//		  len	num of bytes to read
//----------------------------------------------------------------------------
int I2C0_ReadReg(uint8_t addr, uint8_t reg, uint8_t * pBuf, uint8_t len)
{
	I2C_TransferSeq_TypeDef seq;

	// Setup the sequence
	seq.addr = addr;
	seq.flags = I2C_FLAG_WRITE_READ;

	// Setup outgoing data (just an 8 bit register address)
	seq.buf[0].data = &reg;	// ptr to outgoing data
	seq.buf[0].len = 1;		// len of outgoing data

	 // Setup incoming destination
	seq.buf[1].data = pBuf;
	seq.buf[1].len = len;

	// Do a polled transfer
	m_I2C_Status = I2C_TransferInit(I2C0, &seq);
	while (m_I2C_Status == i2cTransferInProgress) {
		m_I2C_Status = I2C_Transfer(I2C0);
	}

	if (m_I2C_Status != i2cTransferDone) {
		return(rcERR);
	} else {
		return(rcOK);
	}
}

//----------------------------------------------------------------------------
// Name	: I2C0_WriteReg
// Desc	: Write a Register
// Ins	: addr  I2C address in 8 bit format, where LSB is reserved for R/W bit
//		  reg	Register Address in I2C slave
//		  pBuf	Data to write
//		  len	num of bytes to write
//----------------------------------------------------------------------------
int I2C0_WriteReg(uint8_t addr, uint8_t reg, uint8_t * pBuf, uint8_t len)
{
	I2C_TransferSeq_TypeDef seq;
	#define MAXLEN	8
	uint8_t data[1+MAXLEN];  // RegsiterAddr + MAXLEN

	if (len > MAXLEN) return(rcERR);

	// Setup the sequence
	seq.addr = addr;
	seq.flags = I2C_FLAG_WRITE;

	// Setup outgoing data (just an 8 bit register address)
	data[0] = reg;
	for (uint8_t i=0; i < len; i++) {
		data[1+i] = pBuf[i];
	}
	seq.buf[0].data = data;	// ptr to outgoing data
	seq.buf[0].len = 1+len;	// len of outgoing data

	// Do a polled transfer
	m_I2C_Status = I2C_TransferInit(I2C0, &seq);
	while (m_I2C_Status == i2cTransferInProgress) {
		m_I2C_Status = I2C_Transfer(I2C0);
	}

	if (m_I2C_Status != i2cTransferDone) {
		return(rcERR);
	} else {
		return(rcOK);
	}
}


