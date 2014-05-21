#include <stdint.h>
#include <stdlib.h>
#include "SDL.h"
#include "cleanup.h"

/* 
  Uses a neat feature of bitwise operations.
  Pass multiple flags in using bitwise or.
  Check for a particular flag using bitwise and.
*/
void cleanup(uint8_t flags)
{
    if(flags & CLEANUP_SDL_ON) 
        SDL_Quit();
    if(flags & CLEANUP_SUCCESS)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);
}
