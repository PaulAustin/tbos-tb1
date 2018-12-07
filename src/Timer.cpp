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
#include "em_timer.h"
#include "em_int.h"		// we use INT_Disable/Enable
#include "em_gpio.h"

#include "Hardware.h"
#include "Timer.h"
#include "Motor.h"
#include "Encoder.h"

typedef struct
{
	uint32_t uptimer[MAX_TIMERS];  // 32 bit = 49 days at 1msec...
} systime_t;

Timer gTimer;
systime_t m_Time;

void Timer::hwTick()
{
	ticks_hw++;
	if ( ticks_hw == (2000/TICK_PERIOD_us) ) {
		// 1000usec = 1msec
		ticks_hw = 0;
		ticks_2ms++;
		flg_2ms = 1;
		// General Purpose CountUp Timers, increment at 2msec
		for (int i=0; i < MAX_TIMERS; i++ ) {
			m_Time.uptimer[i] += 2;    // countUp timers, overflow after 49 days
		}

		if ( ticks_2ms == 5 ) {
			// 10 ms
			ticks_2ms=0;
			ticks_10ms++;
			flg_10ms = 1;

			// All other ms based timers will be on 10ms boundaries
			if ( ticks_10ms == 10 ) {
				// 100ms
				ticks_10ms=0;
				ticks_100ms++;
				flg_100ms = 1;
			}
			if ( ticks_100ms == 5 ) {
				// halfsec
				ticks_100ms = 0;
				ticks_500ms++;
				flg_500ms = 1;
			}
			if ( ticks_500ms == 2 ) {
				// onesec
				ticks_500ms = 0;
				flg_1sec = 1;
			}
		}
	}
}



//----------------------------------------------------------------------------
// Name: Timer0 ISR handler
// Desc: Main system Tick
//		 Timer0 setup for overflow at TICK_FREQ or TICK_PERIOD_us
//----------------------------------------------------------------------------
extern "C" void TIMER0_IRQHandler(void)
{

	TIMER_IntClear(TIMER0, TIMER_IF_OF);      // Clear overflow flag

	// PFA GPIO_SET(gpio_O4);
	gEncoders.RunISR();
	gMotors.RunISR();
	// PFA GPIO_CLR(gpio_O4);
	gTimer.hwTick();
}

/*----------------------------------------------------------------------------
Name: Time_StartTimer
Desc: Start a selected Timer
Ins	: timernum	must be leass than MAX_TIMERS
Outs:
/ ---------------------------------------------------------------------------*/
void Time_StartTimer( uint8_t timernum )
{
	if ( timernum < MAX_TIMERS ) {
		m_Time.uptimer[timernum]=0;
	}
}

/*----------------------------------------------------------------------------
Name: Time_isTimeout
Desc: Check for elapsed time.  Timer must have been started first with StartTimer
Ins	: timernum		must be leass than MAX_TIMERS
	  timeout_ms	period to compare against
Outs: TRUE when timer has run past the specified timeout
	  FALSE otherwise (or is an invalid timernum was specified
/ ---------------------------------------------------------------------------*/
bool Time_isTimeout( uint8_t timernum, uint32_t timeout_ms )
{
	if ( timernum < MAX_TIMERS ) {
		if ( m_Time.uptimer[timernum] > timeout_ms ) {
			return( true );
		}
	}
	return( false );
}

/*----------------------------------------------------------------------------
Name: Time_CheckTime
Desc: Return the time elapsed since Time_StartTimer
Ins	: timernum		must be leass than MAX_TIMERS
Outs: time in msec
/ ---------------------------------------------------------------------------*/
uint32_t Time_CheckTime(uint8_t timernum)
{
	if (timernum < MAX_TIMERS) {
		return( m_Time.uptimer[timernum]);
	} else {
		return 0;
	}
}
