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

#ifndef SERVO_H_
#define SERVO_H_

#include "Value.h"

enum {
	kSERVO_1 = 0,
	kSERVO_2 = 1,
	kSERVO_3 = 2,
	kSERVO_Count
};

class Servo {
public:
	AValue	_active;
	AValue	_position;
	//AValue  _minPosition;
	//AValue  _maxPosition;
public:
	//void CheckRegs(int servoNum);
	//void SetMinRange(int min);
	//void SetMaxRange(int max);
};

class ServoManager {
public:
	Servo	_servos[kSERVO_Count];
	AValue	_gpio;
public:
	void Init();
	void Run();
	void Stop(int ch);
	void SetPeriod();
	//void SetPosition(int ch, int pos);

};

extern ServoManager gServos;

#endif // SERVO_H_
