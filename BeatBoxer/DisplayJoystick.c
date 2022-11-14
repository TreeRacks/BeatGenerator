#include "DisplayJoystick.h"
#include "Joystick.h"
#include "LEDMatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>

pthread_t threadJoystick;
static bool stopDisplay = false;


static void* displayJoystickValues(void* arg){
    while(!stopDisplay){

        clearDisplay();
        // if(JoystickLimit < joyStickCalculationY()){ //up
        //     displayDec(getMaxValue());
        // } else if(-JoystickLimit > joyStickCalculationY()){ //down
        //     displayDec(getMinValue()); 
        // } else if(JoystickLimit < joyStickCalculationX()){ //right
        //     displayDec(getMaxInterval() / 1000000.0);
        // } else if(-JoystickLimit > joyStickCalculationX()){ //left
        //     displayDec(getMinInterval() / 1000000.0);
        // } else{
        //     displayInt(getNumberOfDips()); //centre
        // }
        // sleepForMs(200);
    }
    return NULL;
}

void clearDisplay(){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    for(int i = 0; i < 16; i+=2){
        writeI2cReg(i2cFileDesc, i, 0x00);
    }
}

void displayJoystick_startDisplay(){
    pthread_create(&threadJoystick, NULL, displayJoystickValues, NULL);
}

void displayJoystick_stopDisplay(){
    stopDisplay = true;
    pthread_join(threadJoystick, NULL);
}
