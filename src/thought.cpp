#include <stdio.h>
#include "thought.h"
#include "input.h"

/**Structure Manipulation Functions START**/
void create_WorldEntity(WorldEntity* wE)
{
  wE->thoughts_l = 0;
  wE->interactions_l = 0;
  wE->postInteractions_l = 0;
  wE->entities_l = 0;
  wE->input = 0;
  wE->delta_t = 0;
}

uint8_t create_Entity(WorldEntity* wE, uint8_t cid, float x, float y, uint16_t w, uint16_t h, int16_t xV, int16_t yV, uint16_t ss[])
{
  if(wE->entities_l == MEMCAP_ENTITIES)
    return CREATE_FAILURE;

  wE->entities[wE->entities_l].cid = cid;
  wE->entities[wE->entities_l].x = x;
  wE->entities[wE->entities_l].y = y;
  wE->entities[wE->entities_l].w = w;
  wE->entities[wE->entities_l].h = h;
  wE->entities[wE->entities_l].xV = xV;
  wE->entities[wE->entities_l].yV = yV;

  if(ss != NULL)
  {
    int i;
    for(i = 0; i < MEMCAP_SS; i++)
      wE->entities[wE->entities_l].ss[i] = ss[i];
  }

  return wE->entities_l++;
}

uint8_t create_Thought(WorldEntity* wE, Entity* e, uint8_t(*t)(WorldEntity*, Thought*))
{
  if(wE->thoughts_l == MEMCAP_THOUGHTS)
    return CREATE_FAILURE;

  wE->thoughts[wE->thoughts_l].e = e;
  wE->thoughts[wE->thoughts_l].think = t;

  return wE->thoughts_l++;
}

uint8_t create_Interaction(WorldEntity* wE, Entity* e, Entity* o, void(*i)(WorldEntity*, Entity*, Entity*))
{
  if(wE->interactions_l == (MEMCAP_INTERACTIONS - wE->postInteractions_l))
    return CREATE_FAILURE;

  wE->interactions[wE->interactions_l].e = e;
  wE->interactions[wE->interactions_l].o = o;
  wE->interactions[wE->interactions_l].interact = i;

  return wE->interactions_l++;
}

uint8_t create_PostInteraction(WorldEntity* wE, Entity* e, Entity* o, void(*i)(WorldEntity*, Entity*, Entity*))
{
  if(wE->interactions_l == (MEMCAP_INTERACTIONS - wE->postInteractions_l))
    return CREATE_FAILURE;

  wE->postInteractions_l++;
  wE->interactions[MEMCAP_INTERACTIONS - wE->postInteractions_l].e = e;
  wE->interactions[MEMCAP_INTERACTIONS - wE->postInteractions_l].o = o;
  wE->interactions[MEMCAP_INTERACTIONS - wE->postInteractions_l].interact = i;

  return wE->postInteractions_l;
}

/*
 Unsafe frees
*/
void destroy_Entity(WorldEntity* wE, Entity* e)
{
  wE->entities_l--;
  if(wE->entities_l != 0)
  {
    e->x = wE->entities[wE->entities_l].x;
    e->y = wE->entities[wE->entities_l].y;
    e->xV = wE->entities[wE->entities_l].xV;
    e->yV = wE->entities[wE->entities_l].yV;

    int i;
    for(i = 0; i < MEMCAP_SS; i++)
      e->ss[i] = wE->entities[wE->entities_l].ss[i];
  }
}

void destroy_Thought(WorldEntity* wE, Thought* t)
{
  wE->thoughts_l--;
  if(wE->thoughts_l != 0)
  {
    t->e = wE->thoughts[wE->thoughts_l].e;
    t->think = wE->thoughts[wE->thoughts_l].think;
  }
}

void destroy_Interaction(WorldEntity* wE, Interaction* i)
{
  wE->interactions_l--;
  if(wE->interactions_l != 0)
  {
    i->e = wE->interactions[wE->interactions_l].e;
    i->o = wE->interactions[wE->interactions_l].e;
    i->interact = wE->interactions[wE->interactions_l].interact;
  }
}

void destroy_PostInteraction(WorldEntity* wE, Interaction* i)
{
  if(wE->postInteractions_l != 0)
  {
    i->e = wE->interactions[MEMCAP_INTERACTIONS - wE->postInteractions_l].e;
    i->o = wE->interactions[MEMCAP_INTERACTIONS - wE->postInteractions_l].o;
    i->interact = wE->interactions[MEMCAP_INTERACTIONS - wE->postInteractions_l].interact;
  }
  wE->postInteractions_l--;
}
/**Structure Manipulation Functions END**/

/**Thoughts and Interactions START**/
#define GRAVITY .5
#define GRAVITY_MAX 250

uint8_t think_playerPhysics(WorldEntity* wE, Thought* p)
{
  p->e->yV += GRAVITY * wE->delta_t; /*Required because Gravity is acceleration*/

  if(p->e->yV > GRAVITY_MAX)
    p->e->yV = GRAVITY_MAX;

  create_PostInteraction(wE, p->e, NULL, &postInteract_playerPhysics);

  return THOUGHT_NORMAL;
}

uint8_t think_playerMove(WorldEntity* wE, Thought* p)
{
  if(wE->input & INPUT_LEFT)
    p->e->xV -= p->e->ss[SS_SPEED];
  if(wE->input & INPUT_RIGHT)
    p->e->xV += p->e->ss[SS_SPEED];

  if((wE->input & INPUT_UP) && (p->e->ss[SS_JUMPCOUNT] > 0))
  {
    p->e->ss[SS_JUMPCOUNT] -= 1;
    p->e->yV -= p->e->ss[SS_JUMPHEIGHT];
  }

  return THOUGHT_NORMAL;
}

uint8_t think_playerCollision(WorldEntity* wE, Thought* t)
{
  create_PostInteraction(wE, t->e, NULL, &postInteract_playerCollision);
  return THOUGHT_NORMAL;
}

void postInteract_playerCollision(WorldEntity* wE, Entity* e, Entity* o)
{
  uint8_t i;
  e->ss[SS_JUMPCOUNT] = 0;
  for(i = 0; i < wE->entities_l; i++)
  {
    if(wE->entities[i].cid == CID_SOLID && check_collisionE(e, &wE->entities[i]))
    {
      resolve_solidCollision(wE, e, &wE->entities[i]);
      uint8_t r = check_relativityE(e, &wE->entities[i]);
      if(r & RELATIVE_DOWN)
      {
        e->yV = 0;
        e->ss[SS_JUMPCOUNT] = 1;
      }
      if(r & RELATIVE_UP)
      {
        e->yV = 0;
      }
    }
  }
}

void postInteract_playerPhysics(WorldEntity* wE, Entity* e, Entity* o)
{
  /*horizontal bounding*/
  e->x = e->x < 0 ? 0 : e->x;
  e->x = (e->x > SCREEN_WIDTH - e->w) ? (SCREEN_WIDTH - e->w) : e->x;
}
/**Thoughts and Interactions END**/

/**Helper Functions START**/
uint8_t check_collisionE(Entity* e, Entity* o)
{
  return check_collision(e->x, e->y, e->x + e->w, e->y + e->h, o->x, o->y, o->x + o->w, o->y + o->h);
}

uint8_t check_collision(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
  uint8_t xC, yC;

  /*check x collision*/
  xC = ((x1 >= x3 && x1 < x4) || (x3 >= x1 && x3 < x2)) ? TRUE : FALSE;

  /*check y collision*/
  yC = ((y1 >= y3 && y1 < y4) || (y3 >= y1 && y3 < y2)) ? TRUE : FALSE;

  return xC && yC;
}

uint8_t check_relativityE(Entity* e, Entity* o)
{
  return check_relativity(e->x, e->y, e->x + e->w, e->y + e->h, o->x, o->y, o->x + o->w, o->y + o->h);
}

uint8_t check_relativity(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
  uint8_t flags;

  /*check x relativity*/
  flags = x1 >= x4 ? RELATIVE_LEFT | RELATIVE_X : 0;
  flags = x2 <= x3 ? flags | RELATIVE_RIGHT | RELATIVE_X : flags;

  /*check y relativity*/
  flags = y1 >= y4 ? flags | RELATIVE_UP | RELATIVE_Y : flags;
  flags = y2 <= y3 ? flags | RELATIVE_DOWN | RELATIVE_Y : flags;

  return flags;
}

uint8_t check_adjacencyE(Entity* e, Entity* o)
{
  return check_adjacency(e->x, e->y, e->x + e->w, e->y + e->h, o->x, o->y, o->x + o->w, o->y + o->h);
}

uint8_t check_adjacency(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
  uint8_t flags;

  /*check x adjacency*/
  flags = x1 == x4 ? ADJACENT_LEFT | ADJACENT_X : 0;
  flags = x2 == x3 ? flags | ADJACENT_RIGHT | ADJACENT_X : flags;

  /*check y adjacency*/
  flags = y1 == y4 ? flags | ADJACENT_UP | ADJACENT_Y : flags;
  flags = y2 == y3 ? flags | ADJACENT_DOWN | ADJACENT_Y : flags;

  return flags;
}

void resolve_solidCollision(WorldEntity* wE, Entity* e, Entity* o)
{
  float x1, y1, x2, y2, w, h;
  uint8_t adj;

  /*Step 1: Get Collision Box*/
  y1 = e->y > o->y ? e->y : o->y;
  y2 = e->y + e->h > o->y + o->h ? o->y + o->h : e->y + e->h;
  x1 = e->x > o->x ? e->x : o->x;
  x2 = e->x + e->w > o->x + o->w ? o->x + o->w : e->x + e->w;

  w = x2 - x1;
  h = y2 - y1;
  /*Step 2a: Adjacency Check with old positions.*/
  x1 = e->x - (e->xV * (wE->delta_t / 1000.f));
  y1 = e->y - (e->yV * (wE->delta_t / 1000.f));
  x2 = x1 + e->w;
  y2 = y1 + e->h;
  adj = check_adjacency(x1, y1, x2, y2, o->x, o->y, o->x + o->w, o->y + o->h);

  if(adj & ADJACENT_X)
    if(adj & ADJACENT_Y && ABS(e->yV) <= ABS(e->xV))
      w = 0;
    else
      h = 0;
  else if(adj & ADJACENT_Y)
    w = 0;
  else if(e->xV == 0)
    w = 0;
  else if(e->yV == 0)
    h = 0;
  else
  {
    /*Step 2b: Relativity Check with old positions.*/
    uint8_t rel = check_relativity(x1, y1, x2, y2, o->x, o->y, o->x + o->w, o->y + o->h);
    float s = e->yV / e->xV;
    if(rel & RELATIVE_X)
      if(rel & RELATIVE_Y && h <= w)
        w = -s * h;
      else
      {
        h = -s * w;
      }
    else if(rel & RELATIVE_Y)
      w = -s * h;
  }

  /*Step 3: FINISHING touches*/
  e->x += e->xV < 0 ? w : -w;
  e->y += e->yV < 0 ? h : -h;
}
/**Helper Functions END**/
