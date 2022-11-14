#include "Joystick.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
//X-axis: right is 4095, left is 0
//Y-axis: up is is 4095, down is 0


double Joystick_readX()
{
  // Open file
  char A2D_FILE_VOLTAGE2[100];
  sprintf(A2D_FILE_VOLTAGE2, A2D_FILE_VOLTAGE, 2); 
  FILE *f = fopen(A2D_FILE_VOLTAGE2, "r");
  if (!f) {
    printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
    printf(" Check /boot/uEnv.txt for correct options.\n");
    exit(-1);
  }
  // Get reading
  int a2dReading = 0;
  int itemsRead = fscanf(f, "%d", &a2dReading);
  if (itemsRead <= 0) {
    printf("ERROR: Unable to read values from voltage input file.\n");
    exit(-1);
  }
  // Close file
  fclose(f);
  return a2dReading;
}

double Joystick_readY()
{
  // Open file
  char A2D_FILE_VOLTAGE3[100];
  sprintf(A2D_FILE_VOLTAGE3, A2D_FILE_VOLTAGE, 3); 
  FILE *f = fopen(A2D_FILE_VOLTAGE3, "r");
  if (!f) {
    printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
    printf(" Check /boot/uEnv.txt for correct options.\n");
    exit(-1);
  }
  // Get reading
  int a2dReading = 0;
  int itemsRead = fscanf(f, "%d", &a2dReading);
  if (itemsRead <= 0) {
    printf("ERROR: Unable to read values from voltage input file.\n");
    exit(-1);
  }
  // Close file
  fclose(f);
  return a2dReading;
}

double joyStickCalculationX(){
  double read_x = Joystick_readX();
  //to get interval [-1,1]
  double p2 = (double)read_x/ 4095;
  double x = -1*(p2 -0.5)*2;

  return x; 
}

double joyStickCalculationY(){
  double read_y = Joystick_readY();
  //to get interval [-1,1]
  double p1 = (double)read_y/ 4095;
  double y = -1*(p1 -0.5)*2;
  return y;
}