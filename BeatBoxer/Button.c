#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

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
    for(int i = 15; i < 19; i++){
        writingToGPIO(i);
    }
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
    //printf("Read: '%s'\n", buff);
    return(atoi(buff));
}