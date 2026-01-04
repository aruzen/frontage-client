#version 450
#extension GL_EXT_debug_printf : enable

layout(set = 1, binding = 0) uniform sampler2DArray samplers;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in int fragSamplerIndex;
layout(location = 3) flat in int instanceId;

layout(location = 0) out vec4 outColor;
layout(location = 1) out uint outId;

vec4 blend(vec4 main, vec4 sub) {
    return main + vec4(sub.rgb * min((sub.a + main.a)/4, 1.0), 0.0);
}

void main() {
    outColor = blend(texture(samplers, vec3(fragTexCoord, fragSamplerIndex)), fragColor);
    // outColor = texture(samplers, vec3(fragTexCoord, fragSamplerIndex));
    // outColor = vec4(1.0, 0.0, 0.0, 1.0);
    // outColor = fragColor;
    outId = instanceId;
    // debugPrintfEXT("ID : %d \n", outId);
}