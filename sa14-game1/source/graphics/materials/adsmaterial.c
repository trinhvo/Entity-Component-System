#include "adsmaterial.h"

#include "base/common.h"
#include "engine/game.h"
#include "graphics/material.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "math/vector.h"

#include <GL/glew.h>

typedef struct {
    textureT* ambient_tex;
    textureT* diffuse_tex;
    textureT* specular_tex;

    vec3 ambient_coeff;
    vec3 diffuse_coeff;
    vec3 specular_coeff;

    float shininess;
    bool  wireframe;
} adsMaterialT;

static void adsBegin(materialT* m) {
    adsMaterialT* ads = m->data;
    
    setShaderParam("AmbientCoeff" , &ads->ambient_coeff  );
    setShaderParam("DiffuseCoeff" , &ads->diffuse_coeff  );
    setShaderParam("SpecularCoeff", &ads->specular_coeff );
    setShaderParam("Shininess"    , &ads->shininess      );

    useTexture(ads->ambient_tex  ? ads->ambient_tex  : whiteTexture(), 0);
    useTexture(ads->diffuse_tex  ? ads->diffuse_tex  : whiteTexture(), 1);
    useTexture(ads->specular_tex ? ads->specular_tex : whiteTexture(), 2);

    if (ads->wireframe) {
        glPushAttrib (GL_ENABLE_BIT | GL_POLYGON_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable    (GL_CULL_FACE);
    }
}

static void adsEnd(materialT* m) {
    adsMaterialT* ads = m->data;

    if (ads->wireframe)
        glPopAttrib();
}

materialT* createADSMaterial(textureT* ambient_tex,
                             textureT* diffuse_tex,
                             textureT* specular_tex,
                             vec3      ambient_coeff,
                             vec3      diffuse_coeff,
                             vec3      specular_coeff,
                             float     shininess,
                             bool      wireframe)
{
    materialT* m = createCustomADSMaterial(ambient_tex, diffuse_tex,
                                           specular_tex, ambient_coeff,
                                           diffuse_coeff, specular_coeff,
                                           shininess, wireframe, NULL, NULL);

    return (m);
}

materialT* createCustomADSMaterial(textureT* ambient_tex,
                                   textureT* diffuse_tex,
                                   textureT* specular_tex,
                                   vec3      ambient_coeff,
                                   vec3      diffuse_coeff,
                                   vec3      specular_coeff,
                                   float     shininess,
                                   bool      wireframe,
                                   string*   vert_src,
                                   string*   frag_src)
{
    shaderT* ads_shader;

    if (!vert_src && !frag_src) {
        ads_shader = gameResource("shader:adsmaterial", ResShader);
    }
    else {
        if (!vert_src)
            vert_src = gameResource("shaders/default.vert", ResString);

        if (!frag_src)
            frag_src = gameResource("shaders/materials/adsmaterial.frag", ResString);

        ads_shader = createShader();
        compileVertexShader  (ads_shader, vert_src);
        compileFragmentShader(ads_shader, frag_src);
    }

    materialT*    m   = newMaterial();
    adsMaterialT* ads = malloc(sizeof(adsMaterialT));

    m->type       = "ads";
    m->data       = ads;
    m->sort_value = 1000;
    m->shader     = ads_shader;
    m->begin_fn   = adsBegin;
    m->end_fn     = adsEnd;
    
    ads->ambient_tex    = ambient_tex;
    ads->diffuse_tex    = diffuse_tex;
    ads->specular_tex   = specular_tex;
    ads->ambient_coeff  = ambient_coeff;
    ads->diffuse_coeff  = diffuse_coeff;
    ads->specular_coeff = specular_coeff;
    ads->shininess      = shininess;
    ads->wireframe      = wireframe;

    return (m);
}
