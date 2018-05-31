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
#include "Timer.h"
#include "BQ24195.h"
#include "SpiRegisterMap.h"
#include "Encoder.h"
#include "Sound.h"
#include "Motor.h"
#include "Servo.h"

AValue gChargeStatus;
AValue gVersion;


int main(void)
{
	gRMap.Init();
	HW_Init();

	gSound.Init();
	gEncoders.Init();
	gMotors.Init();
	gServos.Init();
	BQ_Init();

	// Is this an option to allow vi register?
	// BQ_WDenable(0);				// Disable

	gRMap.SetValueObj(kRM_SystemFMVers1, &gVersion);
	gRMap.SetValueObj(kRM_SystemStatus, &gVersion);
	gVersion.Set(0x01000002);

	int bootNote = 3;
	int bootNotes[] = {0, 261, 329, 195, -1};
	int chargeStat = 0;
    int i= 0;

	/* Infinite loop */
	while (1)
	{
		// Move values from register bank at periodic steps.
		// this might be a bit faster than needed. 100Hz would be a minimum.
		if ( gTimer.is_1msec() ) {
			gSound.Run();
			gMotors.Run();
			gEncoders.Run();
			gServos.Run();
		}

		if (gTimer.is_100msec()) {

			if (bootNote >= 0) {
				int pitch = bootNotes[bootNote];
				if (pitch > 0) {
					HW_Timer1_SetFreq(pitch);
					HW_Timer1_Enable(true);  // Start Playing
				} else if (pitch == 0){
					HW_Timer1_Enable(false);  // Stop Playing
				}
				bootNote--;
			}
		}

		if ( gTimer.is_500msec() ) {
			// Periodically see if motors have been idle for a while. If so
			// The 5V enable will be dropped.
			BQ_5VUsagePing();

			if (gMotors.Idle()) {
				BQ_5VCheckTimeout();
			}

			//chargeStat = BQ_ChargeStatus();
			chargeStat = GPIO_Read(CHG_STAT);
			gChargeStatus.Set(chargeStat);

			i++;
			if (i % 20 == 0 ) {
				BQ_IinLim(kBQ_IinLimit_500mA);
			}

		}
	}
}
