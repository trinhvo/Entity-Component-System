#ifndef gameentity_h_
#define gameentity_h_

typedef struct gameEntityT gameEntityT;

#include "base/array.h"
#include "base/common.h"
#include "engine/component.h"
#include "engine/game.h"

struct gameEntityT {
    gameT* game;
    arrayT* components;
    void* data;
};


gameEntityT* newEntity(void);
void freeEntity(gameEntityT* entity);

void attachComponent(gameEntityT* entity, gameComponentT* component);
gameComponentT* getComponent(gameEntityT* entity, const string* subsystem_name);

#endif // gameentity_h_
