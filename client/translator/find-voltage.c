#include "translator.h"
#include <stdlib.h>

double find_voltage(unsigned long int adc, double ev)
{
    // two types of cooked values are generated in the table
    // type one increases as voltage increases and type to decreases as voltage increases
    // must use different comparison for the two types

    // determine type
    double type = table[adc][0][ENG_VAL] - table[adc][1][ENG_VAL];

    // type one
    if(type < 0)
    {
        // check if ev is out of range of possible ev's. if so return nearest corresponding voltage
        if(ev > table[adc][255][ENG_VAL])
            return table[adc][255][VOLTAGE];
        else if(ev < table[adc][0][ENG_VAL]) 
            return table[adc][0][VOLTAGE];

        for(int i=1; i<256; i++)
        {
            // if desired ev is between ev of last and current voltage use it
            if( (ev <= table[adc][i][ENG_VAL]) && (ev >= table[adc][i-1][ENG_VAL]) )
            {
                return table[adc][i][VOLTAGE];
            }
        }
    }
    // type two
    else if(type > 0)
    {
        // check if ev is out of range of possible ev's. if so return nearest corresponding voltage
        if(ev > table[adc][0][ENG_VAL])
            return table[adc][0][VOLTAGE];
        else if(ev < table[adc][255][ENG_VAL])
            return table[adc][255][ENG_VAL];

        for(int i=1; i<256; i++)
        {
            // if desired ev is between ev of last and current voltage use it
            if( (ev >= table[adc][i][ENG_VAL]) && (ev <= table[adc][i-1][ENG_VAL]) )
            {
                return table[adc][i][VOLTAGE];
            }
        }
    }

    return -1;
}

