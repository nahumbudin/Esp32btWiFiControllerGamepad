#line 1 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\Buzzer.h"
/**
 * ============================================================================
 * @file        Buzzer.h
 * @brief       Enables sounding various tones using the SparkFun 
 *              Qwiic Buzzer on ESP32.
 *           
 * @author      [Nahum Budin]
 * @date        27-Nov-2026
 * @version     1.0.0
 *
 * @hardware    ESP32 + SparkFun Qwiic Buzzer (BOB-24474)
 * @dependencies
 *   - Arduino Framework for ESP32
 *   - Wire (I2C)
 *   - SparkFun Qwiic Buzzer Arduino Library ("SparkFun_Qwiic_Buzzer_Arduino_Library.h")
 *
 * ============================================================================
 */

 #include "SparkFun_Qwiic_Buzzer_Arduino_Library.h"

 class Buzzer {
 public:

    enum Tones {
        OFF,
        BEEP_1000_HZ_SHORT,
        BEEP_1000_HZ_LONG
    };

     Buzzer();

     bool begin();
     void playTone(Tones tone, uint8_t volume);
     void stopTone();
 
     private:
     // Add any private members if needed
     QwiicBuzzer buzzer;
 };
 