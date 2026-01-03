#version 450

layout(set = 1, binding = 0) uniform sampler2DArray samplers;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in int fragSamplerIndex;
layout(location = 3) flat in int instanceId;

layout(location = 0) out vec4 outColor;
layout(location = 1) out uint outId;

vec4 over(vec4 src, vec4 dst) {
    float a = src.a + dst.a * (1.0 - src.a);
    vec3 rgb = (src.rgb * src.a + dst.rgb * dst.a * (1.0 - src.a)) / max(a, 1e-6);
    return vec4(rgb, a);
}

void main() {
    // outColor = over(texture(samplers, vec3(fragTexCoord, fragSamplerIndex)), fragColor);
    outColor = texture(samplers, vec3(fragTexCoord, fragSamplerIndex));
    // outColor = vec4(1.0, 0.0, 0.0, 1.0);
    // outColor = fragColor;
    outId = 10;
}