/**
*	@file		HardwareControl.cpp
*	@author		Nahum Budin
*	@date		3-Nov-2026
*	@version	1.0 - Initial version
*
*	@brief		Hardware control object that enables controling hardware elements
*				such as including motors, sensors, etc.. (optimized to LEGO motors).
*				Specifically supports the Maker-ESP32-Pro board 
*				https://github.com/nulllaborg/maker-esp32-pro
*
*	History:\n
*
*/

//#include <driver/ledc.h>

#include <Arduino.h>

#include "HardwareControl.h"

HardwareControl *HardwareControl::hw_control_instance = NULL;

HardwareControl *HardwareControl::get_instance()
{
	if (hw_control_instance == NULL)
	{
		hw_control_instance = new HardwareControl();
	}
	
	return hw_control_instance;
}

HardwareControl::HardwareControl()
{
	dc_motors[0] = {MOTOR_0_PIN_1, MOTOR_0_PIN_2, SERVO_CH1, SERVO_CH2};
	dc_motors[1] = {MOTOR_1_PIN_1, MOTOR_1_PIN_2, SERVO_CH3, SERVO_CH4};
	dc_motors[2] = {MOTOR_2_PIN_1, MOTOR_2_PIN_2, SERVO_CH5, SERVO_CH6};
	dc_motors[3] = {MOTOR_3_PIN_1, MOTOR_3_PIN_2, SERVO_CH7, SERVO_CH8};
	
	ledcSetup(SERVO_CH1, LEGO_FREQ, PWM_RES); 
	ledcSetup(SERVO_CH2, LEGO_FREQ, PWM_RES); 
	ledcSetup(SERVO_CH3, LEGO_FREQ, PWM_RES); 
	ledcSetup(SERVO_CH4, LEGO_FREQ, PWM_RES); 
	ledcSetup(SERVO_CH5, LEGO_FREQ, PWM_RES); 
	ledcSetup(SERVO_CH6, LEGO_FREQ, PWM_RES); 
	ledcSetup(SERVO_CH7, LEGO_FREQ, PWM_RES); 
	ledcSetup(SERVO_CH8, LEGO_FREQ, PWM_RES); 
	
	// Attach the physical IO pins to our dedicated channel slots
	ledcAttachPin(dc_motors[0].pin1, SERVO_CH1);
	ledcAttachPin(dc_motors[0].pin2, SERVO_CH2);
	ledcAttachPin(dc_motors[1].pin1, SERVO_CH3);
	ledcAttachPin(dc_motors[1].pin2, SERVO_CH4);
	ledcAttachPin(dc_motors[2].pin1, SERVO_CH5);
	ledcAttachPin(dc_motors[2].pin2, SERVO_CH6);
	ledcAttachPin(dc_motors[3].pin1, SERVO_CH7);
	ledcAttachPin(dc_motors[3].pin2, SERVO_CH8);

  for (int m = 0; m < 4; m++)
  {
      this->Stop_DC_Motor(dc_motors[m]);
  }
	
	
}

HardwareControl::~HardwareControl ()
{
	
}

// Safe control function specifically mapped for discrete LEGO steps
void HardwareControl::Set_DC_Motor_Speed(motor_t motorID, int speed) {  
	if (speed == 0) {
		ledcWrite(motorID.ch1, 0);
		ledcWrite(motorID.ch2, 0);
	} else if (speed > 0) {
		ledcWrite(motorID.ch1, speed);
		ledcWrite(motorID.ch2, 0);
	} else {
		ledcWrite(motorID.ch1, 0);
		ledcWrite(motorID.ch2, -speed); // Uses the absolute value to drive direction
	}
}

void HardwareControl::Stop_DC_Motor(motor_t motorID)
{
	this->Set_DC_Motor_Speed(motorID, 0);
}

