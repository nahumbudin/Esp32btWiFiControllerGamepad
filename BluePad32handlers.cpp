/**
*	@file		BluePad32handlers.cpp
*	@author		Nahum Budin
*	@date		6-Nov-2026
*	@version	1.0 - Initial version
*
*	@brief		BT Bluepad32 hndlers
*
*	History:\n
*
*/

#include "BluePad32handlers.h"



// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    bool found_empty_slot = false;

    if (ctl->isGamepad()) {
        Serial.printf("Slot %d: Gamepad connected!\n", ctl->index());
    } else if (ctl->isMouse()) {
        Serial.printf("Slot %d: Touchpad Mouse connected!\n", ctl->index());
    }

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (ps_controllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, we can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            ps_controllers[i] = ctl;
            found_empty_slot = true;
            break;
        }
    }
    if (!found_empty_slot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool found_controller = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (ps_controllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            ps_controllers[i] = nullptr;
            found_controller = true;
            break;
        }
    }

    if (!found_controller) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

// Debug printing of all controller values
void dumpGamepad(ControllerPtr ctl) {
    
    #ifdef GAMEPAD_DEBUG_PRINT_ON
    printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
	#endif
}

// Debug print gamepad mouse values
void dumpMouse(ControllerPtr ctl) {
    
	#ifdef GAMEPAD_MOUSE_DEBUG_PRINT_ON
	Serial.printf("idx=%d, buttons: 0x%04x, scrollWheel=0x%04x, delta X: %4d, delta Y: %4d\n",
                   ctl->index(),        // Controller Index
                   ctl->buttons(),      // bitmask of pressed buttons
                   ctl->scrollWheel(),  // Scroll Wheel
                   ctl->deltaX(),       // (-511 - 512) left X Axis
                   ctl->deltaY()        // (-511 - 512) left Y axis
    );
	#endif
}

/*
// Process all gamepad switches and axis inputs and, as applicable, send as 
// commands to the commands queue
void processGamepad(ControllerPtr ctl) {

	static int32_t prev_brake = 0, prev_throttle = 0;
	static int32_t prev_axis_x = 0, prev_axis_rx = 0, prev_axis_y = 0, prev_axis_ry = 0;
	static int32_t prev_gyro_x = 0, prev_gyro_y = 0, prev_gyro_z = 0;
	static int32_t prev_accel_x = 0, prev_accel_y = 0, prev_accel_z = 0;
  
	TickType_t xTicksToWait = pdMS_TO_TICKS(100);
	BaseType_t xStatus = pdFAIL; // Initialize safely to prevent stack garbage bugs	
	bool new_command_sent = false;
	bt_command_t new_command;
  
	// Switches - Changed from else if to independent if blocks to allow concurrent button inputs
	if (ctl->a()) {
		// Button Cross pressed
		new_command.command = UNI_GAMEPAD_MAPPINGS_BUTTON_A;
		new_command.param = 0;
		new_command.safety_stop = false;
		new_command_sent = true;
	
		if (commands_queue != NULL) {
			xStatus = xQueueSend(commands_queue, &new_command, xTicksToWait);
		}
	}
  
	if (ctl->b()) {
		// Button Circle pressed
		new_command.command = UNI_GAMEPAD_MAPPINGS_BUTTON_B;
		new_command.param = 0;
		new_command.safety_stop = false;
		new_command_sent = true;
	
		if (commands_queue != NULL) {
			xStatus = xQueueSend(commands_queue, &new_command, xTicksToWait);
		}
	}
  
	if (ctl->x()) {
		// Button Square pressed - auto activates rotate
		new_command.command = UNI_GAMEPAD_MAPPINGS_BUTTON_X;
		new_command.param = 0;
		new_command.safety_stop = false;
		new_command_sent = true;
	
		if (commands_queue != NULL) {
			xStatus = xQueueSend(commands_queue, &new_command, xTicksToWait);
		}
	}
  
	if (ctl->y()) {
		// Button Triangle pressed
		new_command.command = UNI_GAMEPAD_MAPPINGS_BUTTON_Y;
		new_command.param = 0;
		new_command.safety_stop = false;
		new_command_sent = true;
	
		if (commands_queue != NULL) {
			xStatus = xQueueSend(commands_queue, &new_command, xTicksToWait);
		}
	}
  
	if (ctl->l1()) {
		// Button L1 Trigger pressed
	}
  
	if (ctl->r1()) {
		// Button R1 Trigger Pressed
	}
	
	// Brake and Throttle changes are evaluated independently 
	if (ctl->l2()) {
		int32_t brake = ctl->brake();
		if (prev_brake != brake) {
		prev_brake = brake;
		}
	}
	
	if (ctl->r2()) {
		int32_t throttle = ctl->throttle();
		if (prev_throttle != throttle) {
		prev_throttle = throttle;
		}
	}
	
	if (ctl->thumbL()) {
		// Joystick left sw pressed
	}
	
	if (ctl->thumbR()) {
		// Joystick right sw pressed
	}
	
	// Axis (Joysticks)
	int32_t axis_x = ctl->axisX();
	if (axis_x != prev_axis_x) {
		prev_axis_x = axis_x;
	}
	
	int32_t axis_y = ctl->axisY();
	if (axis_y != prev_axis_y) {
		prev_axis_y = axis_y;
	}
	
	int32_t axis_rx = ctl->axisRX();
	if (axis_rx != prev_axis_rx) {
		prev_axis_rx = axis_rx;
	}
	
	int32_t axis_ry = ctl->axisRY();
	if (axis_ry != prev_axis_ry) {
		prev_axis_ry = axis_ry;
	}
	
	// Gyro and Accelerometer
	int32_t gyro_x = ctl->gyroX();
	if (gyro_x != prev_gyro_x) {
		prev_gyro_x = gyro_x;
	}
	
	int32_t gyro_y = ctl->gyroY();
	if (gyro_y != prev_gyro_y) {
		prev_gyro_y = gyro_y;
	}
	
	int32_t gyro_z = ctl->gyroZ();
	if (gyro_z != prev_gyro_z) {
		prev_gyro_z = gyro_z;
	}
	
	int32_t accel_x = ctl->accelX();
	if (accel_x != prev_accel_x) {
		prev_accel_x = accel_x;
	}
	
	int32_t accel_y = ctl->accelY();
	if (accel_y != prev_accel_y) {
		prev_accel_y = accel_y;
	}
	
	int32_t accel_z = ctl->accelZ();
	if (accel_z != prev_accel_z) {
		prev_accel_z = accel_z;
	}
	
		if (new_command_sent) {
			if (xStatus == pdPASS) {
				// Data successfully sent!
			} else {
				printf("Command: %i Param: %i push to Q - Queue was full and timed out.\n", new_command.command, new_command.param);
			}
			new_command_sent = false;
		}
	
	dumpGamepad(ctl);
}
*/

void processMouse(ControllerPtr ctl) {
    static int32_t prev_delta_x = 0, prev_delta_y = 0;
    int32_t delta_x = ctl->deltaX();
    int32_t delta_y = ctl->deltaY();

    if (delta_x != prev_delta_x)
    {

      prev_delta_x = delta_x;
    }

    if (delta_y != prev_delta_y)
    {

      prev_delta_y = delta_y;
    }

    // See "dumpMouse" for possible things to query.
    dumpMouse(ctl);
}

// Shared delta-filter helper used by BOTH Wi-Fi TCP and Bluepad32 modes
void pushReportToQueue(const uint8_t* data, bool forceReset) {
    constexpr size_t REPORT_SIZE = 9;
    static uint8_t prev[REPORT_SIZE] = {128, 128, 128, 128, 0, 0, 8, 0, 0};
    static const uint8_t cmd_map[REPORT_SIZE] = {
        UNI_GAMEPAD_MAPPINGS_AXIS_X,
        UNI_GAMEPAD_MAPPINGS_AXIS_Y,
        UNI_GAMEPAD_MAPPINGS_AXIS_RX,
        UNI_GAMEPAD_MAPPINGS_AXIS_RY,
        UNI_GAMEPAD_MAPPINGS_BUTTON_TRIGGER_L,
        UNI_GAMEPAD_MAPPINGS_BUTTON_TRIGGER_R,
        UNI_GAMEPAD_MAPPINGS_DPAD,
        UNI_GAMEPAD_MAPPING_BUTTONS_L,
        UNI_GAMEPAD_MAPPING_BUTTONS_H
    };

    if (forceReset) {
        memset(prev, 0xFF, REPORT_SIZE);
        return;
    }

    rx_command_t cmd;
    cmd.safety_stop = false;
    for (size_t i = 0; i < REPORT_SIZE; i++) {
        if (data[i] != prev[i]) {
            prev[i] = data[i];
            cmd.command = cmd_map[i];
            cmd.param = data[i];
            xQueueSend(rx_commands_queue, &cmd, 0);
        }
    }
}

// Convert Bluepad32 D-Pad bitmask to Blugaemand 0..8 hat code
uint8_t mapBp32DpadToHat(uint8_t dpad) {
    bool up    = dpad & DPAD_UP;
    bool down  = dpad & DPAD_DOWN;
    bool left  = dpad & DPAD_LEFT;
    bool right = dpad & DPAD_RIGHT;

    if (up && right)   return 1; // NE
    if (down && right) return 3; // SE
    if (down && left)  return 5; // SW
    if (up && left)    return 7; // NW
    if (up)            return 0; // N
    if (right)         return 2; // E
    if (down)          return 4; // S
    if (left)          return 6; // W
    return 8;                    // Centered
}

// Convert Bluepad32 buttons into Blugaemand's 16-bit Linux HID button mask
uint16_t mapBp32ButtonsToMask(ControllerPtr ctl) {
    uint16_t mask = 0;
    if (ctl->a())           mask |= 0x0001; // SOUTH (A / Cross)
    if (ctl->b())           mask |= 0x0002; // EAST  (B / Circle)
    if (ctl->x())           mask |= 0x0008; // NORTH (X / Triangle)
    if (ctl->y())           mask |= 0x0010; // WEST  (Y / Square)
    if (ctl->l1())          mask |= 0x0040; // L1 / LB
    if (ctl->r1())          mask |= 0x0080; // R1 / RB
    if (ctl->l2())          mask |= 0x0100; // L2 Digital
    if (ctl->r2())          mask |= 0x0200; // R2 Digital
    if (ctl->miscSelect())  mask |= 0x0400; // Select / Share / Minus
    if (ctl->miscStart())   mask |= 0x0800; // Start / Options / Plus
    if (ctl->miscSystem())  mask |= 0x1000; // Home / Guide
    if (ctl->thumbL())      mask |= 0x2000; // L3
    if (ctl->thumbR())      mask |= 0x4000; // R3
    return mask;
}

void processGamepad(ControllerPtr ctl) {
    constexpr size_t REPORT_SIZE = 9;
		uint8_t report[REPORT_SIZE];

    // 1. Convert Bluepad32 sticks (-512..511, center 0) -> 0..255 (center 128)
    report[0] = (uint8_t)constrain((ctl->axisX()  + 512) >> 2, 0, 255);
    report[1] = (uint8_t)constrain((ctl->axisY()  + 512) >> 2, 0, 255);
    report[2] = (uint8_t)constrain((ctl->axisRX() + 512) >> 2, 0, 255);
    report[3] = (uint8_t)constrain((ctl->axisRY() + 512) >> 2, 0, 255);

    // 2. Convert Bluepad32 triggers (0..1023) -> 0..255
    report[4] = (uint8_t)constrain(ctl->brake()    >> 2, 0, 255);
    report[5] = (uint8_t)constrain(ctl->throttle() >> 2, 0, 255);

    // 3. D-Pad hat (0..8)
    report[6] = mapBp32DpadToHat(ctl->dpad());

    // 4. 16-bit button bitmask split into Low & High bytes
    uint16_t btnMask = mapBp32ButtonsToMask(ctl);
    report[7] = (uint8_t)(btnMask & 0xFF);
    report[8] = (uint8_t)((btnMask >> 8) & 0xFF);

    pushReportToQueue(report);
}

void processControllers() {
    for (auto ps_controller : ps_controllers) {
        if (ps_controller && ps_controller->isConnected() && ps_controller->hasData()) {
            if (ps_controller->isGamepad()) {
                processGamepad(ps_controller);
            } 
            else if (ps_controller->isMouse()) {
                processMouse(ps_controller);
            } 
            //else if (ps_controller->isKeyboard()) {
            //    processKeyboard(ps_controller);
            //} 
            //else if (ps_controller->isBalanceBoard()) {
            //    processBalanceBoard(ps_controller);
            //} 
            else {
                Serial.println("Unsupported controller");
            }
        }
    }
}







