#include "Random.h"

// this is used in a lot of methods, so localizing it to prevent include issues

// GLOBAL: TOY2 0x0084C3E0
uint8_t g_randDatBuffer[2048];

// GLOBAL: TOY2 0x0052ADAC
uint8_t* g_randDatBufferPtr;