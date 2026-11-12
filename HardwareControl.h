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



//#if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)

// CRITICAL LEGO CHANGE: Set frequency to 1150 Hz instead of 50 Hz
#define LEGO_FREQ     100  //1150 
#define PWM_RES		    8

// Specific Motors pins of the Maker-ESP32-Pro board
#define MOTOR_0_PIN_1	27
#define MOTOR_0_PIN_2	13
#define MOTOR_1_PIN_1	5
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
		
		// Safe control function specifically mapped for discrete LEGO steps
		void Set_DC_Motor_Speed(motor_t motorID, int speed);
		void Stop_DC_Motor(motor_t motorID);

		motor_t dc_motors[4];
		
	private:
	
		HardwareControl();
	
		
	
		static HardwareControl* hw_control_instance;
};

//#endif // ESP_ARDUINO_VERSION
