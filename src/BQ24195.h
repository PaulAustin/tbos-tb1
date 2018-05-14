#ifndef BQ24195_H_
#define BQ24195_H_


// <BQ24195.c>
enum {
	kBQ_IinLimit_Mask   = (0x07),	// REG00: bits[2:0]=IINLIM[2:0]
	kBQ_IinLimit_100mA  = (0x00),   // 000 100mA
	kBQ_IinLimit_150mA  = (0x01),	// 001 150mA
	kBQ_IinLimit_500mA  = (0x02),	// 010 500mA
	kBQ_IinLimit_900mA	= (0x03), 	// 011 900mA
	kBQ_IinLimit_1200mA = (0x04), 	// 100 1200mA
	kBQ_IinLimit_1500mA = (0x05),   // 101 1500mA
	kBQ_IinLimit_2000mA = (0x06),   // 110 2000mA
	kBQ_IinLimit_3000mA = (0x07),   // 110 3000mA
};

void BQ_Init(void);
void BQ_5VUsagePing();
void BQ_5VCheckTimeout();
void BQ_5VEnable(bool enable);
void BQ_WatchdogEnable(bool enable);
void BQ_IinLim(uint16_t IinLimit);
uint8_t BQ_ChargeStatus();

#endif
