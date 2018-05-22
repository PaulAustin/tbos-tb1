/*
 * Encoder.h
 *
 *  Created on: May 2, 2018
 *      Author: paulaustin
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "Value.h"

enum
{
	kENCODER_1=0,
	kENCODER_2=1,
	kENCODER_Count
};

// Bit mask for ENC_Reset register
//#define mask_ENC_Reset_ENC1 	(1<<0)	//
//define mask_ENC_Reset_ENC2 	(1<<1)	//

class Encoder {
public:
	// ValueRegisters
	ATrigger _reset;
	AValue 	_ppr;
	AValue 	_count;
	AValue 	_rpm;
	int		_lastCount;
	// Used by ISR
	int 	_countEdge;  // count on A edge
	bool 	_lastEdgeA;
};

class EncoderManager
{
public:
	void 	Init();
	void 	RunISR();
	void 	CalckRPM(int);
	void 	Run();
	Encoder	_encoders[2];

};

extern EncoderManager gEncoders;


#endif /* ENCODER_H_ */
