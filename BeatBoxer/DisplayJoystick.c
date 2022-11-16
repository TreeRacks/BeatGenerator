#include "DisplayJoystick.h"
#include "Joystick.h"
#include "Button.h"
#include "audioMixer.h"
#include "mainHelper.h"
#include "LEDMatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>

static pthread_t threadJoystick;
static bool stopDisplay = false;
static int bpm = 120;
static int msDelayPerBeat = 500;
static int volume = 80;

static void* displayJoystickValues(void* arg){
    while(!stopDisplay){
        printStats();
        clearDisplay();
        if(0.5 < joyStickCalculationY()){ //up
            volume += 5;
            AudioMixer_setVolume(volume);
            if(volume >= AUDIOMIXER_MAX_VOLUME){
                volume = AUDIOMIXER_MAX_VOLUME;
                displayInt(99);
            }
            else{
                displayInt(volume);
            }
            
        } else if(-0.5 > joyStickCalculationY()){ //down
            volume -= 5;
            AudioMixer_setVolume(volume);
            if(volume <= 0){
                volume = 0;
                displayInt(0);
            }
            else{
                displayInt(volume);
            }
        } else if(0.5 < joyStickCalculationX()){ //right
            msDelayPerBeat -= 20; //delay sleepForMs(bpmInMs)
            bpm += 5;
            if(bpm > 300){
                bpm = 300;
            }
            if(bpm > 100){
                displayInt(bpm - bpm + 99);
            }
            else{
                displayInt(bpm);
            }
            

        } else if(-0.5 > joyStickCalculationX()){ //left
            msDelayPerBeat += 20;
            bpm -= 5;
            if (bpm < 40){
                bpm = 40;
            }
            if(bpm > 100){
                displayInt(bpm - bpm + 99);
            }
            else{
                displayInt(bpm);
            }
            
        }  else{
            if(getMode() == 0){
                displayMode("M0");
            } else if(getMode() == 1){
                displayMode("M1");
            } else if(getMode() == 2){
                displayMode("M2");
            }
        }
        sleepForMs(500);
    }
    return NULL;
}

int getBPM(){
    return bpm;
}

int getMsDelayPerBeat(){
    return msDelayPerBeat;
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
