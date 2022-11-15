#include "Button.h"
#include "audioMixer.h"
#include "mainHelper.h"
#include "DisplayJoystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define greyButtonPath "/sys/class/gpio/gpio47/value"
#define redButtonPath "/sys/class/gpio/gpio46/value"
#define yellowButtonPath "/sys/class/gpio/gpio27/value"
#define greenButtonPath "/sys/class/gpio/gpio65/value"

#define drumSound "sounds/100051__menegass__gui-drum-bd-hard.wav"
#define snareSound "sounds/100058__menegass__gui-drum-snare-hard.wav"
#define highHatSound "sounds/100062__menegass__gui-drum-tom-hi-hard.wav"

pthread_t threadButton;
static bool stopButton = false;
static wavedata_t drum, snare, highHat;
static int mode = 0;


static void* Button(void* arg){
    AudioMixer_readWaveFileIntoMemory(drumSound, &drum);
    AudioMixer_readWaveFileIntoMemory(snareSound, &snare);
    AudioMixer_readWaveFileIntoMemory(highHatSound, &highHat);
    while(!stopButton){
        if(greyButtonPressed()){
            while(greyButtonPressed()){};
            switchBeatMode();
        } else if(redButtonPressed()){
            while(redButtonPressed()){};
            AudioMixer_queueSound(&drum);
            sleepForMs(100);
        } else if(yellowButtonPressed()){
            while(yellowButtonPressed()){};
            AudioMixer_queueSound(&snare);
            sleepForMs(100);
        } else if(greenButtonPressed()){
            while(greenButtonPressed()){};
            AudioMixer_queueSound(&highHat);
            sleepForMs(100);
        }
        sleepForMs(10);
    }
    AudioMixer_freeWaveFileData(&drum);
    AudioMixer_freeWaveFileData(&snare);
    AudioMixer_freeWaveFileData(&highHat);
    return NULL;
}

void start_startButton(){
    pthread_create(&threadButton, NULL, Button, NULL);
}

void stop_stopButton(){
    stopButton = true;
    pthread_join(threadButton, NULL);
}


void rockBeat(){
    
        printf("delay is %d\n", getMsDelayPerBeat());
        int halfBeat = getMsDelayPerBeat()/2;
        AudioMixer_queueSound(&highHat);
        AudioMixer_queueSound(&drum);
        sleepForMs(halfBeat);
        
        AudioMixer_queueSound(&highHat);
        sleepForMs(halfBeat);

        AudioMixer_queueSound(&highHat);
        AudioMixer_queueSound(&snare);
        sleepForMs(halfBeat);

        AudioMixer_queueSound(&highHat);
        sleepForMs(halfBeat);

        AudioMixer_queueSound(&highHat);
        AudioMixer_queueSound(&drum);
        sleepForMs(halfBeat);

        AudioMixer_queueSound(&highHat);
        sleepForMs(halfBeat);

        AudioMixer_queueSound(&highHat);
        AudioMixer_queueSound(&snare);
        sleepForMs(halfBeat);

        AudioMixer_queueSound(&highHat);
        sleepForMs(halfBeat);

        if(greyButtonPressed()){
            //do nothing aka no recursion
        } else if (greenButtonPressed()){
            AudioMixer_queueSound(&highHat);
            sleepForMs(100);
            rockBeat();
        }else if (redButtonPressed()){
            AudioMixer_queueSound(&drum);
            sleepForMs(100);
            rockBeat();
        }else if(yellowButtonPressed()){
            AudioMixer_queueSound(&snare);
            sleepForMs(100);
            rockBeat();
        }else if (mode == 2){
            rockBeat();
        }

    //Time For Half Beat [sec] = 60 [sec/min] / BPM / 2 [half-beats per beat]
}


void switchBeatMode(){
    if(mode == 0){
                    printf("1\n");
        mode += 1;
        // displayInt();
        //Do nothing
    }
    else if(mode == 1){
                    printf("2\n");
        mode += 1;
        rockBeat();
    }
    else if(mode == 2){
                    printf("3\n");
        mode = 0;
        //customBeat();
    }

}

void customBeat(){

}

void writingToGPIO(float value){
    FILE *pFile = fopen("/sys/class/gpio/export", "w");
    if (pFile == NULL) {
        printf("ERROR: Unable to open export file.\n");
        exit(1);
    }
    fprintf(pFile, "%f", value);
    fclose(pFile);
}

void exportAll4Button(){
    writingToGPIO(47);
    writingToGPIO(46);
    writingToGPIO(27);
    writingToGPIO(65);
}

bool greyButtonPressed(){
    return (readButton(greyButtonPath) == 1);
}

bool redButtonPressed(){
    return (readButton(redButtonPath) == 1);
}

bool yellowButtonPressed(){
    return (readButton(yellowButtonPath) == 1);
}

bool greenButtonPressed(){
    return (readButton(greenButtonPath) == 1);
}

int readButton(char *button)
{
    FILE *pFile = fopen(button, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", button);
        exit(-1);
    }
    // Read string (line)
    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);
    // Close
    fclose(pFile);
    // printf("Read: '%s'\n", buff);
    return(atoi(buff));
}

