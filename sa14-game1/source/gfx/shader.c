/*------------------------------------------------
 * INCLUDES
 *----------------------------------------------*/

#include "shader.h"

#include "core/array.h"
#include "core/common.h"
#include "core/debug.h"

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
    arrayT *shaders; // The attached shaders. Used to detach and delete them.
};

/*------------------------------------------------
 * GLOBALS
 *----------------------------------------------*/

shaderT *active_shader = NULL;

/*------------------------------------------------
 * FUNCTIONS
 *----------------------------------------------*/

static void compileShader(GLenum type, shaderT *prog, string const *source) {
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

    glAttachShader(prog->id, shader_id);
    glLinkProgram (prog->id);

    glGetProgramiv(prog->id, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        GLint info_log_length;
        glGetProgramiv(prog->id, GL_INFO_LOG_LENGTH, &info_log_length);
        GLchar *log = malloc(sizeof(GLchar) * info_log_length);
        glGetProgramInfoLog(prog->id, info_log_length, NULL, log);
        printf("\n%s", log);
        free(log);
        error("Shader program failed to link");
    }

    arrayAdd(prog->shaders, &shader_id);
}

shaderT *createShader(void) {
    shaderT *shader = malloc(sizeof(shaderT));

    shader->id      = glCreateProgram();
    shader->shaders = newArray(sizeof(GLuint));

    return (shader);
}

void deleteShader(shaderT *shader) {
    for (int i = 0; i < arrayLength(shader->shaders); i++) {
        GLuint shader_id = *(GLuint *)arrayGet(shader->shaders, i);
        glDeleteShader(shader_id);
    }

    useShader(NULL);

    glDeleteProgram(shader->id);

    freeArray(shader->shaders);
    free(shader);
}

shaderT *useShader(shaderT const *shader) {
    shaderT *old_shader = active_shader;

    glUseProgram(shader ? shader->id : 0);
    active_shader = shader;

    return (old_shader);
}

void compileFragmentShader(shaderT *shader, string const *source) {
    compileShader(GL_FRAGMENT_SHADER, shader, source);
}

void compileGeometryShader(shaderT *shader, string const *source) {
    compileShader(GL_GEOMETRY_SHADER, shader, source);
}

void compileVertexShader(shaderT *shader, string const *source) {
    compileShader(GL_VERTEX_SHADER, shader, source);
}

void setShaderParam(string const *name, void const *value) {
    GLuint index;
    glGetUniformIndices(active_shader->id, 1, &name, &index);

    GLint type = 0;
    glGetActiveUniformsiv(active_shader->id, 1, &index, GL_UNIFORM_TYPE, &type);

    GLint loc = glGetUniformLocation(active_shader->id, name);
    switch (type) {
    case GL_INT:
        glUniform1i(loc, *(GLint *)value);
        break;
    case GL_FLOAT:
        glUniform1f(loc, *(GLfloat *)value);
        break;
    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(loc, 1, GL_TRUE, (GLfloat *)value);
        break;
    default:
        error("Unknown uniform type specified");
    }
}
