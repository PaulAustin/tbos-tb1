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

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

#define MAX_UPTIMERS  2

class Timer {
private:
	bool flg_5ms;
	bool flg_100ms;
	bool flg_500ms;
	bool flg_1sec;
	int ticks_hw;
	int ticks_5ms;
	int ticks_100ms;
	int ticks_500ms;


public:
	uint32_t uptimer[MAX_UPTIMERS];
	bool is_5msec() {
		if (flg_5ms) {
			flg_5ms = false;
			return true;
		} else {
			return false;
		}
	};
	bool is_100msec() {
		if (flg_100ms) {
			flg_100ms = false;
			return true;
		} else {
			return false;
		}
	};
	bool is_500msec() {
		if (flg_500ms) {
			flg_500ms = false;
			return true;
		} else {
			return false;
		}
	};

	bool is_1sec() {
		if (flg_1sec) {
			flg_1sec = false;
			return true;
		} else {
			return false;
		}
	};

	void hwTick();



};

	void Time_StartTimer(uint8_t timerNum);
	bool Time_isTimeOut(uint8_t timerNum, uint32_t timout_ms);
	uint32_t Time_CheckTime(uint8_t timerNum);



extern Timer gTimer;

#endif // TIMER_H_
