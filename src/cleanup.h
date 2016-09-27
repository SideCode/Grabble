#ifndef CLEANUP_H
#define CLEANUP_H

#include <stdint.h>
#include "common.h"

/*Flags*/
#define CLEANUP_FAILURE B8(00000000) /*Not really needed, but used for clarity.*/
#define CLEANUP_SUCCESS B8(00000001)
#define CLEANUP_SDL_ON B8(00000010)

extern void cleanup(uint8_t);

#endif
