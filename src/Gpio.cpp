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
#include "Gpio.h"
#include "SpiRegisterMap.h"

GpioManager gGpio;


void GpioManager::Init(void){

	//Set the id to the hardware id
	_Gpios[0]._id = O4;
	_Gpios[1]._id = O5;
	_Gpios[2]._id = IO6;
	_Gpios[3]._id = IO7;
	_Gpios[4]._id = IO8;
	_Gpios[5]._id = IO9;


	//Set the modes to Inputs

	for(int i = 2; i < GPIO_NUM; i++)
	{
		HW_GPIO_SetMode(_Gpios[i]._id, 1);
	}

	//set values to the register map
	gRMap.SetValueObj(kRM_GpioReadPins, &gpioOutput);
	gRMap.SetValueObj(kRM_GpioWritePins, &gpioInput);
	gRMap.SetValueObj(kRM_GpioMode, &gpioMode);
	gRMap.SetValueObj(kRM_GpioPullup, &gpioPullup);
}



void GpioManager::Run()
{
	//Update the individual bits of info
	//GPIO_Write(O4, 1);
	//Set the states of each pin
	if(gpioInput.HasAsyncSet())
	{
		int byte = gpioInput.Get();
		for(int i = 0; i < GPIO_NUM; i++)
		{
			int inputBit = (byte & 1);
			int gpioBit = _Gpios[i]._state;
			if(gpioBit == inputBit){
				//They were the same
				//Do nothing
			}
			else {
				if (inputBit == 1)
				{
					_Gpios[i]._state = 1;
				}
				else {
					_Gpios[i]._state = 0;
				}
			}

			byte = byte >> 1;
		}
	}

	//Check which GPIO is supposed to be pulled up
	if(gpioPullup.HasAsyncSet())
	{
		int byte = gpioPullup.Get();
		for(int i = 0; i < GPIO_NUM; i++)
		{
			int pullupBit = (byte & 1);
			int gpioBit = _Gpios[i]._pullup;
			if(gpioBit == pullupBit){
				//They were the same
				//Do nothing
			}
			else {
				if (pullupBit == 1)
				{
					_Gpios[i]._pullup = 1;
					HW_GPIO_SetMode(_Gpios[i]._id, 2);
				}
				else {
					_Gpios[i]._pullup = 0;
				}
			}

			byte = byte >> 1;
		}
	}
	//Whether the pin is an input or an output
	if(gpioMode.HasAsyncSet())
	{
		int byte = gpioMode.Get();
		for(int i = 0; i < GPIO_NUM; i++)
		{
			int modeBit = (byte & 1);
			int gpioBit = _Gpios[i]._mode;
			if(gpioBit == modeBit){
				//They were the same
				//Do nothing
			}
			else {
				if (modeBit == 1)
				{
					//OUTPUT
					_Gpios[i]._mode = 1;
					HW_GPIO_SetMode(_Gpios[i]._id, 0);
				}
				else {
					//INPUT
					_Gpios[i]._mode = 0;
					HW_GPIO_SetMode(_Gpios[i]._id, 1);
				}
			}

			byte = byte >> 1;
		}
	}

	//Update the _states of all of the pins
	for(int e = 0; e < GPIO_NUM; e++)
	{
		//Input pin
		if (_Gpios[e]._mode == 0)
		{
				//Put a not here??
				_Gpios[e]._output = GPIO_Read(_Gpios[e]._id);
		}
		//Output pin
		else
		{
			GPIO_Write(_Gpios[e]._id, _Gpios[e]._state);
			_Gpios[e]._output = _Gpios[e]._state;
		}
	}
	//Set the Output Register to the combined

	int output = 0;
	for(int i = 5; i >= 0; i--)
	{
		output = (output << 1);
		output = (output | _Gpios[i]._output);

	}
	gpioOutput.Set(output);
	//GPIO_Write(O4, 0);
}





