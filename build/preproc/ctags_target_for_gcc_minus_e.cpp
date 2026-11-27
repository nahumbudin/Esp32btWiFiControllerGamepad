# 1 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino"
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

# 27 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2
# 28 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2
# 29 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2

# 31 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2
# 32 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2

# 34 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2
# 35 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2
//#include <AsyncUDP.h>
# 37 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2

# 39 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2
# 40 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2

# 42 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2
# 43 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2

# 45 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 2

// Uncomment to test motors
//#define TEST_MOTORS_EN

// Uncomment to enable the use of the RGB LED
//#define RGB_LED_ENABLE

// Uncoment to enable the SparkFun Qwiic Buzzer
//#define BUZZER_ENABLE



bool isWifiMode = false;

void control_task(void *pvParameters);

// Unique BT GATT Profile UUID Layout Contract Constants




// US Distance Sensor
int16_t dist = 2; // Distance in cm
int16_t prev_dist = 2;
float temp = 20.0f; // Temperature in °C
float prev_temp = 20.0f;
int us_dist_meas_cycle_count = 0;

// BT Connection
volatile bool deviceConnected = false;
bool oldDeviceConnected = false;

// FreeRTOS Synchronization Mutex Primitive
//SemaphoreHandle_t xStateMutex = NULL;

HardwareControl *hw_control;

// RGB LED I2C controller (allocates only Mutex)




// US Distance sensor
DFRobot_URM09 URM09;

// Global queue to pass rx commands and parameters to the HW control tasks.
QueueHandle_t rx_commands_queue = 
# 91 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 3 4
                                 __null
# 91 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino"
                                     ;

// Declare the Wi-Fi credentials and UDP parameters
const char* ssid = "ESP32_Gamepad_Hotspot"; // Name your network
const char* password = "gamepad12345"; // Password must be at least 8 characters
const int tcp_port = 9999; // The port the app will send data to

IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

//AsyncUDP  udp;
WiFiServer tcpServer(9999);
WiFiClient tcpClient;

// Global container arrays for tracking the PS controller pointers
ControllerPtr ps_controllers[
# 107 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 3 4
                            4
# 107 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino"
                                             ];

// ---- Helper Function ---------
float limit(float v, float mn, float mx) { return (v < mn) ? mn : ((v > mx) ? mx : v); }

void control_task(void *pvParameters) {
    rx_command_t new_command;
    const TickType_t xInterval = ( ( TickType_t ) ( ( ( TickType_t ) ( 10 ) * ( TickType_t ) ( 
# 114 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 3 4
                                1000 
# 114 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino"
                                ) ) / ( TickType_t ) 1000U ) ); // 10 milliseconds Must be 1/HardwareControl.dt

    //TickType_t xLastWakeTime = xTaskGetTickCount();

    // Initialize steering hall effect sensor filter with starting position
    int filtered_adc = hw_control->Read_Steering_Hall_Sensor_Input(hw_control->FILTER_ALPHA);

    while (1) {
        // Wait precisely until the next 10ms cycle
        //vTaskDelayUntil(&xLastWakeTime, xInterval);
        // Do not block waiting for data
        if (xQueueReceive(rx_commands_queue, &new_command, xInterval) == ( ( ( BaseType_t ) 1 ) )) {
            // Data arrived before the 50ms window closed!
            do {
                //printf("New Command Received: %i Param: %i.\n", new_command.command, new_command.param);
                hw_control->Commands_Processor(new_command.command, new_command.param);
            } while (xQueueReceive(rx_commands_queue, &new_command, 0) == ( ( ( BaseType_t ) 1 ) )); // drain all

        } else {
            // 10ms passed and NO data arrived. 
            // The queue check safely timed out.
        }

        // Update the steering position feedback.
        filtered_adc = hw_control->Read_Steering_Hall_Sensor_Input(hw_control->FILTER_ALPHA);
        hw_control->Commands_Processor(105, filtered_adc);
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

  // Initialize I2C RGB LED Controller
# 170 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino"
// SparkFun Qwiic Buzzer object




  // Configure mode and maximum range:
  // Modes:  MEASURE_MODE_PASSIVE or MEASURE_MODE_AUTOMATIC
  // Ranges: MEASURE_RANG_150, MEASURE_RANG_300, or MEASURE_RANG_500
  URM09.setModeRange(0x00 /*|< passive mode*/, 0x00 /*|< Ranging from 150 */);
  delay(100);

    // Mode select input pin
  pinMode(5 /* Connect to GND for WiFi mode, leave open (HIGH) for BT mode*/, 0x05);
  delay(10);
  isWifiMode = (digitalRead(5 /* Connect to GND for WiFi mode, leave open (HIGH) for BT mode*/) == 0x0);

  rx_commands_queue = xQueueGenericCreate( ( 128 ), ( sizeof(rx_command_t) ), ( ( ( uint8_t ) 0U ) ) );
  if (rx_commands_queue == 
# 187 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 3 4
                          __null
# 187 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino"
                              ) {
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
    WiFi.mode(WIFI_MODE_AP);
    WiFi.setSleep(false);
    WiFi.softAPConfig(local_IP, gateway, subnet); // ????

    if (WiFi.softAP(ssid, password, 1, 0, 4)) { // Channel 1
        delay(100);
        esp_wifi_set_protocol(WIFI_IF_AP, 1 | 2);
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
    WiFi.mode(WIFI_MODE_NULL);

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
        control_task, // Pointer to the function implementing the task
        "Control Task", // A descriptive name for debugging/tracking
        4096, // Stack size allocated to the task (in words, 4KB is safe)
        
# 244 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 3 4
       __null
# 244 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino"
           , // Parameter to pass to the task (we don't need one, so NULL)
        1, // Task Priority (1 is standard, higher numbers have higher priority)
        
# 246 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino" 3 4
       __null
# 246 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino"
           , // Task Handle pointer (NULL means we don't need to track its reference)
        1 // Pin to core 1
    );

  hw_control = HardwareControl::get_instance();
}


void loop() {
    constexpr size_t REPORT_SIZE = 9;
    static uint8_t prev[REPORT_SIZE] = {128, 128, 128, 128, 0, 0, 8, 0, 0};







    static const uint8_t cmd_map[REPORT_SIZE] =
    {
        UNI_GAMEPAD_MAPPINGS_AXIS_X,
        UNI_GAMEPAD_MAPPINGS_AXIS_Y,
        UNI_GAMEPAD_MAPPINGS_AXIS_RX,
        UNI_GAMEPAD_MAPPINGS_AXIS_RY,
        UNI_GAMEPAD_MAPPINGS_BUTTON_TRIGGER_L,
        UNI_GAMEPAD_MAPPINGS_BUTTON_TRIGGER_R,
        100,
        101,
        102
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
# 366 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Esp32btWiFiControllerGamepad_1.ino"
    // Test Hall sensor inputdebug
    //int hall_read = hw_control->Read_Steering_Hall_Sensor_Input(0.1);
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
        dist = URM09.getDistance(); // Distance in cm
        temp = URM09.getTemperature(); // Temperature in °C
    }

    if (dist < 0)
    {
        // Out of range - dont't stop
        dist = 20 /* minimum distance - stop the car*/ + 10;
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
        cmd.command = 103;
        cmd.param = dist;
        xQueueGenericSend( ( rx_commands_queue ), ( &cmd ), ( 0 ), ( ( BaseType_t ) 0 ) );

        cmd.safety_stop = false;
        cmd.command = 104;
        cmd.param = (int)temp;
        xQueueGenericSend( ( rx_commands_queue ), ( &cmd ), ( 0 ), ( ( BaseType_t ) 0 ) );

        prev_dist = dist;
        prev_temp = temp;
    }
}
