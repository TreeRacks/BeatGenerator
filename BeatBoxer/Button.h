#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdbool.h>

int readButton(char *button);
void writingToGPIO(float value);
void exportAll4Button();
bool greyButtonPressed();
bool redButtonPressed();
bool yellowButtonPressed();
bool greenButtonPressed();

#endif