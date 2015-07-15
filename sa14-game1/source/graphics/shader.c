/*------------------------------------------------
 * INCLUDES
 *----------------------------------------------*/

#include "shader.h"

#include "base/array.h"
#include "base/common.h"
#include "base/debug.h"
#include "graphics/graphics.h"
#include "graphics/texture.h"
#include "graphics/trimesh.h"

#include <stdlib.h>

#include <GL/glew.h>

/*------------------------------------------------
 * TYPES
 *----------------------------------------------*/

/*--------------------------------------
 * Type: shaderT
 *
 * Description:
 *   Concrete implementation for the shader program data type. The fields are
 *   mainly related to deep system stuff and so should not be exposed publicly.
 *------------------------------------*/
struct shaderT {
    GLuint  id;      // The shader program identifier, given by OpenGL.
    arrayT* shaders; // The attached shaders. Used to detach and delete them.
};

/*------------------------------------------------
 * GLOBALS
 *----------------------------------------------*/

shaderT* active_shader = NULL;

/*------------------------------------------------
 * FUNCTIONS
 *----------------------------------------------*/

static void compileShader(GLenum type, shaderT* shader, const string* source) {
    assert(source != NULL);

    GLuint shader_id = glCreateShader(type);

    glShaderSource (shader_id, 1, &source, NULL);
    glCompileShader(shader_id);

    GLint result;

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint info_log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar *log = malloc(sizeof(GLchar) * info_log_length);
        glGetShaderInfoLog(shader_id, info_log_length, NULL, log);
        printf("\n%s", log);
        free(log);
        error("Shader failed to compile");
    }

    glAttachShader(shader->id, shader_id);
    glLinkProgram (shader->id);

    glGetProgramiv(shader->id, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        GLint info_log_length;
        glGetProgramiv(shader->id, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar *log = malloc(sizeof(GLchar) * info_log_length);
        glGetProgramInfoLog(shader->id, info_log_length, NULL, log);
        printf("\n%s", log);
        free(log);
        error("Shader program failed to link");
    }

    arrayAdd(shader->shaders, &shader_id);
}

shaderT* createShader(void) {
    shaderT* shader = malloc(sizeof(shaderT));

    shader->id      = glCreateProgram();
    shader->shaders = arrayNew(sizeof(GLuint));

    return (shader);
}

void deleteShader(shaderT* shader) {
    for (int i = 0; i < arrayLength(shader->shaders); i++) {
        GLuint shader_id = *(GLuint*)arrayGet(shader->shaders, i);
        glDeleteShader(shader_id);
    }

    useShader(NULL);

    glDeleteProgram(shader->id);

    arrayFree(shader->shaders);
    free(shader);
}

shaderT* useShader(const shaderT* shader) {
    shaderT* old_shader = active_shader;

    if (shader != active_shader) {
        glUseProgram(shader ? shader->id : 0);
        active_shader = shader;
    }

    return (old_shader);
}

void compileFragmentShader(shaderT* shader, const string* source) {
    compileShader(GL_FRAGMENT_SHADER, shader, source);
}

void compileGeometryShader(shaderT* shader, const string* source) {
    compileShader(GL_GEOMETRY_SHADER, shader, source);
}

void compileVertexShader(shaderT* shader, const string* source) {
    compileShader(GL_VERTEX_SHADER, shader, source);
}

void setShaderParam(const string* name, const void* value) {
    if (!active_shader)
        error("No shader in use");

    GLuint index = (-1);
    glGetUniformIndices(active_shader->id, 1, &name, &index);

    assert((int)index >= 0);

    GLint type = 0;
    glGetActiveUniformsiv(active_shader->id, 1, &index, GL_UNIFORM_TYPE, &type);

    GLint loc = glGetUniformLocation(active_shader->id, name);
    switch (type) {
    case GL_INT          : glUniform1i       (loc,  *(GLint  *)value); break;
    case GL_UNSIGNED_INT : glUniform1ui      (loc,  *(GLuint *)value); break;
    case GL_FLOAT        : glUniform1f       (loc,  *(GLfloat*)value); break;
    case GL_FLOAT_VEC2   : glUniform2fv      (loc, 1,          value); break;
    case GL_FLOAT_VEC3   : glUniform3fv      (loc, 1,          value); break;
    case GL_FLOAT_VEC4   : glUniform4fv      (loc, 1,          value); break;
    case GL_FLOAT_MAT2   : glUniformMatrix2fv(loc, 1, GL_TRUE, value); break;
    case GL_FLOAT_MAT3   : glUniformMatrix3fv(loc, 1, GL_TRUE, value); break;
    case GL_FLOAT_MAT4   : glUniformMatrix4fv(loc, 1, GL_TRUE, value); break;
    default              : error("Unknown uniform type specified");
    }
}

void shaderPostProcess(void) {
    if (!active_shader)
        error("No shader in use");

    textureT* texture = createTextureFromScreen();
    triMeshT* quad    = createQuad(2.0f, 2.0f);

    clearDisplay(1.0f, 0.0f, 1.0f);

    useTexture(texture, 0);

    drawMesh(quad);

    freeMesh(quad);
    useTexture(NULL, 0);
    freeTexture(texture);
}
