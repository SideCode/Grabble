#include "SDL.h"
#include "input.h"
#include "common.h"
#include "cleanup.h"

/*Takes in the current key state, updates it, and returns it.*/
uint8_t getInput(uint8_t i)
{
    SDL_Event event;
	
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT: 
                i = i | INPUT_QUIT;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        i = i | INPUT_QUIT;
                        break;
                    case SDLK_UP:
                        i = i | INPUT_UP;
                        break;
                    case SDLK_LEFT:
                        i = i | INPUT_LEFT;
                        break;
                    case SDLK_RIGHT:
                        i = i | INPUT_RIGHT;
                        break;
                    case SDLK_DOWN:
                        i = i | INPUT_DOWN;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym)
                {
                    case SDLK_UP:
                        i = i & ~INPUT_UP; /*Chek 0ut dem (albeit fairly simple) bit-triks.*/
                        break;
                    case SDLK_LEFT:
                        i = i & ~INPUT_LEFT;
                        break;
                    case SDLK_RIGHT:
                        i = i & ~INPUT_RIGHT;
                        break;
                    case SDLK_DOWN:
                        i = i & ~INPUT_DOWN;
                        break;
                    default:
                        break;
                }
                break;
        }
    }
    
    return i;
}
