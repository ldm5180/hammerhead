#include "translator.h"
#include <stdlib.h>

double find_voltage(int adc, double ev)
{
/*    if(ev >= table[adc][255][ENG_VAL])
        return table[adc][255][VOLTAGE];

    if(ev <= table[adc][0][ENG_VAL])
        return table[adc][0][VOLTAGE];
*/
    for(int i=0; i<256; i++)
    {
        // if desired ev is between ev of last and current voltage use it
        if( (ev <= table[adc][i][ENG_VAL]) && (ev >= table[adc][i-1][ENG_VAL]) )
        {
           // printf("Eng val = %f\n", table[adc][i][ENG_VAL]);
            return table[adc][i][VOLTAGE];
        }
    }

    return -1;
}

