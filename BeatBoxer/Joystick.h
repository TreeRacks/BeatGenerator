#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#define A2D_FILE_VOLTAGE "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

double Joystick_readX();
double Joystick_readY();
double joyStickCalculationX();
double joyStickCalculationY();

#endif
