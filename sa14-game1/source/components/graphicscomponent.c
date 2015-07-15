/*------------------------------------------------
 * INCLUDES
 *----------------------------------------------*/

#include "graphicscomponent.h"

#include "base/common.h"
#include "engine/component.h"

/*------------------------------------------------
 * FUNCTIONS
 *----------------------------------------------*/

static void cleanup(gameComponentT* component, gameSubsystemT* subsystem) {
    graphicsComponentDataT* gfx = component->data;

    if (gfx->mesh) {
        freeMesh(gfx->mesh);
        gfx->mesh = NULL;
    }
}

gameComponentT* newGraphicsComponent(triMeshT* mesh) {
    gameComponentT* component = newComponent("graphics");
    graphicsComponentDataT* data = calloc(1, sizeof(graphicsComponentDataT));

    data->mesh = mesh;
    mat_identity(&data->transform);
    mat_identity(&data->normal_transform);

    component->data = data;
    component->cleanup_fn = cleanup;

    return (component);
}
