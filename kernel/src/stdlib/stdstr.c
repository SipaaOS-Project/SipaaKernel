#include "stdstr.h"

char* strcat(char* destination, const char* source) {
    char *walk = destination;
    while (*walk) walk++;
    while (*source) *walk++ = *source++;
    *walk = 0;
    return destination;
}