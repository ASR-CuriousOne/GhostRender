#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec4 inTangent;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outFragPos; 
layout(location = 4) out vec4 outTangent;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    vec4 ambientLightColor;
    vec4 cameraPos;
    int numLights;
    PointLight lights[50];
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    vec4 albedoTint;
    float metallicFactor;
    float roughnessFactor;
} push;

void main() {
    vec4 worldPos = push.modelMatrix * vec4(inPosition, 1.0);
    outFragPos = worldPos.xyz;

    outColor = inColor;
    outUV = inUV;

    mat3 normalMatrix = transpose(inverse(mat3(push.modelMatrix)));
    outNormal = normalize(normalMatrix * inNormal);
    vec3 T = normalize(normalMatrix * inTangent.xyz);
	outTangent = vec4(T, inTangent.w);

    gl_Position = ubo.projection * ubo.view * worldPos;
}
