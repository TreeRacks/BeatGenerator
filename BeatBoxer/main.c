#include "LEDMatrix.h"
#include "Joystick.h"
#include "mainHelper.h"
#include "DisplayJoystick.h"
#include "audioMixer.h"
#include "Button.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

int main(){
    configureI2C();
    initializeStartRegisters();
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    runCommand("config-pin p8.43 gpio");
    runCommand("config-pin -q p8.43");
    exportAll4Button();



    close(i2cFileDesc);
    return 0;
}