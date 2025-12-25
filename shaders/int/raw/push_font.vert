#version 450

layout(push_constant) uniform Push {
    vec4 textColor;
    vec4 backColor;
    ivec2 window;
    ivec2 center;
    ivec2 size;
    ivec2 image;
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
    vec2 h = vec2(pushData.image) * 0.5;
    vec2 c    = vec2(pushData.center);
    vec2 win  = vec2(pushData.window);

    vec2 pixelPos;
    if (gl_VertexIndex == 0) {
        pixelPos = c + vec2( h.x, -h.y);
    } else if (gl_VertexIndex == 1) {
        pixelPos = c + vec2(-h.x, -h.y);
    } else if (gl_VertexIndex == 2) {
        pixelPos = c + vec2(-h.x,  h.y);
    } else {
        pixelPos = c + vec2( h.x,  h.y);
    }

    vec2 ndc;
    ndc.x =  (pixelPos.x / win.x) * 2.0 - 1.0;
    ndc.y = -(pixelPos.y / win.y) * 2.0 + 1.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
    fragColor = pushData.textColor;
    backColor = pushData.backColor;
    fragTexCoord = TEXCOORD[gl_VertexIndex];
}