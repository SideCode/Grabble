#include <stdio.h>
#include <stdint.h>
#include "SDL.h"
#include "cleanup.h"
#include "input.h"
#include "common.h"
#include "thought.h"

#define BOX_COLOR SDL_MapRGB(screen->format,10,10,230)
#define WHITE SDL_MapRGB(screen->format,254,254,254)
#define FPS 100
#define FPS_MINIMUM 20

#define SPEED_OF_LIGHT 19

int main(int argc, char *argv[])
{
    /**Start: Declaration**/
    SDL_Surface* screen;
    SDL_Rect box; /*used purely for drawing onto the screen*/
    WorldEntity worldEntity;
    uint32_t time,sleep_time,frame,fps_time;
	int i; /*Used in 'for loops'. Compatability issues disallow declarations in 'for loops'.*/
    /**End: Declaration**/

    /**Start: Initialization**/
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error Initializing SDL: %s\n", SDL_GetError());
        cleanup(CLEANUP_FAILURE);
    }

    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,32,SDL_SWSURFACE);
    if(screen == NULL)
    {
        fprintf(stderr, "Error Creating Screen: %s\n", SDL_GetError());
        cleanup(CLEANUP_SDL_ON | CLEANUP_FAILURE);
    }
    frame = 0;
    fps_time = 0;
    
    SDL_WM_SetCaption(NULL,"Grabble");
    
    time = SDL_GetTicks();
    
    create_WorldEntity(&worldEntity);
	
	uint16_t temp[6];
	temp[SS_SPEED] = 150;
	temp[SS_JUMPCOUNT] = 1;
	temp[SS_JUMPHEIGHT] = 300;
	
	i = create_Entity(&worldEntity,CID_PLAYER,20,20,20,24,0,0,temp);
	
	create_Thought(&worldEntity, &worldEntity.entities[i],&think_playerPhysics);
	create_Thought(&worldEntity, &worldEntity.entities[i],&think_playerMove);
	create_Thought(&worldEntity, &worldEntity.entities[i],&think_playerCollision);
	
	create_Entity(&worldEntity,CID_SOLID,0,160,320,20,0,0,NULL);
    create_Entity(&worldEntity,CID_SOLID,110,100,100,25,0,0,NULL);
    create_Entity(&worldEntity,CID_SOLID,150,125,20,35,0,0,NULL);
    create_Entity(&worldEntity,CID_SOLID,0,0,20,160,0,0,NULL);
    /**End: Initialization**/

    /**Start: Program Loop**/
    while(!(worldEntity.input & INPUT_QUIT))
    {	
		/*TIME STUFF*/
		worldEntity.delta_t = SDL_GetTicks() - time;
        time = SDL_GetTicks();
		fps_time += worldEntity.delta_t;
		worldEntity.delta_t = worldEntity.delta_t > 1000 / FPS_MINIMUM ? 1000 / FPS_MINIMUM : worldEntity.delta_t;
		
		/*Bootleg FPS Display*/
		//printf("FPS:%f yV:%i y:%f x:%f j:%i\n",(frame / (float)fps_time) * 1000, worldEntity.entities[0].yV,
		//worldEntity.entities[0].y,worldEntity.entities[0].x,worldEntity.entities[0].ss[SS_JUMPCOUNT]);
		
		if(fps_time > 100)
		{	
			frame = 0;
			fps_time = 0;
		}
		
		/*OH YEAHH. SWEET INPUT*/
		worldEntity.input = getInput(worldEntity.input);
        
        /*Friction Updates*/
		for(i = 0; i < worldEntity.entities_l; i++)
			worldEntity.entities[i].xV = 0;
        
        /*Thought Machine*/
        for(i = 0; i < worldEntity.thoughts_l; i++)
        {
			i -= worldEntity.thoughts[i].think(&worldEntity,&worldEntity.thoughts[i]);
		}
		
		/*Interaction Machine*/
		while(worldEntity.interactions_l > 0)
        {
			worldEntity.interactions[0].interact(&worldEntity,worldEntity.interactions[0].e,worldEntity.interactions[0].o);
			destroy_Interaction(&worldEntity, &worldEntity.interactions[0]);
		}
		
		/*Position Updates*/
		for(i = 0; i < worldEntity.entities_l; i++)
		{
			float xV,yV;
			xV = worldEntity.entities[i].xV * (worldEntity.delta_t / 1000.f);
			yV = worldEntity.entities[i].yV * (worldEntity.delta_t / 1000.f);
			
			if(ABS(xV) > SPEED_OF_LIGHT)
				xV = xV > 0 ? SPEED_OF_LIGHT : -SPEED_OF_LIGHT;
			if(ABS(yV) > SPEED_OF_LIGHT)
				yV = yV > 0 ? SPEED_OF_LIGHT : -SPEED_OF_LIGHT;
				
			/*delta_d = s*delta_t & ms to s*/
			worldEntity.entities[i].x += xV;
			worldEntity.entities[i].y += yV;
		}
		
		/*Post-Interaction Machine*/
		while(worldEntity.postInteractions_l > 0)
		{
			worldEntity.interactions[MEMCAP_INTERACTIONS - 1].interact(&worldEntity,worldEntity.interactions[MEMCAP_INTERACTIONS - 1].e,worldEntity.interactions[MEMCAP_INTERACTIONS - 1].o);
			destroy_PostInteraction(&worldEntity, &worldEntity.interactions[MEMCAP_INTERACTIONS - 1]);
		}
		
		/**Render START**/
		SDL_FillRect(screen,NULL,WHITE);
		
		/*SDL_Rect's position and dimensions will morph to accomodate going offscreen.*/
		for(i = 0; i < worldEntity.entities_l; i++)
		{
			box.x = worldEntity.entities[i].x;
			box.y = worldEntity.entities[i].y;
			box.w = worldEntity.entities[i].w;
			box.h = worldEntity.entities[i].h;
			SDL_FillRect(screen,&box,BOX_COLOR);
		}
		
        if(SDL_Flip(screen) == -1)
        {
            fprintf(stderr, "Error Displaying Screen: %s\n", SDL_GetError());
            cleanup(CLEANUP_SDL_ON | CLEANUP_FAILURE);
        }
        /**Render END**/
		
		/*Frame Cap and TIME STUFF*/
		sleep_time = SDL_GetTicks() - time;
		
		SDL_Delay(sleep_time < 1000 / FPS ? 1000.f / FPS - sleep_time : 1); /* prevents the program from sucking up all of the CPU power*/

		frame++;
    }
    /**End: Program Loop**/

	cleanup(CLEANUP_SDL_ON | CLEANUP_SUCCESS);
	
    return 0; /*should never reach this line*/
}
