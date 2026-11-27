/**
 * ============================================================================
 * @file        Buzzer.cpp
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

 #include "Buzzer.h"

Buzzer::Buzzer() 
{
    //check if buzzer will connect over I2C
  if (buzzer.begin() == false) {
    Serial.println("Buzzer did not connect! Freezing.");
    while (1);
  }
  buzzer.off();
  Serial.println("Buzzer connected.");
}

void Buzzer::stopTone() 
{
  buzzer.off();
}

void Buzzer::playTone(Tones tone, uint8_t volume) 
{
  if (tone == OFF) 
  {
    buzzer.off();
  } 
  else if (tone == BEEP_1000_HZ_SHORT) 
  {
    buzzer.configureBuzzer(1000, 100, volume);
    buzzer.on();
  } 
  else if (tone == BEEP_1000_HZ_LONG) 
  {
    buzzer.configureBuzzer(1000, 500, volume);
    buzzer.on();
  }
  else
  {
    buzzer.off();
  } 
}
