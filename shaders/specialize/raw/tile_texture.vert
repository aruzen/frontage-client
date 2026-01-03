#version 450

layout(location = 0) in vec2 inPosition;
// | 状態 4bit | テクスチャインデックス 8bit | 空き(拡張領域) 4bit | ID 12bit |
layout(location = 1) in int instanceState;
layout(location = 2) in mat4 model;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) flat out int fragSamplerIndex;
layout(location = 3) flat out int instanceId;

vec4 ChoseColor(int instanceState) {
    switch (instanceState >> (32 - 4)) {
    case 1: // mouse over
        return vec4(1.0, 0.0, 0.0, 1.0);
    default:
        return vec4(0.0, 0.0, 0.0, 0.0);
    }
}

int UnpackSamplerIndex(int instanceState) {
    return (instanceState >> 20) & 0xFF;
}

int UnpackID(int instanceState) {
    return instanceState & 0xFFF;
}

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

const vec2 TEXCOORD[4] = {
    vec2(0.0f, 0.0f),
    vec2(0.0f, 1.0f),
    vec2(1.0f, 1.0f),
    vec2(1.0f, 0.0f)
};

void main() {
    gl_Position = ubo.proj * ubo.view * model * vec4(inPosition, 0.0, 1.0);
    // gl_Position = model * vec4(inPosition, 0.0, 1.0);
    // gl_Position = vec4(inPosition, 0.0, 1.0);
    // gl_Position = vec4(TEXCOORD[gl_VertexIndex % 4], 0.0, 1.0);
    fragTexCoord = TEXCOORD[gl_VertexIndex % 4];
    fragColor = ChoseColor(instanceState);
    // fragColor = vec4(TEXCOORD[gl_VertexIndex % 4], 0.0, 1.0);
    fragSamplerIndex = UnpackSamplerIndex(instanceState);
    instanceId = UnpackID(instanceState);
}