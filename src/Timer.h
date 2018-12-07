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

#include <stdint.h>

enum
{
	TIMER_SERVO1,
	TIMER_SERVO2,
	TIMER_SERVO3,
	TIMER_SPICHAR,
	TIMER_NOTE,
	MAX_TIMERS
};

class Timer {
private:
	bool _2ms;
	bool _10ms;
	bool _100ms;
	bool _500ms;
	bool _1sec;
	int _hwTicks;
	int _2msTicks;
	int _10msTicks;
	int _100msTicks;
	int _500msTicks;
public:
	bool is_2msec() 	{ bool f = _2ms; _2ms = false; return f; };
	bool is_10msec() 	{ bool f = _10ms; _10ms = false; return f; };
	bool is_100msec() 	{ bool f = _100ms; _100ms = false; return f; };
	bool is_500msec() 	{ bool f = _500ms; _500ms = false; return f; };
	bool is_1sec()		{ bool f = _1sec; _1sec = false; return f; };
	void hwTick();
};
extern Timer gTimer;

// -- Time Section --
void Time_StartTimer( uint8_t timernum );
bool Time_isTimeout( uint8_t timernum, uint32_t timeout_ms );
uint32_t Time_CheckTime(uint8_t timernum);
