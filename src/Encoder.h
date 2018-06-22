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

#ifndef ENCODER_H_
#define ENCODER_H_

#include "Value.h"

class Encoder {
public:
	// ValueRegisters
	AValue 	_ppr;
	AValue 	_count;
	AValue 	_rpm;
	int		_lastCount;
	// Used by ISR
	int 	_countEdge;  // count on A edge
	bool 	_lastEdgeA;
public:
	void Init();
	void Run();
	void CalcRPM();
};

class EncoderManager
{
public:
	Encoder _e1;
	Encoder _e2;
public:
	void 	Init();
	void 	RunISR();
	void 	CalckRPM();
	void 	Run();
};

extern EncoderManager gEncoders;

#endif /* ENCODER_H_ */
