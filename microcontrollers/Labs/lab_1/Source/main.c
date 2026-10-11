
#include <xc.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config CP = OFF

unsigned char TMP;

void main(void) {
    TMP = 0;

    while (1) {
        TMP = TMP + 2;
        TMP--;
    }

    return;
}
  
