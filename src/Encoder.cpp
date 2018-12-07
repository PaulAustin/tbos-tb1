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

/*------------------------------------------------------------------
/ ----------------------------------------------------------------*/

//------------------------------------------------------------------
void Encoder::Init() {
	_ppr.Set(1400);
	_count.Set(0);
	_rpm.Set(0);
}

void Encoder::CalcRPM()
{
	int current = _count.Get();
	int rpm = ( current - _lastCount) * 10 / _ppr.Get();
	_rpm.Set(rpm);
	_lastCount = current;
}

void EncoderManager::Init(void)
{
	gRMap.SetValueObj(kRM_Motor1Encoder, &_e1._count);
	gRMap.SetValueObj(kRM_Motor1Rpm, &_e1._rpm);

	gRMap.SetValueObj(kRM_Motor2Encoder, &_e2._count);
	gRMap.SetValueObj(kRM_Motor2Rpm, &_e2._rpm);
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
	if (_e1._lastEdgeA != qA) {
		_e1._lastEdgeA = qA;
		if (qA) {
			delta = qB ? 1 : -1; 	// Rising edge on A
		} else {
			delta = qB ? -1 : 1; 	// Falling edge on A
		}
		_e1._countEdge += delta;
	}

	qA = GPIO_READ(gpio_ENC2_QA);
	qB = GPIO_READ(gpio_ENC2_QB);
	if (_e2._lastEdgeA != qA) {
		_e2._lastEdgeA = qA;
		if (qA) {
			delta = qB ? 1 : -1; 	// Rising edge on A
		} else {
			delta = qB ? -1 : 1; 	// Falling edge on A
		}
		_e2._countEdge += delta;
	}
}

/*----------------------------------------------------------------------------
Name: Encoder_Run
/ ---------------------------------------------------------------------------*/
void EncoderManager::Run(void)
{
	// See if encoder clear register was written to.
	if ( _resetTrigger.HasAsyncSet() ) {
		_e1._countEdge = 0;
		_e2._countEdge = 0;
	}

	// Map low lever ISR value to registers
	_e1._count.Set(_e1._countEdge);
	_e2._count.Set(_e2._countEdge);
	_e1._count.HasAsyncSet();
	_e2._count.HasAsyncSet();
}

void EncoderManager::CalcRPM()
{
	_e1.CalcRPM();
	_e2.CalcRPM();
}

