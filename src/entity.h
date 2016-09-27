#ifndef ENTITY_H
#define ENTITY_H

/*To do: Combine this with thought.h*/

#include <stdint.h>
#include "thought.h"

/*
 This is a great case of terrible programming.
 When a thought is terminated, swap with last thought in array and decrement length.
*/
struct WorldEntity_
{
  uint8_t thoughts_l, interactions_l, input;
  Thought thoughts[16];
  Interaction interactions[16];
};

/*
 x,y - self explanatory
 xV, yV - x & y velocities
 w, h - width & height
 ss[] - special states (utilizes defined constants to access a specific 'state'. ex. ss[JUMP_COUNT] -= 1)
*/
struct Entity_
{
  float x, y;
  int16_t xV, yV;
  uint16_t w, h, ss[6];
};



#endif
