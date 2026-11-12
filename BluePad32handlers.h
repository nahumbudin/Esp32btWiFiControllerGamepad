/**
*	@file		BluePad32handlers.h
*	@author		Nahum Budin
*	@date		6-Nov-2026
*	@version	1.0 - Initial version
*
*	@brief		BT Bluepad32 hndlers
*
*	History:\n
*
*/

#pragma once

// Uncomment to turn gamepad debug print on
#define GAMEPAD_DEBUG_PRINT_ON

// Uncomment to turn gamepad mouse pad debug print on
#define GAMEPAD_MOUSE_DEBUG_PRINT_ON

#include <Bluepad32.h>

#define UNI_GAMEPAD_MAPPINGS_DPAD     100
#define UNI_GAMEPAD_MAPPING_BUTTONS_L 101
#define UNI_GAMEPAD_MAPPING_BUTTONS_H 102

// Thread-safe Data Payload Structure
typedef struct {
    int command;
    int32_t param; // e.g., -100 to 100
    bool safety_stop;  // System override flag
} rx_command_t;

// Global container arrays for tracking the controller pointers
extern ControllerPtr ps_controllers[BP32_MAX_GAMEPADS];

// Global queue to pass commands and parameters to the HW control tasks.
extern QueueHandle_t rx_commands_queue;


// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl);

void onDisconnectedController(ControllerPtr ctl);

// Debug printing of all controller values
void dumpGamepad(ControllerPtr ctl);

// Debug print gamepad mouse values
void dumpMouse(ControllerPtr ctl);

// Process all gamepad switches and axis inputs and, as applicable, send as 
// commands to the commands queue
void processGamepad(ControllerPtr ctl);

void processMouse(ControllerPtr ctl);

void processControllers();

void pushReportToQueue(const uint8_t* data, bool forceReset = false);
uint8_t mapBp32DpadToHat(uint8_t dpad);
uint16_t mapBp32ButtonsToMask(ControllerPtr ctl);


