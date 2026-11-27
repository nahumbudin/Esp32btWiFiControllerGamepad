#line 1 "I:\\Workspace\\ESP32\\Maker-ESP32-Pro\\Esp32btWiFiControllerGamepad_1\\README.md"
# Esp32btWiFiControllerGamepad

An Arduino sketch for the **Maker-ESP32-Pro** board that provides dual-mode wireless gamepad control via **Bluetooth** (Sony PS4 DualShock 4) or **TCP WiFi** (Android PS4 Gamepad emulator).

## Overview

This project allows an ESP32 controller to receive gamepad inputs to drive hardware elements such as motors and sensors (optimized for LEGO Technic motors). 

* **Bluetooth Mode:** Connects directly to a physical Sony PS4 Gamepad.
* **WiFi Mode:** Connects over TCP WiFi to a modified version of the [Blugaemand](https://github.com/pablopenna/blugaemand) Android gamepad emulation app.
* **Mode Selection:** Determined at startup (`setup()`) by reading the state of an external physical switch connected to a designated GPIO pin.

## Hardware & Environment

* **Target Board:** [Maker-ESP32-Pro](https://github.com/nulllaborg/maker-esp32-pro) (optimized for LEGO motors, servos, and sensors)
* **Arduino Board Selection:** `esp32_bluepad32` -> `ESP32 Dev Module`

## Project Details

* **File:** `Esp32btWiFiControllerGamepad.ino`
* **Author:** Nahum Budin
* **Date:** November 1, 2026
* **Version:** 1.0 (Initial version)

## Version History

* **v1.0** (1-Nov-2026) – Initial version.
