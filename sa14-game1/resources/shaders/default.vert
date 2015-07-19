#version 430

struct fragDataT {
    vec3 pos;
    vec3 normal;
    vec2 tex_coord;
};

uniform mat4 ModelTransform;
uniform mat4 NormalTransform;
uniform mat4 View;
uniform mat4 Proj;

layout(location = 0) in vec3 VertPos;
layout(location = 1) in vec3 VertNormal;
layout(location = 2) in vec2 TexCoord;

out fragDataT frag;

void main() {
    mat4 pv  = Proj * View;
    mat4 pvm = pv * ModelTransform;
    mat4 pvn = pv * NormalTransform;

    gl_Position = pvm * vec4(VertPos, 1.0);

    frag.pos       = (ModelTransform * vec4(VertPos, 1.0)).xyz;
    frag.normal    = (NormalTransform * vec4(VertNormal, 0.0)).xyz;
    frag.tex_coord = TexCoord;
}
