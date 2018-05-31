//============================================================================
//  Title	: Motor.c
//  Desc	: Motor/Encoder Functions
//  2017-07-24	Paul		Created (based on servo.c)
//============================================================================
#include "Hardware.h"
#include "SpiRegisterMap.h"
#include "Encoder.h"

EncoderManager gEncoders;


void EncoderManager::Init(void)
{
	gRMap.SetValueObj(kRM_Motor1Encoder, &_encoders[kENCODER_1]._count);
	gRMap.SetValueObj(kRM_Motor1Rpm, &_encoders[kENCODER_1]._rpm);

	gRMap.SetValueObj(kRM_Motor2Encoder, &_encoders[kENCODER_2]._count);
	gRMap.SetValueObj(kRM_Motor2Rpm, &_encoders[kENCODER_2]._rpm);

	for (int i = 0; i < kENCODER_Count; i++) {
		_encoders[i]._ppr.Set(1400);
		_encoders[i]._count.Set(0);
		_encoders[i]._rpm.Set(0);
	}
}

/*----------------------------------------------------------------------------
Name: Encoder_RunISR
Desc: Motor/Encoder State Machine
	  Called at highest rate possible to poll encoder edges and
	  to generate pulse chain for power control.
/ ---------------------------------------------------------------------------*/
void EncoderManager::RunISR()
{
	int32_t delta = 0;

	bool qA = GPIO_READ(gpio_ENC1_QA);
	bool qB = GPIO_READ(gpio_ENC1_QB);
	if (_encoders[kENCODER_1]._lastEdgeA != qA) {
		_encoders[kENCODER_1]._lastEdgeA = qA;
		if (qA) {
			delta = qB ? 1 : -1; 	// Rising edge on A
		} else {
			delta = qB ? -1 : 1; 	// Falling edge on A
		}
		_encoders[kENCODER_1]._countEdge += delta;
	}

	qA = GPIO_READ(gpio_ENC2_QA);
	qB = GPIO_READ(gpio_ENC2_QB);
	if (_encoders[kENCODER_2]._lastEdgeA != qA) {
		_encoders[kENCODER_2]._lastEdgeA = qA;
		if (qA) {
			delta = qB ? 1 : -1; 	// Rising edge on A
		} else {
			delta = qB ? -1 : 1; 	// Falling edge on A
		}
		_encoders[kENCODER_2]._countEdge += delta;
	}
}

/*----------------------------------------------------------------------------
Name: Encoder_Run
/ ---------------------------------------------------------------------------*/
void EncoderManager::Run(void)
{
	// See if encoder clear register was written to.
	for (int i = 0; i < kENCODER_Count; i++) {
		if (_encoders[i]._reset.HasAsyncSet())
		_encoders[i]._countEdge = 0;
	}

	// Map low lever ISR value to registers
	_encoders[kENCODER_1]._count.Set(_encoders[kENCODER_1]._countEdge);
	_encoders[kENCODER_2]._count.Set(_encoders[kENCODER_2]._countEdge);
}

/*------------------------------------------------------------------
/ ----------------------------------------------------------------*/
void EncoderManager::CalckRPM(int dt)
{
	for (int i = kENCODER_1; i <= kENCODER_2; i++) {
		Encoder* pE = &_encoders[i];
		int current = pE->_count.Get();
		int rpm = ( current - pE->_lastCount) * 10 / pE->_ppr.Get();
		pE->_rpm.Set(rpm);
		pE->_lastCount = current;
	}
}

