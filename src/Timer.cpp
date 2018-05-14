//============================================================================
//  Title	: Utils.c
//  Desc	: Misc utility functions
//  2016-11-03	Daraius		Created
//============================================================================

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
	if ( ticks_hw == (1000/TICK_PERIOD_us) ) {
		// 1000usec = 1msec
		ticks_hw = 0;
		ticks_1ms++;
		flg_1ms = 1;
		// General Purpose CountUp Timers, increment at 1msec
		for (int i=0; i < MAX_TIMERS; i++ ) {
			m_Time.uptimer[i]++;    // countUp timers, overflow after 49 days
		}

		if ( ticks_1ms==10 ) {
			// 10 ms
			ticks_1ms=0;
			ticks_10ms++;
			flg_10ms = 1;

			// All other ms based timers will be on 10ms boundaries
			if ( ticks_10ms==10 ) {
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
	gMotor.RunISR();
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
