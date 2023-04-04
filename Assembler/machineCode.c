#include "machineCode.h"

void translateToMc(int num, char *mc) {   
    int i;
    for(i=13;i >= 0; --i) {
        if(num & (1 << i)) {
            mc[i] = '/';
        }else {
            mc[i] = '.';
        }
    }
}
