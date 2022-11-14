#include "Button.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define greyButtonPath "/sys/class/gpio/gpio47/value"
#define redButtonPath "/sys/class/gpio/gpio46/value"
#define yellowButtonPath "/sys/class/gpio/gpio27/value"
#define greenButtonPath "/sys/class/gpio/gpio65/value"

pthread_t threadButton;
static bool stopButton = false;

static void* Button(void* arg){
    while(!stopButton){
        if(greyButtonPressed()){
            printf("grey was pressed\n");
        } else if(redButtonPressed()){
            printf("red was pressed\n");
        } else if(yellowButtonPressed()){
            printf("yellow was pressed\n");
        } else if(greenButtonPressed()){
            printf("green was pressed\n");
        }
    }
    return NULL;
}

void start_startButton(){
    pthread_create(&threadButton, NULL, Button, NULL);
}

void stop_stopButton(){
    stopButton = true;
    pthread_join(threadButton, NULL);
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

