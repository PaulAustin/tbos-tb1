
#include <stdint.h>
#include "em_gpio.h"

void HW_Init(void);
void HW_Timer1_Enable(bool enable);
void HW_Timer1_SetFreq(uint16_t freq);
void HW_Timer2_Enable(bool enable);
void HW_Timer2_SetPeriod_us(uint16_t per);
void HW_Timer2_SetPW_us(uint8_t ch, uint16_t pw);
void GPIO_Write(uint8_t id, uint8_t out);
bool GPIO_Read(uint8_t id);

int I2C0_ReadReg(uint8_t addr, uint8_t reg, uint8_t * pBuf, uint8_t len);
int I2C0_WriteReg(uint8_t addr, uint8_t reg, uint8_t * pBuf, uint8_t len);


#define SYSCLK					24000000L			// Hz, System Clock
#define HFPERCLK				SYSCLK				// Hz, HF Peripheral Clock
#define	TICK_FREQ 				8000				// Timer Tick (Hz)
#define TICK_PERIOD_us			(1000000L/TICK_FREQ)// Timer Tick Period (ms)

// --- GPIO Configuration ---
typedef struct
{
	GPIO_Port_TypeDef port;
	uint8_t pin;
	GPIO_Mode_TypeDef mode;
	uint8_t out;
} gpioConfig_t;


enum
{
	rcOK,
	rcERR,
	rcTIMEOUT,
	rcBQ_InitFail,
	rcBQ_Fail,
};

// --- GPIO ID's ---
enum
{
	ENC1_QA,
	ENC1_QB,
	MPWR_ENA,
	SERVO1,
	SERVO2,
	SERVO3,
	ENC2_QA,
	ENC2_QB,
	BEEP,
	XTP,
	XTN,
	IO6,
	IO7,
	IO8,
	IO9,
	CHG_STAT,
	MOT_NSLEEP,
	SPK_EN, // used to be MOT_NFAULT
	CHG_INT,
	USB_DM,
	USB_DP,
	O4,
	O5,
	SDA,
	SCL,
	MOSI,
	MISO,
	SCK,
	P16_CS1,
	MOT1_F,
	MOT1_R,
	MOT2_F,
	MOT2_R,
	gpioConfig_MAX
};

static const gpioConfig_t gpio_O4     = {gpioPortD, 4, gpioModePushPull, 0};
static const gpioConfig_t gpio_O5 	  = {gpioPortD, 5, gpioModePushPull, 0};
static const gpioConfig_t gpio_MOT1_F = {gpioPortF, 2, gpioModePushPull, 0};
static const gpioConfig_t gpio_MOT1_R = {gpioPortF, 3, gpioModePushPull, 0};
static const gpioConfig_t gpio_MOT2_F = {gpioPortF, 4, gpioModePushPull, 0};
static const gpioConfig_t gpio_MOT2_R = {gpioPortF, 5, gpioModePushPull, 0};
static const gpioConfig_t gpio_BEEP   = {gpioPortB, 11, gpioModePushPull, 0};
static const gpioConfig_t gpio_IO7    = {gpioPortC, 1, gpioModeInput, 0};

//{gpioPortC, 1, gpioModeInput, 0},		//	IO7,

static const gpioConfig_t gpio_ENC1_QA = {gpioPortA, 0, gpioModeInput, 1};
static const gpioConfig_t gpio_ENC1_QB = {gpioPortA, 1, gpioModeInput, 1};
static const gpioConfig_t gpio_ENC2_QA = {gpioPortB, 7, gpioModeInput, 1};
static const gpioConfig_t gpio_ENC2_QB = {gpioPortB, 8, gpioModeInput, 1};

#define GPIO_SET(_PIN_) BUS_RegBitWrite(&GPIO->P[_PIN_.port].DOUT, _PIN_.pin, 1)
#define GPIO_CLR(_PIN_) BUS_RegBitWrite(&GPIO->P[_PIN_.port].DOUT, _PIN_.pin, 0)
#define GPIO_WRITE(_PIN_, _VALUE_) BUS_RegBitWrite(&GPIO->P[_PIN_.port].DOUT, _PIN_.pin, _VALUE_)
#define GPIO_READ(_PIN_) BUS_RegBitRead(&GPIO->P[_PIN_.port].DIN, _PIN_.pin);


