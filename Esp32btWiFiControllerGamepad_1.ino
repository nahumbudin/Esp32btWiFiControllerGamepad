/**
*	@file		Esp32btWiFiControllerGamepad.ino
*	@author		Nahum Budin
*	@date		1-Nov-2026
*	@version	1.0 - Initial version
*
*	@brief	This sketch implements a BT interface to a Sony PS4 Gamepad, OR
*           an TCP WiFi interface an Android PS4 Gamepad emulation application 
*           (modified Bluegeamand https://github.com/pablopenna/blugaemand)
*
*   Selection between BT or WiFi mode is done at sketch setup time by reading
*   an external switch connected to an IO pin.
*
*   The sketch specifically supports the Maker-ESP32-Pro board 
*   https://github.com/nulllaborg/maker-esp32-pro
*
*   This board enables controling hardware elements such as motors, sensors, etc.. 
*    (optimized to LEGO motors).
*
*   Arduino Board: esp32_bluepad32 ESP32 Dev Module
*
*	History:\n
*
*/

#include <Arduino.h>
#include <Preferences.h>
#include "DFRobot_URM09.h"

#include <Bluepad32.h>
#include "BluePad32handlers.h"

#include <WiFi.h>
#include <WiFiUdp.h>
//#include <AsyncUDP.h>
#include <esp_wifi.h>

#include <Wire.h>
#include <ESP32Servo.h>

#include "HardwareControl.h"

// Uncomment to test motors
//#define TEST_MOTORS_EN

#define MODE_SELECT_PIN 5   // Connect to GND for WiFi mode, leave open (HIGH) for BT mode
                            // for Wi-Fi mode
bool isWifiMode = false;

void control_task(void *pvParameters);


// Unique GATT Profile UUID Layout Contract Constants
#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_TX "d2a3ca31-152c-474c-b5f6-c51e9e30a501"

#define UNI_GAMEPAD_MAPPINGS_DPAD     100
#define UNI_GAMEPAD_MAPPING_BUTTONS_L 101
#define UNI_GAMEPAD_MAPPING_BUTTONS_H 102

int16_t dist = 2; // Distance in cm
int16_t prev_dist = 2;     
float temp = 20.0f;    // Temperature in °C
float prev_temp = 20.0f;
int us_dist_meas_cycle_count = 0;

volatile bool deviceConnected = false;
bool oldDeviceConnected = false;

// FreeRTOS Synchronization Mutex Primitive
//SemaphoreHandle_t xStateMutex = NULL;

HardwareControl *hw_control;

DFRobot_URM09 URM09; // US Distance sensor

// Global queue to pass rx commands and parameters to the HW control tasks.
QueueHandle_t rx_commands_queue = NULL;

// Declare the Wi-Fi credentials and UDP parameters
const char* ssid = "ESP32_Gamepad_Hotspot"; // Name your network
const char* password = "gamepad12345";       // Password must be at least 8 characters
const int tcp_port = 9999;                    // The port the app will send data to

IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

//AsyncUDP  udp;
WiFiServer tcpServer(9999);
WiFiClient tcpClient;

// Global container arrays for tracking the controller pointers
ControllerPtr ps_controllers[BP32_MAX_GAMEPADS];

// ---- Helper Function ---------
float limit(float v, float mn, float mx) { return (v < mn) ? mn : ((v > mx) ? mx : v); }

void control_task(void *pvParameters) {
    rx_command_t new_command;
    const TickType_t xInterval = pdMS_TO_TICKS(50); // 50 milliseconds

    while (1) {
        // Block for a MAXIMUM of 50ms waiting for data
        if (xQueueReceive(rx_commands_queue, &new_command, xInterval) == pdPASS) {
            // Data arrived before the 50ms window closed!
            do {            
                //printf("New Command Received: %i Param: %i.\n", new_command.command, new_command.param);
                hw_control->Commands_Processor(new_command.command, new_command.param);
            } while (xQueueReceive(rx_commands_queue, &new_command, 0) == pdPASS); // drain all

        } else {
            // 50ms passed and NO data arrived. 
            // The queue check safely timed out.
        }

        // --- RUN YOUR PERIODIC CODE HERE ---
        // This block will execute roughly every 50ms (or sooner if commands arrive)
        // Example: Update motor PWM calculations, check battery voltage
    }
}

volatile uint32_t mssg_num = 0; // dbg

void setup() {
  Serial.begin(115200);
  delay(500);

  // Initialize I2C on default ESP32 pins (SDA = 21, SCL = 22)
  Wire.begin(21, 22);
  Wire.setClock(400000);

  // Initialize US distance sensor at default I2C address (0x11)
  while (!URM09.begin(0x11)) {
    Serial.println("SEN0304 not found! Check wiring and I2C pins.");
    delay(1000);
  }

  // Configure mode and maximum range:
  // Modes:  MEASURE_MODE_PASSIVE or MEASURE_MODE_AUTOMATIC
  // Ranges: MEASURE_RANG_150, MEASURE_RANG_300, or MEASURE_RANG_500
  URM09.setModeRange(MEASURE_MODE_PASSIVE, MEASURE_RANG_150);
  delay(100);

    // Mode select input pin
  pinMode(MODE_SELECT_PIN, INPUT_PULLUP);
  delay(10);
  isWifiMode = (digitalRead(MODE_SELECT_PIN) == LOW);

  rx_commands_queue = xQueueCreate(128, sizeof(rx_command_t));
  if (rx_commands_queue == NULL) {
        Serial.println("Fatal: Failed to allocate memory for commands_queue!");
        while(1); // Stop execution
  }

  if (isWifiMode)
  {
    // Mode 1: WiFi TCP (Blugaemand Android App)
    Serial.println("Booting in WI-FI TCP Mode...");

    // Shut down BTstack completely so Wi-Fi gets 100% of the 2.4 GHz radio
    btStop(); 

    // Initialize the ESP32 Wi-Fi Hotspot (Access Point Mode)
    Serial.print("Setting up Wi-Fi Access Point...");
    WiFi.persistent(false);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);
    WiFi.setSleep(false);
    WiFi.softAPConfig(local_IP, gateway, subnet); // ????

    if (WiFi.softAP(ssid, password, 1, 0, 4)) { // Channel 1
        delay(100);
        esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
        tcpServer.begin(tcp_port);
        tcpServer.setNoDelay(true);
        Serial.printf("AP + TCP Ready on %s:%d\n", WiFi.softAPIP().toString().c_str(), tcp_port);
    } else {
        Serial.println(" AP Generation Failed!");
    }
  }
  else
  {
    // MODE 2: BLUETOOTH (Bluepad32 Physical Controller)
    Serial.println("Booting in BLUETOOTH (Bluepad32) Mode...");
    WiFi.disconnect(true);
    // Shut down Wi-Fi completely so Bluetooth gets 100% of the 2.4 GHz radio
    WiFi.mode(WIFI_OFF);

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", 
                    addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.enableVirtualDevice(true);
  }

  delay(500);
  Serial.println("Starting Esp32btController...");

  xTaskCreatePinnedToCore(
        control_task,     // Pointer to the function implementing the task
        "Control Task",   // A descriptive name for debugging/tracking
        4096,             // Stack size allocated to the task (in words, 4KB is safe)
        NULL,             // Parameter to pass to the task (we don't need one, so NULL)
        1,                // Task Priority (1 is standard, higher numbers have higher priority)
        NULL,             // Task Handle pointer (NULL means we don't need to track its reference)
        1                 // Pin to core 1
    );

  hw_control = HardwareControl::get_instance();
}


void loop() {
    constexpr size_t REPORT_SIZE = 9;
    static uint8_t prev[REPORT_SIZE] = {128, 128, 128, 128, 0, 0, 8, 0, 0};

#ifdef TEST_MOTORS_EN
    static int loop_count = 0;
    static int iteration_count = 0;
    static int direction = 1;
#endif

    static const uint8_t cmd_map[REPORT_SIZE] = 
    {
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

    static bool wasConnected = false;



    if (isWifiMode)
    {
        if (!tcpClient || !tcpClient.connected()) 
        {
            if (wasConnected) {
                wasConnected = false;
                Serial.println("TCP Client Disconnected!");
            }

            WiFiClient incoming = tcpServer.available();
            if (incoming) 
            {
                tcpClient = incoming;
                tcpClient.setNoDelay(true);
                memset(prev, 0xFF, REPORT_SIZE);
                wasConnected = true;
                Serial.println("TCP Client Connected!");
            }
        }

        if (tcpClient && tcpClient.connected()) 
        {
            // WiFi
            while (tcpClient.available() >= 10) 
            {
                if (tcpClient.read() != 0xAA) continue;

                uint8_t data[REPORT_SIZE];
                for (size_t i = 0; i < REPORT_SIZE; i++) 
                {
                    data[i] = (uint8_t)tcpClient.read();
                }

                pushReportToQueue(data);

                /*
                bt_command_t cmd; // TODO: Change name
                cmd.safety_stop = false;
                for (size_t i = 0; i < REPORT_SIZE; i++) 
                {
                    if (data[i] != prev[i]) 
                    {
                        prev[i] = data[i];
                        cmd.command = cmd_map[i];
                        cmd.param = data[i];
                        xQueueSend(commands_queue, &cmd, 0);
                    }
                }
                */
            }
        }
    }
    else
    {
        // BT
        bool dataUpdated = BP32.update();
        if (dataUpdated)
            processControllers();
    } 

#ifdef TEST_MOTORS_EN
    if (loop_count == 0)
    {
        hw_control->Set_DC_Motor_Speed(hw_control->dc_motors[0], direction * 200);
        hw_control->Set_DC_Motor_Speed(hw_control->dc_motors[1], direction * 200);
        hw_control->Set_DC_Motor_Speed(hw_control->dc_motors[2], direction * 200);
        hw_control->Set_DC_Motor_Speed(hw_control->dc_motors[3], direction * 200);
        direction *= -1;
    }

    loop_count++;
    if (loop_count >= 400)
    {
        loop_count = 0;
        iteration_count++;
        printf("Iteration: %i  Direction: %i\n", iteration_count, direction);
    }


    if (iteration_count >= 5)
    {
        direction = 0;
    }
#endif

    // Test Hall sensor inputdebug
    int hall_read = hw_control->Read_Steering_Hall_Sensor_Input(0.1);
    //printf("Steering Hall Sensor Input: %i\n", hall_read);

    // Trigger a measurement (required only in MEASURE_MODE_PASSIVE)
  us_dist_meas_cycle_count++;
  if (us_dist_meas_cycle_count >= 10)
  {
    // 10 * 25 = 0.25sec
    URM09.measurement();
    us_dist_meas_cycle_count = 0;
  }

    delay(25);

    if (us_dist_meas_cycle_count == 0)
    {
        dist = URM09.getDistance();     // Distance in cm
        temp = URM09.getTemperature();    // Temperature in °C
    }

    if ((dist != prev_dist) || (temp != prev_temp))
    {
        //Serial.print("Distance: ");
        //Serial.print(dist);
        //Serial.print(" cm | Temp: ");
        //Serial.print(temp, 1);
        //Serial.println(" °C");

        rx_command_t cmd;
        cmd.safety_stop = false;
        cmd.command = US_SENSOR_DISTANCE;
        cmd.param = dist;
        xQueueSend(rx_commands_queue, &cmd, 0);

        cmd.safety_stop = false;
        cmd.command = US_SENSOR_TEMP;
        cmd.param = (int)temp;
        xQueueSend(rx_commands_queue, &cmd, 0);
    
        prev_dist = dist;
        prev_temp = temp;
    }
}






