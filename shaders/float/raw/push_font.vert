#version 450

layout(push_constant) uniform Push {
    vec4 textColor;
    vec4 backColor;
    vec2 p0;
    vec2 p1;
    vec2 p2;
    vec2 p3;
} pushData;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 backColor;
layout(location = 2) out vec2 fragTexCoord;

const vec2 TEXCOORD[4] = {
    vec2(1.0f, 0.0f),
    vec2(0.0f, 0.0f),
    vec2(0.0f, 1.0f),
    vec2(1.0f, 1.0f)
};

void main() {
    vec2 pos;
    vec2 uv;
    if (gl_VertexIndex == 0) {
        pos = pushData.p0;
        uv = TEXCOORD[0];
    } else if (gl_VertexIndex == 1) {
        pos = pushData.p1;
        uv = TEXCOORD[1];
    } else if (gl_VertexIndex == 2) {
        pos = pushData.p2;
        uv = TEXCOORD[2];
    } else {
        pos = pushData.p3;
        uv = TEXCOORD[3];
    }

    gl_Position = vec4(pos, 0.0, 1.0);
    fragColor = pushData.textColor;
    backColor = pushData.backColor;
    fragTexCoord = uv;
}