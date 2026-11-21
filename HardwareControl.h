/**
*	@file		HardwareControl.h
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

#pragma once

//#include <encoder_motor.h>
//#include "encoder_motor_lib.h"

//#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)

// CRITICAL LEGO CHANGE: Set frequency to 1150 Hz instead of 50 Hz
#define LEGO_FREQ     1150  //100 
#define PWM_RES		    8

#define MIN_FRONT_DISTANCE 20 // minimum distance - stop the car

// Specific Motors pins of the Maker-ESP32-Pro board
#define MOTOR_0_PIN_1	27
#define MOTOR_0_PIN_2	13
#define MOTOR_1_PIN_1	4
#define MOTOR_1_PIN_2	2
#define MOTOR_2_PIN_1	17
#define MOTOR_2_PIN_2	12
#define MOTOR_3_PIN_1	14
#define MOTOR_3_PIN_2	15

// Force internal PWM channel numbers to prevent version conflicts
#define SERVO_CH1  0
#define SERVO_CH2  1
#define SERVO_CH3  2
#define SERVO_CH4  3
#define SERVO_CH5  4
#define SERVO_CH6  5
#define SERVO_CH7  6
#define SERVO_CH8  7

// Sreering Hall Effect feedback sensor analog input
#define STEERING_HALL_SENS_ANALOG_IN_PIN	34

// DRAGON Wierless Controller for PS4 Controls Buttons Mapping
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_X					1
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_O					2
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_SQUARE		8
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_TRIANGLE	16
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_L1				64
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_LOW_R1				128

#define WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_TRIGGER_L	1
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_TRIGGER_R	2
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_SHARE			4
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_OPTION			8
#define WIRELESS_CONTROLLER_PS4_BUTTOMS_HIGH_HOME				16

// DRAGON Wierless Controller for PS4 Controls DPAD Mapping
#define WIRELESS_CONTROLLER_PS4_DPAD_UP								0
#define WIRELESS_CONTROLLER_PS4_DPAD_UP_RIGHT					1
#define WIRELESS_CONTROLLER_PS4_DPAD_RIGHT						2
#define WIRELESS_CONTROLLER_PS4_DPAD_RIGHT_DOWN				3
#define WIRELESS_CONTROLLER_PS4_DPAD_DOWN							4
#define WIRELESS_CONTROLLER_PS4_DPAD_LEFT_DOWN				5
#define WIRELESS_CONTROLLER_PS4_DPAD_LEFT							6
#define WIRELESS_CONTROLLER_PS4_DPAD_UP_LEFT					7
#define WIRELESS_CONTROLLER_PS4_DPAD_NONE							8

class HardwareControl {
	
	public:

		typedef enum {
		
		
		} hw_comands_type_t;
		
		struct motor_t
		{
			int pin1;
			int pin2;
			int ch1;
			int ch2;
		};
	
		~HardwareControl();
		
		static HardwareControl *get_instance();

		void Commands_Processor(int command, int param);
		
		// Safe control function specifically mapped for discrete LEGO steps
		void Set_DC_Motor_Speed(motor_t motorID, int speed);
		void Stop_DC_Motor(motor_t motorID);

		int Read_Analog_Input(uint8_t input_pin);

		int Read_Steering_Hall_Sensor_Input(float filter = 0.f);

		motor_t dc_motors[4];
		
	private:
	
		HardwareControl();

		int prev_x_axis_val = 128;
		int prev_y_axis_val = 128;
		int prev_rx_axis_val = 128;
		int prev_ry_axis_val = 128;
	
		int front_dist = MIN_FRONT_DISTANCE + 10;
	
		static HardwareControl* hw_control_instance;
};

//#endif // ESP_ARDUINO_VERSION
