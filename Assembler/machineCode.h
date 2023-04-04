#ifndef MACHINECODE_H
#define MACHINECODE_H
/**
 * @brief a struct that contains the machine code of a given word
 * 
 */
typedef struct Mcode {
    unsigned int bin_code:14;
    char word[30];
    int lineNum;
}machine_code;

/**
 * @brief translate a number to a machine code
 * 
 * @param num the number
 * @param mc the machine code
 */
void translateToMc(int num, char *mc);
#endif
