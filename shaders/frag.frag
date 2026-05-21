#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;
layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main() {
   // outColor = texture(texSampler, fragUV) * vec4(fragColor, 1.0);
	outColor = vec4(fragNormal,1.0) * 0.5f + 0.5f;
}

