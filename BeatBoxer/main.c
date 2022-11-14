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
    configureAllPins();
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    exportAll4Button(); 
 
    while(true){
        
    }

    close(i2cFileDesc);
    return 0;
}