#ifndef THOUGHT_H
#define THOUGHT_H

#include <stdint.h>
#include "common.h"

/**Forward Declarations START**/
typedef struct WorldEntity_ WorldEntity;
typedef struct Entity_ Entity;
typedef struct Thought_ Thought;
typedef struct Interaction_ Interaction;
/**Forward Declarations END**/

/*
 Main will first proccess Thoughts, then Interactions, then Post-Interactions.
 Each Thought and Interaction may be connected to an entity.

 A Thought has the following abilities:
  - Read access on a specific entity.
  - Read access on boolean Information from WorldEntity. (ex. Does this point(x,y) collide with anything?)
  - Add paired Interaction into WorldEntity's Interaction queue.
  - Deletion from WorldEntity's Thought set.
  - Manipulation of an entity's xV and yV values. (This is a special case that i will explan later. its kinda obvs)
 An Interaction has the following abilties:
  - Read/Write access on a specific entity.
  - Deletion of paired Thought.
  - Can not manipulate an entity's x and y values.
 Post-Interactions are the same as Interactions except:
  - Can manipulate an entity's x and y values.

 The reason this seperation is neccesary is best explained by example:
  If wizards one and two turn each other into cats on the same frame, nothing unexpected happens.


 If a Thought is not connected to any Interaction, set 'interact' to NULL.
 If an Interaction is not connected to any Thought, set 'think' to NULL.

 Interactions are deleted after they've been executed.
*/
struct Thought_
{
  Entity* e;
  uint8_t(*think)(WorldEntity*, Thought*);
};

/* Returned by Thoughts. The iterator used to sequence through Thoughts is subtracted by this value.*/
#define THOUGHT_NORMAL 0
#define THOUGHT_REPLACED 1

struct Interaction_
{
  Entity *e, *o;
  void(*interact)(WorldEntity*, Entity*, Entity*);
};

/**Thought and Interaction Prototypes START**/
extern uint8_t think_playerPhysics(WorldEntity*, Thought*);
extern uint8_t think_playerMove(WorldEntity*, Thought*);
extern uint8_t think_playerCollision(WorldEntity*, Thought*);
extern void postInteract_playerPhysics(WorldEntity*, Entity*, Entity*);
extern void postInteract_playerCollision(WorldEntity*, Entity*, Entity*);
/**Thought and Interaction Prototypes END**/

/*
 cid - collision id
 rcp - relative collision position
 cbw, cbh - collision box width & height
 x,y - self explanatory
 xV, yV - x & y velocities
 w, h - width & height
 ss[] - special states (utilizes defined constants to access a specific 'state'. ex. ss[JUMP_COUNT] -= 1)
 selfAction() - Self-Action
  A special interaction that all entities have.
  This is executed after regular Interactions, and are alwys executed as long as the entity exists.
*/
#define MEMCAP_SS 6
struct Entity_
{
  uint8_t cid;
  float x, y;
  int16_t xV, yV;
  uint16_t w, h, ss[MEMCAP_SS];
};

/**List of Collision IDs START**/
#define CID_NONE 0
#define CID_SOLID 1
#define CID_PLAYER 2
#define CID_ENEMY 3
/**List of Collision IDs END**/

/**List of Special States START**/
#define SS_SPEED 0
#define SS_JUMPCOUNT 1
#define SS_JUMPHEIGHT 2
/**List of Special States END**/

/*
 Glorified global variables.
*/
#define MEMCAP_THOUGHTS 32
#define MEMCAP_INTERACTIONS 32
#define MEMCAP_ENTITIES 16
struct WorldEntity_
{
  uint8_t thoughts_l, interactions_l, entities_l, postInteractions_l, input;
  uint32_t delta_t;
  Thought thoughts[MEMCAP_THOUGHTS];
  Interaction interactions[MEMCAP_INTERACTIONS];
  Entity entities[MEMCAP_ENTITIES];
};

/**Structure Manipulation Prototypes START**/
extern void create_WorldEntity(WorldEntity*); /*Special Case*/
/*Returns -1 if error (This is due to a memory cap). Otherwise, returns index.*/
extern uint8_t create_Entity(WorldEntity*, uint8_t, float, float, uint16_t, uint16_t, int16_t, int16_t, uint16_t[]);
extern uint8_t create_Thought(WorldEntity*, Entity*, uint8_t(*)(WorldEntity*, Thought*));
extern uint8_t create_Interaction(WorldEntity*, Entity*, Entity*, void(*)(WorldEntity*, Entity*, Entity*));
extern uint8_t create_PostInteraction(WorldEntity*, Entity*, Entity*, void(*)(WorldEntity*, Entity*, Entity*));
/*Returned by create functions.*/
#define CREATE_FAILURE -1
/*Since WorldEntity is never destroyed it does not need a destroy function.*/
extern void destroy_Entity(WorldEntity*, Entity*);
extern void destroy_Thought(WorldEntity*, Thought*);
extern void destroy_Interaction(WorldEntity*, Interaction*);
extern void destroy_PostInteraction(WorldEntity*, Interaction*);
/**Structure Manipulation Prototypes END**/

/**Helper Functions START**/
void resolve_solidCollision(WorldEntity*, Entity*, Entity*);
uint8_t check_collisionE(Entity*, Entity*);
uint8_t check_collision(float, float, float, float, float, float, float, float); /*Returns True or False*/

uint8_t check_adjacencyE(Entity*, Entity*);
uint8_t check_adjacency(float, float, float, float, float, float, float, float);
/*Return values for check_adjacency. Returns 2nd box's adjacency relative to 1st box.*/
#define ADJACENT_LEFT  B8(00000001)
#define ADJACENT_RIGHT B8(00000010)
#define ADJACENT_UP    B8(00000100)
#define ADJACENT_DOWN  B8(00001000)
#define ADJACENT_Y	   B8(10000000)
#define ADJACENT_X	   B8(01000000)
uint8_t check_relativityE(Entity*, Entity*);
uint8_t check_relativity(float, float, float, float, float, float, float, float);
/*Return values for check_relativity. Returns 2nd box's position relative to 1st box.*/
#define RELATIVE_LEFT  B8(00000001)
#define RELATIVE_RIGHT B8(00000010)
#define RELATIVE_UP    B8(00000100)
#define RELATIVE_DOWN  B8(00001000)
#define RELATIVE_Y	   B8(10000000)
#define RELATIVE_X	   B8(01000000)
/**Helper Functions END**/
#endif
