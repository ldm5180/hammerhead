#include "translator.h"
#include <math.h>

void set_calibration_const(int adc, int calib, double constant)
{
    // set constant
    calibration_const[adc][calib] = constant;
    // update table
    generate_table(adc);
}

void generate_table(int adc)
{
    for(int i=0; i<256; i++)
    {
       table[adc][i][ENG_VAL] = calibration_const[adc][0] +
                                 calibration_const[adc][1]*(pow(table[adc][i][VOLTAGE], 1)) +
                                 calibration_const[adc][2]*(pow(table[adc][i][VOLTAGE], 2)) +
                                 calibration_const[adc][3]*(pow(table[adc][i][VOLTAGE], 3)) +
                                 calibration_const[adc][4]*(pow(table[adc][i][VOLTAGE], 4)) +
                                 calibration_const[adc][5]*(pow(table[adc][i][VOLTAGE], 5)) +
                                 calibration_const[adc][6]*(pow(table[adc][i][VOLTAGE], 6)); 
    }
}
