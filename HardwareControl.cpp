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

#include <Bluepad32.h>

#include "HardwareControl.h"
#include "BluePad32handlers.h"

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

// Uncomment to print debug info
#define COMMANDS_PROC_DEBG_PRT_ON

void HardwareControl::Commands_Processor(int command, int param)
{
	if (command == cmd_map[0])
	{
		// AXIS_X
		#ifdef COMMANDS_PROC_DEBG_PRT_ON
		printf("Axis X %i\n", param);
		#endif
	}

	// Use ifs as more than one button may be pressed
	if (command == cmd_map[1])
	{
		// AXIS_Y
		#ifdef COMMANDS_PROC_DEBG_PRT_ON
		printf("Axis Y %i\n", param);
		#endif
	}

	if (command == cmd_map[2])
	{
		// AXIS_RX
		#ifdef COMMANDS_PROC_DEBG_PRT_ON
		printf("Axis RX %i\n", param);
		#endif
	}

	if (command == cmd_map[3])
	{
		// AXIS_RY
		#ifdef COMMANDS_PROC_DEBG_PRT_ON
		printf("Axis RY %i\n", param);
		#endif
	}

	if (command == cmd_map[4])
	{
		// Trigger L
		#ifdef COMMANDS_PROC_DEBG_PRT_ON
		printf("Trigger L %i\n", param);
		#endif
	}

	if (command == cmd_map[5])
	{
		// Trigger R
		#ifdef COMMANDS_PROC_DEBG_PRT_ON
		printf("Trigger R %i\n", param);
		#endif
	}

	if (command == cmd_map[6])
	{
		// DPAD
		#ifdef COMMANDS_PROC_DEBG_PRT_ON
		printf("DPAD %i\n", param);
		#endif

		switch (param) { // Use switch as only one state may be valid.
			case WIRELESS_CONTROLLER_PS4_DPAD_UP: // 0
				// ACTION PLACEHOLDER: D-Pad Up
				break;

			case WIRELESS_CONTROLLER_PS4_DPAD_UP_RIGHT: // 1
				// ACTION PLACEHOLDER: D-Pad Up-Right
				break;

			case WIRELESS_CONTROLLER_PS4_DPAD_RIGHT: // 2
				// ACTION PLACEHOLDER: D-Pad Right
				break;

			case WIRELESS_CONTROLLER_PS4_DPAD_RIGHT_DOWN: // 3
				// ACTION PLACEHOLDER: D-Pad Down-Right
				break;

			case WIRELESS_CONTROLLER_PS4_DPAD_DOWN: // 4
				// ACTION PLACEHOLDER: D-Pad Down
				break;

			case WIRELESS_CONTROLLER_PS4_DPAD_LEFT_DOWN: // 5
				// ACTION PLACEHOLDER: D-Pad Down-Left
				break;

			case WIRELESS_CONTROLLER_PS4_DPAD_LEFT: // 6
				// ACTION PLACEHOLDER: D-Pad Left
				break;

			case WIRELESS_CONTROLLER_PS4_DPAD_UP_LEFT: // 7
				// ACTION PLACEHOLDER: D-Pad Up-Left
				break;

			case WIRELESS_CONTROLLER_PS4_DPAD_NONE: // 8
			default:
				// ACTION PLACEHOLDER: D-Pad Released / Centered
				break;
		}
	}

	if (command == cmd_map[7])
	{
		// Buttons Low
		#ifdef COMMANDS_PROC_DEBG_PRT_ON
		printf("Buttons Low %i\n", param);
		#endif

		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_X) 
		{ // 0x0001
      // ACTION PLACEHOLDER: Button X pressed
    }

    if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_O) { // 0x0002
        // ACTION PLACEHOLDER: Button O pressed
    }
        
		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_SQUARE) { // 0x0008
      // ACTION PLACEHOLDER: Button Square pressed
    }
        
		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_TRIANGLE) { // 0x0010
      // ACTION PLACEHOLDER: Button Triangle pressed
    }
        
		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_L1) { // 0x0040
      // ACTION PLACEHOLDER: L1 / LB pressed
    }
        
		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_R1) { // 0x0080
      // ACTION PLACEHOLDER: R1 / RB pressed
    }
	}

	if (command == cmd_map[8])
	{
		// Buttons High
		#ifdef COMMANDS_PROC_DEBG_PRT_ON
		printf("Buttons High %i\n", param);
		#endif

		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_TRIGGER_L) { // 0x0001
      // ACTION PLACEHOLDER: Trigger L pressed
    }

		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_TRIGGER_R) { // 0x002
      // ACTION PLACEHOLDER: Trigger R pressed
    }

		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_SHARE) { // 0x0004
      // ACTION PLACEHOLDER: Share pressed
    }

		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_OPTION) { // 0x0008
      // ACTION PLACEHOLDER: Options pressed
    }

		if (param & WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_HOME) { // 0x0010
      // ACTION PLACEHOLDER: Home pressed
    }
	}
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

