#include "Button.h"
#include "audioMixer.h"
#include "mainHelper.h"
#include "DisplayJoystick.h"
#include "LEDMatrix.h"
#include "intervalTimer.h"
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
#define gongSound "sounds/GONG.WAV"

pthread_t threadButton;
static bool stopButton, stopPlaying, stopPlayingCustom = false;
static wavedata_t drum, snare, highHat, gong;
static int mode = 0;
static Interval_statistics_t *beatBoxStatistics;

static void* Button(void* arg){

    AudioMixer_readWaveFileIntoMemory(drumSound, &drum);
    AudioMixer_readWaveFileIntoMemory(snareSound, &snare);
    AudioMixer_readWaveFileIntoMemory(highHatSound, &highHat);
    AudioMixer_readWaveFileIntoMemory(gongSound, &gong);
    while(!stopButton){
        if(greyButtonPressed()){
            while(greyButtonPressed()){};
            switchBeatMode();
        } else if(redButtonPressed()){
            while(redButtonPressed()){};
            AudioMixer_queueSound(&drum);
            markAndGetStatistic();
            sleepForMs(100);
        } else if(yellowButtonPressed()){
            while(yellowButtonPressed()){};
            AudioMixer_queueSound(&snare);
            markAndGetStatistic();
            sleepForMs(100);
        } else if(greenButtonPressed()){
            while(greenButtonPressed()){};
            AudioMixer_queueSound(&highHat);
            markAndGetStatistic();
            sleepForMs(100);
        }
        sleepForMs(10);
    }
    AudioMixer_freeWaveFileData(&drum);
    AudioMixer_freeWaveFileData(&snare);
    AudioMixer_freeWaveFileData(&highHat);
    AudioMixer_freeWaveFileData(&gong);
    return NULL;
}

void markAndGetStatistic(){
    Interval_markInterval(INTERVAL_BEAT_BOX);
    Interval_getStatisticsAndClear(INTERVAL_BEAT_BOX, beatBoxStatistics);
}

void printStats()
{
    printf("M%d %dbpm vol:%d  Low [%f  %f] avg %f / %d  Beat[%f  %f] avg %f / %d",mode, getBPM(), 
    AudioMixer_getVolume(), 
    AudioMixer_getMinInterval(), 
    AudioMixer_getMaxInterval(),
    AudioMixer_getAvgInterval(),
    AudioMixer_getNumSamplesInterval(),
    beatBoxStatistics->maxIntervalInMs,
    beatBoxStatistics->minIntervalInMs,
    beatBoxStatistics->avgIntervalInMs,
    beatBoxStatistics->numSamples);
    sleepForMs(1000);
}

void start_startButton(){
    beatBoxStatistics = malloc(sizeof(*beatBoxStatistics));
    pthread_create(&threadButton, NULL, Button, NULL);
}

void stop_stopButton(){
    stopButton = true;
    pthread_join(threadButton, NULL);
}

void checkForButtonPress(){
    if(greyButtonPressed()){
        stopPlaying = true;
        stopPlayingCustom = true;
    }
}

void rockBeat(){
    while(!(stopPlaying)){
        checkForButtonPress();

        int halfBeat = getMsDelayPerBeat()/2;
        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
        AudioMixer_queueSound(&drum);
        markAndGetStatistic();
        checkForButtonPress();
        sleepForMs(halfBeat);
        checkForButtonPress();
        
        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
        sleepForMs(halfBeat);
        checkForButtonPress();

        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
        AudioMixer_queueSound(&snare);
        markAndGetStatistic();
        sleepForMs(halfBeat);
        checkForButtonPress();

        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
        sleepForMs(halfBeat);
        checkForButtonPress();

        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
        AudioMixer_queueSound(&drum);
        markAndGetStatistic();
        sleepForMs(halfBeat);
        checkForButtonPress();

        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
        sleepForMs(halfBeat);
        checkForButtonPress();

        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
        AudioMixer_queueSound(&snare);
        markAndGetStatistic();
        sleepForMs(halfBeat);
        checkForButtonPress();

        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
        sleepForMs(halfBeat);
        checkForButtonPress();
        if(greenButtonPressed()){
            while(greenButtonPressed()){};
            AudioMixer_queueSound(&highHat);
            markAndGetStatistic();
            sleepForMs(100);
        }
        if (redButtonPressed()){
            while(redButtonPressed()){};
            AudioMixer_queueSound(&drum);
            markAndGetStatistic();
            sleepForMs(100);
        }
        if(yellowButtonPressed()){
            while(yellowButtonPressed()){};
            AudioMixer_queueSound(&snare);
            markAndGetStatistic();
            sleepForMs(100);
        } 
        
    }
    //Time For Half Beat [sec] = 60 [sec/min] / BPM / 2 [half-beats per beat]

}


void customBeat(){
    while(!(stopPlayingCustom)){
        checkForButtonPress();

        int halfBeat = getMsDelayPerBeat()/2;
        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
        AudioMixer_queueSound(&drum);
        markAndGetStatistic();
        sleepForMs(halfBeat/2);
        
        sleepForMs(halfBeat/2);
        checkForButtonPress();
        AudioMixer_queueSound(&snare);
        markAndGetStatistic();
        sleepForMs(halfBeat);
        checkForButtonPress();
         AudioMixer_queueSound(&highHat);
         markAndGetStatistic();
        sleepForMs(halfBeat/4);
        checkForButtonPress();
        AudioMixer_queueSound(&highHat);
        markAndGetStatistic();
         sleepForMs(halfBeat/4);
         checkForButtonPress();
         AudioMixer_queueSound(&highHat);
         markAndGetStatistic();
         sleepForMs(halfBeat/2);
        checkForButtonPress();
        AudioMixer_queueSound(&snare);
        markAndGetStatistic();
        
        AudioMixer_queueSound(&gong);
        markAndGetStatistic();
        sleepForMs(halfBeat);
        checkForButtonPress();
        if(greenButtonPressed()){
            while(greenButtonPressed()){};
            AudioMixer_queueSound(&highHat);
            markAndGetStatistic();
            sleepForMs(100);
        }
        if (redButtonPressed()){
            while(redButtonPressed()){};
            AudioMixer_queueSound(&drum);
            markAndGetStatistic();
            sleepForMs(100);
        }
        if(yellowButtonPressed()){
            while(yellowButtonPressed()){};
            AudioMixer_queueSound(&snare);
            markAndGetStatistic();
            sleepForMs(100);
        } 
    }
}


void switchBeatMode(){
    if(mode == 0){
        mode = 1;
        stopPlaying = false;
        rockBeat();
        //Do nothing
    }
    else if(mode == 1){
        mode = 2;
        stopPlayingCustom = false;
        customBeat();
    }
    else if(mode == 2){
        mode = 0;
    }

}

int getMode(){
    return mode;
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

