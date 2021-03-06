#ifndef game_h_
#define game_h_

/*------------------------------------------------
 * INCLUDES
 *----------------------------------------------*/

typedef struct gameT gameT;

#include "base/common.h"
#include "base/pak.h"
#include "engine/component.h"
#include "engine/entity.h"
#include "engine/subsystem.h"
#include "input/keyboard.h"
#include "input/mouse.h"

/*------------------------------------------------
 * FUNCTIONS
 *----------------------------------------------*/

void initGame(const string* title, int screen_width, int screen_height);
void exitGame(void);

void gameMain(void(*frame_func)(float dt));

void addSubsystemToGame(gameSubsystemT* subsystem);
void addEntityToGame(gameEntityT* entity);

void gameAddResource(const string* name, void* data, int type);
const void* gameResource(const string* name, int type);

#endif // game_h_
