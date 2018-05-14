
#include "Value.h"

class MotorManager {
	Value _power1;
	Value _power2;
	Value _break1;
	Value _break2;
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

