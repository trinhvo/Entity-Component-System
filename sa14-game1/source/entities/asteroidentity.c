#include "asteroidentity.h"

#include "base/common.h"
#include "components/graphicscomponent.h"
#include "components/physicscomponent.h"
#include "engine/entity.h"
#include "graphics/material.h"
#include "graphics/trimesh.h"
#include "graphics/io/3ds.h"

#include <stdlib.h>

static vec3 randomVector(void) {
    vec3 v;

    v.x = rand()/(float)RAND_MAX - 0.5f;
    v.y = rand()/(float)RAND_MAX - 0.5f;
    v.z = rand()/(float)RAND_MAX - 0.5f;

    vec_normalize(&v, &v);

    return (v);
}

static gameComponentT* createGraphicsComponent(void) {
    static triMeshT* mesh = NULL;
    static materialT* mat = NULL;

    if (!mesh) {
        //mesh = createGeodesicSphere(0.3f, 0);
        //mesh = createBox(0.3f, 0.3f, 0.3f);
        
        //a3dsDataT* a3ds = a3dsLoad(readGamePakFile("meshes/player.3ds"));
        //getchar();

        const a3dsDataT* a3ds = gameResource      ("mesh:doughnut", ResMesh);
                         mesh = a3dsCreateMesh    (a3ds, "doughnut");
                         mat  = a3dsCreateMaterial(a3ds, "doughnut_materia");

        //a3dsFree(a3ds);
        //mesh = load3DS();
        //calcSmoothNormals(mesh);
        //updateMesh(mesh);
    }

    if (!mat) {

        mat = getNamedMaterial("shiny black");
        if ((rand() % 4) == 0) {
            mat = getNamedMaterial("blue crystal");
        }
    }

    gameComponentT* component = newGraphicsComponent(mesh, mat);

    return (component);
}

static gameComponentT* createPhysicsComponent(void) {
    gameComponentT* component = newPhysicsComponent(1.0f * Kilogram);
    
    physicsComponentDataT* phys = component->data;
    
    vec2 pos;
    vec2 vel;
    
    switch (rand() % 4) {
        
    case 0: {
        // Spawn above screen.
        pos = (vec2) { -0.5f + 1.0f * rand()/(float)RAND_MAX, 0.6f };
        vel = (vec2) { -1.0f + 2.0f * rand()/(float)RAND_MAX,
                       -0.2f - 1.0f * rand()/(float)RAND_MAX };
        break;
    };

    case 1: {
        // Spawn to the right of screen.
        pos = (vec2) {  1.0f, -0.5f + 1.0f * rand()/(float)RAND_MAX };
        vel = (vec2) { -0.2f - 1.0f * rand() / (float)RAND_MAX,
                       -1.0f + 2.0f * rand() / (float)RAND_MAX };
        break;
    };

    case 2: {
        // Spawn below screen.
        pos = (vec2) { -0.5f + 1.0f * rand()/(float)RAND_MAX, -0.6f };
        vel = (vec2) { -1.0f + 2.0f * rand()/(float)RAND_MAX,
                        0.2f + 1.0f * rand()/(float)RAND_MAX };
        break;
    };

    case 3: {
        // Spawn to the left of screen.
        pos = (vec2) { -1.0f, -0.5f + 1.0f * rand()/(float)RAND_MAX };
        vel = (vec2) {  0.2f + 1.0f * rand() / (float)RAND_MAX,
                       -1.0f + 2.0f * rand() / (float)RAND_MAX };
        break;
    };

    }

    vec_scale(&pos, 6.0f, &pos);
    //bodySetOrientation(((physicsComponentDataT*)phys)->body, 17 * 3.141592f / 180.0f);
    //pos.x = -1.0f;
    //pos.y = 0.0f;
    //vel.x = -1.0f;
    //vel.y = 0.0f;
    bodySetPosition(((physicsComponentDataT*)phys)->body, pos);

    //vec_scale(&vel, 0.1f, &vel);
    bodySetVelocity(((physicsComponentDataT*)phys)->body, vel);
    
    return (component);
}

static void rotateAsteroid(gameComponentT* component, float dt) {
    asteroidEntityDataT*    asteroid = component->entity->data;
    graphicsComponentDataT* gfx      = getComponent(component->entity, "graphics")->data;
    physicsComponentDataT*  phys     = getComponent(component->entity, "physics" )->data;
    
    float o = bodyOrientation(phys->body);
    //vec2  x = bodyPosition   (phys->body);
    
    mat4x4 m, r;

    mat_identity (&m);
    mat_rot_z    (o, &r);
    mat_mul      (&r, &m, &m);

    asteroid->angle1 += dt * asteroid->a1;
    mat4x4 lol;
    mat_rot(&asteroid->rot_axis_1, asteroid->angle1, &lol);
    mat_mul(&lol, &m, &m);

    gfx->transform = m;
}

gameEntityT* newAsteroidEntity(void) {
    gameEntityT* entity = newEntity();

    entity->data = calloc(1, sizeof(asteroidEntityDataT));

    gameComponentT* gfx  = createGraphicsComponent();
    gfx->update_fn = rotateAsteroid;
    
    gameComponentT* phys = createPhysicsComponent();

    attachComponent(entity, gfx);
    attachComponent(entity, phys);
    
    asteroidEntityDataT* asteroid = entity->data;
    
    asteroid->a1 = (rand() / (float)RAND_MAX - 0.5f) * 6.0f;
    asteroid->a2 = (rand() / (float)RAND_MAX - 0.5f) * 6.0f;
    asteroid->angle1 = 0.0f;
    asteroid->angle2 = 0.0f;
    asteroid->scale = 0.03f * (0.5f + rand() / (float)RAND_MAX);
    asteroid->rot_axis_1 = randomVector();
    asteroid->rot_axis_2 = randomVector();

    return (entity);
}
