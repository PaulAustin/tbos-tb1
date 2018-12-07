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
#include "Gpio.h"


Value gChargeStatus;
Value gVersion;
Value gByteOrderTest;


int main(void)
{
	/* Chip errata */
	gRMap.Init();
	HW_Init();

//	Sys_Status_Init();
	gSound.Init();
	gEncoders.Init();
	gMotors.Init();
	gServos.Init();
	gGpio.Init();
	BQ_Init();

	GPIO_Write(SPK_EN, 1);		// Turn on op-amp for speaker
	// BQ_5Venable(1);			// ToDo:  later this will be controlled
	// BQ_WDenable(0);				// Disable

	gRMap.SetValueObj(kRM_SystemFMVers1, &gVersion);
	gRMap.SetValueObj(kRM_ByteOrderTest, &gByteOrderTest);
	gRMap.SetValueObj(kRM_SystemStatus, &gVersion);
	gByteOrderTest.Set(0x01020304);
	gVersion.Set(0x01000003);

	GPIO_Write(O5, 1);
	GPIO_Write(O4, 1);
	GPIO_Write(MOT2_R, 0);
	GPIO_Write(MOT1_R, 0);

	GPIO_Write(O4, 2);

	int bootNote = 2;
	int bootNotes[] = {0, 261, 329, 195, -1};
	int chargeStat = 0;
    int i= 0;

	/* Infinite loop */
	while (1)
	{
		// Move values from register bank at periodic steps.
		// this might be a bit faster than needed. 100Hz would be a minimum.
		if ( gTimer.is_2msec() ) {
			gSound.Run();
			gMotors.Run();
			gEncoders.Run();
			gServos.Run();
			gGpio.Run();
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

			if (bootNote >= 0) {
				int pitch = bootNotes[bootNote];
				if (pitch > 0) {
					gSound.PluckFrequency(pitch);
				}
				bootNote--;
			}
		}
	}
}
