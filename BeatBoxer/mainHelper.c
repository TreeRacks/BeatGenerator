#include "mainHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


void configureI2C(){
    runCommand("config-pin P9_18 i2c");
    runCommand("config-pin P9_17 i2c");
}

void runCommand(char* command)
{
 // Execute the shell command (output into pipe)
 FILE *pipe = popen(command, "r");
 // Ignore output of the command; but consume it
 // so we don't get an error when closing the pipe.
 char buffer[1024];
 while (!feof(pipe) && !ferror(pipe)) {
 if (fgets(buffer, sizeof(buffer), pipe) == NULL)
 break;
 // printf("--> %s", buffer); // Uncomment for debugging
 }
 // Get the exit code from the pipe; non-zero is an error:
 int exitCode = WEXITSTATUS(pclose(pipe));
 if (exitCode != 0) {
 perror("Unable to execute command:");
 printf(" command: %s\n", command);
 printf(" exit code: %d\n", exitCode);
 }
}

void configureAllPins(){
    runCommand("config-pin p8.15 gpio");
    runCommand("config-pin -q p8.15");
    runCommand("config-pin p8.16 gpio");
    runCommand("config-pin -q p8.16");
    runCommand("config-pin p8.17 gpio");
    runCommand("config-pin -q p8.17");
    runCommand("config-pin p8.18 gpio");
    runCommand("config-pin -q p8.18");
}


