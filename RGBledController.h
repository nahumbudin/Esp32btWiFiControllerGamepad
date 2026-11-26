/**
 * ============================================================================
 * @file        LP55231Controller.h
 * @brief       Thread-safe C++ wrapper and lighting engine for the TI LP55231
 *              9-channel programmable LED driver (SparkFun BOB-13884) on ESP32.
 * @details     Provides non-blocking RGB lighting animations (Solid, Breathing,
 *              Chaser, Rainbow) with built-in FreeRTOS background task support
 *              and mutex-protected state transitions.
 *
 * @author      [Nahum Budin]
 * @date        26-Nov-2026
 * @version     1.0.0
 *
 * @hardware    ESP32 + SparkFun LED Driver Breakout - LP55231 (BOB-13884)
 * @dependencies
 *   - Arduino Framework for ESP32
 *   - Wire (I2C)
 *   - SparkFun LP55231 Arduino Library (lp55231.h)
 *
 * @license     MIT License
 * ============================================================================
 */

#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <lp55231.h>

class LP55231Controller {
public:
    /**
     * @brief Available built-in lighting animation modes.
     */
    enum Pattern {
        OFF,            ///< All 9 channels turned off
        SOLID_COLOR,    ///< Static RGB color across all 3 onboard RGB LEDs
        BREATHING,      ///< Smooth sine-wave brightness fade
        CHASER,         ///< Sequential single-LED chase effect
        RAINBOW         ///< Continuous HSV color-wheel cycle across LEDs
    };

    /**
     * @brief Construct a new LP55231Controller object.
     * @param address 7-bit I2C address of the LP55231 (default: 0x32).
     */
    explicit LP55231Controller(uint8_t address = 0x32);

    /**
     * @brief Destroy the LP55231Controller object and clean up FreeRTOS resources.
     */
    ~LP55231Controller();

     /**
     * @brief Initialize I2C (optional) and verify the LP55231 responds on the bus.
     * @param wirePort  Reference to the I2C bus instance (default: Wire).
     * @param initWire  Set to true to call wirePort.begin(sdaPin, sclPin),
     *                  or false if you already called Wire.begin() in setup().
     * @param sdaPin    GPIO pin for I2C SDA (used only if initWire is true).
     * @param sclPin    GPIO pin for I2C SCL (used only if initWire is true).
     * @return true if the LP55231 ACKed on the I2C bus, false if not found.
     */
    bool begin(TwoWire& wire_port = Wire, bool init_wire = true, int sda_pin = 21, int scl_pin = 22);

    /**
     * @brief Set PWM duty cycle to 0 on all 9 LED channels.
     */
    void clear();

    /**
     * @brief Thread-safe method to update the active pattern, color, and speed.
     * @param pattern Target lighting pattern from LP55231Controller::Pattern.
     * @param r       Red intensity (0-255).
     * @param g       Green intensity (0-255).
     * @param b       Blue intensity (0-255).
     * @param speedMs Frame interval in milliseconds (lower = faster animation).
     */
    void setPattern(Pattern pattern, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint16_t speed_ms = 30);

    /**
     * @brief Thread-safe getter for the current frame interval speed.
     * @return uint16_t Frame interval in milliseconds.
     */
    uint16_t getSpeed();

    /**
     * @brief Spawn a dedicated FreeRTOS background task to drive animations.
     * @param stackSize Task stack size in bytes (default: 3072).
     * @param priority  FreeRTOS task priority (default: 1).
     * @param core      ESP32 CPU core ID: 0 or 1 (default: 1).
     * @return true if the task was created successfully, false otherwise.
     */
    bool startTask(uint32_t stack_size = 3072, UBaseType_t priority = 1, BaseType_t core = 1);

    /**
     * @brief Stop and delete the running FreeRTOS background task.
     */
    void stopTask();

    /**
     * @brief Render a single animation frame. Called automatically when running
     *        as a FreeRTOS task, or call manually in loop() for cooperative mode.
     */
    void update();

private:
    Lp55231 _driver;
    TwoWire* _wire = &Wire;
    uint8_t _i2c_addr;
    bool _initialized = false;

    // Pattern state
    Pattern _current_pattern = OFF;
    uint8_t _r = 0;
    uint8_t _g = 0;
    uint8_t _b = 0;
    uint16_t _speed_ms = 30;
    uint16_t _step = 0;

    // FreeRTOS handles
    TaskHandle_t _task_handle = NULL;
    SemaphoreHandle_t _mutex = NULL;

    // Internal helpers
    static void taskTrampoline(void* pv_parameters);
    void setRgbLed(uint8_t led_index, uint8_t r, uint8_t g, uint8_t b);
    void colorWheel(uint8_t pos, uint8_t& r, uint8_t& g, uint8_t& b);
};


