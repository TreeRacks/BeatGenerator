#include "LEDMatrix.h"
#include "Joystick.h"
#include "mainHelper.h"
#include "intervalTimer.h"
#include "DisplayJoystick.h"
#include "audioMixer.h"
#include "Button.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>

int main(){
    configureI2C();
    initializeStartRegisters();
    configureAllPins();
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    exportAll4Button();


    displayJoystick_startDisplay();
    Interval_init();
    AudioMixer_init();
    start_startButton();
    
    
    //printf("Enter 'Q' to quit.\n");
    whileLoop();
    stop_stopButton();
    Interval_cleanup();
    AudioMixer_cleanup();
    displayJoystick_stopDisplay();
    

    close(i2cFileDesc);
    printf("\nDone shutdown! Goodbye!\n");
    return 0;
}