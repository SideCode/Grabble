#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include "common.h"

/*Each bit is an on/off switch that represents the state of a particular key.*/
#define INPUT_UP B8(00000001)
#define INPUT_DOWN B8(00000010)
#define INPUT_LEFT B8(00000100)
#define INPUT_RIGHT B8(00001000)
#define INPUT_QUIT B8(00010000)

extern uint8_t getInput(uint8_t);

#endif
