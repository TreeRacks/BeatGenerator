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
#include <ctype.h>

#define drumSound "sounds/100051__menegass__gui-drum-bd-hard.wav"
#define snareSound "sounds/100058__menegass__gui-drum-snare-hard.wav"
#define highHatSound "sounds/100062__menegass__gui-drum-tom-hi-hard.wav"

int main(){
    configureI2C();
    initializeStartRegisters();
    configureAllPins();
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    exportAll4Button();
    static wavedata_t drum, snare, highHat;
    AudioMixer_readWaveFileIntoMemory(drumSound, &drum);
	AudioMixer_readWaveFileIntoMemory(snareSound, &snare);
	AudioMixer_readWaveFileIntoMemory(highHatSound, &highHat);
    AudioMixer_init();
    printf("Enter 'Q' to quit.\n");
    while (true) {
    // Quit?
    printf("1\n");
        AudioMixer_queueSound(&drum);
        //AudioMixer_queueSound(&snare);
        sleepForMs(1000);
        if (toupper(getchar()) == 'Q') {
            printf("1\n");
            break;
        }
        
    }

    AudioMixer_cleanup();
    

    close(i2cFileDesc);
    return 0;
}