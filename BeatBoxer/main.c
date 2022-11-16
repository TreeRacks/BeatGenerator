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
#include <stdlib.h>

int main(){
    configureI2C();
    initializeStartRegisters();
    configureAllPins();
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    exportAll4Button();


    
    start_startButton();
    Interval_init();
    AudioMixer_init();
    displayJoystick_startDisplay();
    char c;
    // printf("Enter 'Q' to quit.\n");
    while(true){
        scanf("%c",&c);
        if(toupper(c) == 'Q'){
           break;
        }
        sleepForMs(1000);
    }
    AudioMixer_cleanup();
    displayJoystick_stopDisplay();
    stop_stopButton();
    Interval_cleanup();
    
    close(i2cFileDesc);
    printf("\nDone shutdown! Goodbye!\n");
    return 0;
}