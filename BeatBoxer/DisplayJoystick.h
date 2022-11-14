#ifndef DISPLAYJOYSTICK_H_
#define DISPLAYJOYSTICK_H_


#define I2C_DEVICE_ADDRESS 0x70
#define JoystickLimit 0.5
//static void* displayJoystickValues(void* arg);
void clearDisplay();
void displayJoystick_startDisplay();
void displayJoystick_stopDisplay();

#endif
