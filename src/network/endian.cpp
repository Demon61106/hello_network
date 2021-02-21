#include "endian.h"

#include <stdio.h>

void byteOrder() {
    union {
        short value;
        char bytes[sizeof(short)];
    } test;
    test.value = 0x0102;
    if (test.bytes[0] == 0x01 && test.bytes[1] == 0x02) {
        printf("this is big endian\n");
    } else if (test.bytes[0] == 0x02 && test.bytes[1] == 0x01) {
        printf("this is little endian\n");
    } else {
        printf("unknow");
    }
    return ;
}