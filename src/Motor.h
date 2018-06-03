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

typedef struct
{
	int32_t lastEncoderCount;
	int32_t targetDeltaCount;
	int32_t cumulativeError;
	uint8_t pwmHighF;
	uint8_t pwmHighR;
} motor_t;

enum {
	kMOTOR_1=0,
	kMOTOR_2=1,
	kMOTOR_Count
};

class MotorManager {
	AValue _power1;
	AValue _power2;
	AValue _break1;
	AValue _break2;
	motor_t _motor[kMOTOR_Count];

public:
	void Init();
	void Run();
	bool Idle();
	void SetPower(int motor, int power);
	void SetBreak(int motor, bool state);
	void RunISR();
};

extern MotorManager gMotors;

#endif // MOTOR_H_

