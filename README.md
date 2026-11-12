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
*	History:\n
*
*/
