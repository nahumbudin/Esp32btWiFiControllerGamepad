#line 1 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\RGBledController.cpp"
/**
 * ============================================================================
 * @file        LP55231Controller.cpp
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

#include "RGBledController.h"

LP55231Controller::LP55231Controller(uint8_t address) 
    : _driver(address), _i2c_addr(address) {
    _mutex = xSemaphoreCreateMutex();
}

LP55231Controller::~LP55231Controller() {
    stopTask();
    if (_mutex != NULL) {
        vSemaphoreDelete(_mutex);
    }
}

bool LP55231Controller::begin(TwoWire& wire_port, bool init_wire, int sda_pin, int scl_pin) {
    if (xSemaphoreTake(_mutex, portMAX_DELAY) != pdTRUE) return false;

    _wire = &wire_port;
    if (init_wire) {
        _wire->begin(sda_pin, scl_pin);
    }

    // Ping the LP55231 on the I2C bus to verify wiring and address
    _wire->beginTransmission(_i2c_addr);
    uint8_t error = _wire->endTransmission();

    if (error != 0) {
        _initialized = false;
        xSemaphoreGive(_mutex);
        return false; // Device did not ACK (check wiring, power, or 0x32..0x35 address)
    }

    _driver.Begin();
    _driver.Enable();
    delay(100); // Allow charge pump and internal oscillator to stabilize
    clear();

    _initialized = true;
    xSemaphoreGive(_mutex);
    return true;
}


void LP55231Controller::clear() {
    for (uint8_t i = 0; i < 9; i++) {
        _driver.SetChannelPWM(i, 0);
    }
}

void LP55231Controller::setPattern(Pattern pattern, uint8_t r, uint8_t g, uint8_t b, uint16_t speed_ms) {
    if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
        _current_pattern = pattern;
        _r = r;
        _g = g;
        _b = b;
        _speed_ms = speed_ms;
        _step = 0;
        xSemaphoreGive(_mutex);
    }
}

uint16_t LP55231Controller::getSpeed() {
    uint16_t spd = 30;
    if (xSemaphoreTake(_mutex, portMAX_DELAY) == pdTRUE) {
        spd = _speed_ms;
        xSemaphoreGive(_mutex);
    }
    return spd;
}

bool LP55231Controller::startTask(uint32_t stack_size, UBaseType_t priority, BaseType_t core) {
    if (_task_handle != NULL) return false;
    BaseType_t res = xTaskCreatePinnedToCore(
        taskTrampoline,
        "LP55231_Task",
        stack_size,
        this,
        priority,
        &_task_handle,
        core
    );
    return (res == pdPASS);
}

void LP55231Controller::stopTask() {
    if (_task_handle != NULL) {
        vTaskDelete(_task_handle);
        _task_handle = NULL;
    }
}

void LP55231Controller::taskTrampoline(void* pv_parameters) {
    LP55231Controller* instance = static_cast<LP55231Controller*>(pv_parameters);
    while (true) {
        instance->update();
        uint16_t delay_time = instance->getSpeed();
        vTaskDelay(pdMS_TO_TICKS(delay_time > 0 ? delay_time : 10));
    }
}

void LP55231Controller::setRgbLed(uint8_t led_index, uint8_t r, uint8_t g, uint8_t b) {
    if (led_index > 2) return;
    uint8_t base = led_index * 3;
    _driver.SetChannelPWM(base + 0, r);
    _driver.SetChannelPWM(base + 1, g);
    _driver.SetChannelPWM(base + 2, b);
}

void LP55231Controller::colorWheel(uint8_t pos, uint8_t& r, uint8_t& g, uint8_t& b) {
    pos = 255 - pos;
    if (pos < 85) {
        r = 255 - pos * 3; g = 0; b = pos * 3;
    } else if (pos < 170) {
        pos -= 85;
        r = 0; g = pos * 3; b = 255 - pos * 3;
    } else {
        pos -= 170;
        r = pos * 3; g = 255 - pos * 3; b = 0;
    }
}

void LP55231Controller::update() {
    if (xSemaphoreTake(_mutex, pdMS_TO_TICKS(10)) != pdTRUE) return;

    switch (_current_pattern) {
        case OFF:
            clear();
            break;

        case SOLID_COLOR:
            for (uint8_t i = 0; i < 3; i++) {
                setRgbLed(i, _r, _g, _b);
            }
            break;

        case BREATHING: {
            float rad = (_step % 100) * (2.0f * PI / 100.0f);
            float factor = (sin(rad - PI / 2.0f) + 1.0f) / 2.0f;
            for (uint8_t i = 0; i < 3; i++) {
                setRgbLed(i, _r * factor, _g * factor, _b * factor);
            }
            _step++;
            break;
        }

        case CHASER: {
            clear();
            uint8_t active_led = _step % 3;
            setRgbLed(active_led, _r, _g, _b);
            _step++;
            break;
        }

        case RAINBOW: {
            for (uint8_t i = 0; i < 3; i++) {
                uint8_t r, g, b;
                colorWheel((_step + (i * 40)) & 0xFF, r, g, b);
                setRgbLed(i, r, g, b);
            }
            _step++;
            break;
        }
    }

    xSemaphoreGive(_mutex);
}
