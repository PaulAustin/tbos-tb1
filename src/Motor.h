
#include "Value.h"

class MotorManager {
	AValue _power1;
	AValue _power2;
	AValue _break1;
	AValue _break2;
public:
	void Init();
	void Run();
	bool Idle();
	void SetPower(int motor, int power);
	void RunISR();
};

extern MotorManager gMotor;

enum
{
	kMOTOR_1=0,
	kMOTOR_2=1,
	kMOTOR_Count
};


void Motor_Init(void);
bool Motors_Idle();
void Motor_Run(void);
void Motor_RunISR(void);
void Motor_SetPower(int id, int power);

