#version 450

#extension GL_EXT_debug_printf : enable
#extension GL_EXT_spirv_intrinsics : enable

layout(push_constant) uniform Push {
    ivec2 window;
    ivec2 p0;
    ivec2 p1;
    ivec2 p2;
    ivec2 p3;
    vec2 uv0;
    vec2 uv1;
    vec2 uv2;
    vec2 uv3;
} pushData;

layout(location = 0) out vec2 fragTexCoord;

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
        pos = vec2(pushData.p0);
        uv  = vec2(pushData.uv0);
    } else if (gl_VertexIndex == 1) {
        pos = vec2(pushData.p1);
        uv  = vec2(pushData.uv1);
    } else if (gl_VertexIndex == 2) {
        pos = vec2(pushData.p2);
        uv  = vec2(pushData.uv2);
    } else {
        pos = vec2(pushData.p3);
        uv  = vec2(pushData.uv3);
    }

    vec2 win = vec2(pushData.window);

    vec2 ndc;
    ndc.x = (pos.x / win.x) * 2.0 - 1.0;
    ndc.y = -(pos.y / win.y) * 2.0 + 1.0;

    debugPrintfEXT("index : %d, pos : (%f, %f)", gl_VertexIndex, pos.x, pos.y);
    gl_Position = vec4(pos, 0.0, 1.0);
    // gl_Position = vec4(TEXCOORD[gl_VertexIndex], 0.0, 1.0);
    fragTexCoord = uv;
}
