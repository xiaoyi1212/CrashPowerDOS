#ifndef CPOS_VBE_H
#define CPOS_VBE_H

#include <stdint.h>

struct register_16{

}reg16_t;

struct IVTE {
    uint16_t IP;
    uint16_t CS;
};
struct IVTE IVT[256];

void Int();

#endif
