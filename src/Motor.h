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

#ifndef MOTOR_H_
#define MOTOR_H_

#include "Value.h"

class Motor
{
public:
	AValue 	_power;
	AValue 	_break;
	int32_t _lastEncoderCount;
	int32_t _targetDeltaCount;
	int32_t _cumulativeError;
	uint8_t _pwmHighF;
	uint8_t _pwmHighR;
public:
	void CheckRegs();
	void SetPower(int power);
	void SetBreak(bool state);
};

class MotorManager {
	Motor _m1;
	Motor _m2;
public:
	void Init();
	void Run();
	bool Idle();
	void RunISR();
};

extern MotorManager gMotors;

#endif // MOTOR_H_

