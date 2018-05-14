
#include "Hardware.h"
#include "Timer.h"
#include "BQ24195.h"
#include "SpiRegisterMap.h"
#include "Encoder.h"
#include "Sound.h"
#include "Motor.h"


Value gChargeStatus;
Value gVersion;


int main(void)
{

	gRMap.Init();
	HW_Init();

	gSound.Init();
	gEncoders.Init();
	gMotor.Init();
//	Servo_Init();
	BQ_Init();

	GPIO_Write(SPK_EN, 1);		// Turn on op-amp for speaker
	// BQ_5Venable(1);			// ToDo:  later this will be controlled
	// BQ_WDenable(0);				// Disable

	GPIO_Write(O5, 1);
	GPIO_Write(O4, 1);
	GPIO_Write(MOT2_R, 0);
	GPIO_Write(MOT1_R, 0);
	GPIO_Write(O4, 2);

	gRMap.SetValueObj(kRM_SystemFMVers1, &gVersion);
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
			gMotor.Run();
			gEncoders.Run();
		}

		if (gTimer.is_100msec()) {
			//chargeStat = BQ_ChargeStatus();
			chargeStat = GPIO_Read(CHG_STAT);
			gChargeStatus.Set(chargeStat);

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

			if (gMotor.Idle()) {
				BQ_5VCheckTimeout();
			}

			i++;
			if (i % 20 == 0 ) {
				BQ_IinLim(kBQ_IinLimit_500mA);
			}

		}
	}
}
