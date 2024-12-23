#include "utilities.h"
#include <stdio.h>
#include <string.h>

#define AUG "-08-"
#define SEPT "-09-"
#define OCT "-10-"
#define AUG_MAX 31
#define SEPT_MAX 30
#define OCT_MAX AUG_MAX


int main(void)
{
    int start = 21;
    int month = 0;

    for (int i = 0; i < 3; i++) {
        int max = i == 1 ? SEPT_MAX : AUG_MAX;


        char * curr = NULL;
        switch (i) {
            case 0:
                strcpy(curr, AUG);
                break;
            case 1:
                strcpy(curr, SEPT);
                break;
            case 2:
                strcpy(curr, OCT);
                break;
            default:
                continue;
        }
        
        for (int x = start; x <= max; x++) {

            char * date = NULL;
            char * cmd = "./innings_predict";
            char day[2];
            
            sprintf(day, "%d", x);

            strcat(date, curr);
            strcat(date, day);
            strcat(date, "\0");
            
            strcat(cmd, date);

            system(cmd);
        }

        start = 1;
        month++;
    }
}
