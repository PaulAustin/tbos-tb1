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
	bool flg_1ms;
	bool flg_10ms;
	bool flg_100ms;
	bool flg_500ms;
	bool flg_1sec;
	int ticks_hw;
	int ticks_1ms;
	int ticks_10ms;
	int ticks_100ms;
	int ticks_500ms;
public:
	bool is_1msec() 	{ bool f = flg_1ms; flg_1ms = false; return f; };
	bool is_10msec() 	{ bool f = flg_10ms; flg_10ms = false; return f; };
	bool is_100msec() 	{ bool f = flg_100ms; flg_100ms = false; return f; };
	bool is_500msec() 	{ bool f = flg_500ms; flg_500ms = false; return f; };
	bool is_1sec()		{ bool f = flg_1sec; flg_1sec = false; return f; };
	void hwTick();
};
extern Timer gTimer;

// -- Time Section --
void Time_StartTimer( uint8_t timernum );
bool Time_isTimeout( uint8_t timernum, uint32_t timeout_ms );
uint32_t Time_CheckTime(uint8_t timernum);
