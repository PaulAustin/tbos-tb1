//============================================================================
//  Title	: Motor.c
//  Desc	: Motor/Encoder Functions
//  2017-07-24	Paul Austin
//============================================================================

#include <stdlib.h>
#include "Hardware.h"
#include "SpiRegisterMap.h"
#include "BQ24195.h"
#include "Motor.h"
#include "Encoder.h"

//#include "SpiMem.h"
//#include "SpiRegisterMap.h"

#define mask_MOTOR2_Break 		(1<<7)
#define mask_MOTOR1_Break 		(1<<6)
#define mask_MOTOR2_Forward 	(1<<5)
#define mask_MOTOR1_Forward 	(1<<4)
#define mask_MOTOR2_Run 		(1<<1)
#define mask_MOTOR1_Run 		(1<<0)

// Motor / encoder speed control using SW DIO for h-bridge control
// and SW DIO for encoder reading. Math indicates that sampling at 4kHz
// should be OK.

uint8_t pwmWidth;
uint8_t pwmTic;


typedef struct
{
	int32_t lastEncoderCount;
	int32_t targetDeltaCount;
	int32_t cumulativeError;
	uint8_t pwmHighF;
	uint8_t pwmHighR;
} motor_t;

motor_t m_Motor[kMOTOR_Count];
MotorManager gMotor;

void MotorManager::Init(void)
{
	pwmWidth = 10;
	pwmTic = 0;

	gRMap.SetValueObj(kRM_Motor1Power, &_power1);
	gRMap.SetValueObj(kRM_Motor2Power, &_power2);

	gRMap.SetValueObj(kRM_Motor1Break, &_break1);
	gRMap.SetValueObj(kRM_Motor2Break, &_break2);


	// Clear GPI for the HBridge so no power to the motor.
	GPIO_Write(MOT1_F, 0);
	GPIO_Write(MOT1_R, 0);
	GPIO_Write(MOT2_F, 0);
	GPIO_Write(MOT2_R, 0);
	GPIO_Write(MOT_NSLEEP, 1);
}

#define MOTOR_TUNING_INTERVAL 20
#define MOTOR_CPR 420 // counts per revolution.

/*----------------------------------------------------------------------------
Name: Check_Motor_Power
Desc: At a slower rate see if there is a reason to keep the 5V enabled for
the motors.
/ ---------------------------------------------------------------------------*/
bool MotorManager::Idle()
{
	return (gMotor._power1.Read() == 0 && gMotor._power2.Read() == 0);
}

/*----------------------------------------------------------------------------
Name: Motor_Run
Desc: Motor/Encoder State Machine
	  Called every 1msec for control operations
	  Operates the motors, read the encoders
/ ---------------------------------------------------------------------------*/
void MotorManager::Run()
{
	int w1,w2 = 0;

	if (_power1.Updated()) {
		w1 = _power1.Read();
		SetPower(kMOTOR_1, w1);
	}

	if (_power2.Updated()) {
		w2 = _power2.Read();
		SetPower(kMOTOR_2, w2);
	}
}



/*------------------------------------------------------------------
Motor_SetPWM - Motor/Encoder State Machine
	  Called at highest rate possible to poll encoder edges and
	  to generate pulse chain for power control.
/ ----------------------------------------------------------------*/
void MotorManager::SetPower(int motorIndex, int power) {
	int ticEdge = abs(power+5) / 10;

	motor_t* pMotor = & m_Motor[motorIndex];

	if (ticEdge > 10)
		ticEdge = 10;

	if (power != 0) {
		BQ_5VUsagePing();
	}

	// pwmHigh is %0-%100 power level, need to map more carefully
	// Set the 0 first to avoid the ISR seeing both set.
	if (power > 0) {
		pMotor->pwmHighR = 0;
		pMotor->pwmHighF = ticEdge;
	} else if ( power < 0 ){
		pMotor->pwmHighF = 0;
		pMotor->pwmHighR = ticEdge;
	} else {
		// coast
		pMotor->pwmHighR = 0;
		pMotor->pwmHighF = 0;
	}
}

/*------------------------------------------------------------------
Name: Motor_RunISR
Desc: Motor/Encoder State Machine
	  Called at highest rate possible to poll encoder edges and
	  to generate pulse chain for power control.
/ ----------------------------------------------------------------*/
void MotorManager::RunISR()
{
	// Motors share the same primary PWM counter, the duty cycle
	// is specific to each motor and direction. The PWM values
	// are configured so the F or R will always be low, unless
	// the motors are stopped with break mode.
	pwmTic++;
	if (pwmTic >= pwmWidth) {
		pwmTic = 0;
	}
	GPIO_WRITE(gpio_MOT1_F, m_Motor[kMOTOR_1].pwmHighF > pwmTic);
	GPIO_WRITE(gpio_MOT1_R, m_Motor[kMOTOR_1].pwmHighR > pwmTic);
	GPIO_WRITE(gpio_MOT2_F, m_Motor[kMOTOR_2].pwmHighF > pwmTic);
	GPIO_WRITE(gpio_MOT2_R, m_Motor[kMOTOR_2].pwmHighR > pwmTic);

}
