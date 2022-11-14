#ifndef mainHelper_H_
#define mainHelper_H_
#include <stdbool.h>

#define userButton "/sys/class/gpio/gpio72/value"

void runCommand(char* command);
void configureI2C();
// void pressButtonToEndProgram();

#endif