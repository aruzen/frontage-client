#version 450

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

void main() {
    vec2 pos = (gl_VertexIndex == 0 ? vec2(pushData.p0 * 2):
                gl_VertexIndex == 1 ? vec2(pushData.p1 * 2):
                gl_VertexIndex == 2 ? vec2(pushData.p2 * 2):
                                      vec2(pushData.p3 * 2));
    pos.x /= pushData.window.x;
    pos.y /= pushData.window.y;
    gl_Position = vec4(pos - vec2(1.0, 1.0), 0.0, 1.0);
    fragTexCoord = (gl_VertexIndex == 0 ? pushData.uv0:
                    gl_VertexIndex == 1 ? pushData.uv1:
                    gl_VertexIndex == 2 ? pushData.uv2:
                                          pushData.uv3);
}
