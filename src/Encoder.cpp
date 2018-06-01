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
	for (int i = 0; i < kENCODER_Count; i++) {
		if (_encoders[i]._count.HasAsyncSet()) {
			// If register was written to then update the low level value.
			// this is typically used to reset the counter.
			_encoders[kENCODER_1]._countEdge = _encoders[kENCODER_1]._count.Get();
		} else {
			// Other wise copy level counter to register.
			_encoders[kENCODER_1]._count.Set(_encoders[kENCODER_1]._countEdge);
		}
	}
}

/*------------------------------------------------------------------
/ ----------------------------------------------------------------*/
// TODO: not done/tested
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

