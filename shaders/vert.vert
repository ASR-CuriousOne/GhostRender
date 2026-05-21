#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out vec3 fragPosWorld;
layout(location = 3) out vec3 fragNormalWorld;

struct PointLight {
    vec4 position; 
    vec4 color;  
};

const int MAX_LIGHTS = 50;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    vec4 ambientLightColor;
    vec4 cameraPos; 
    int numLights;
    PointLight lights[MAX_LIGHTS];
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
    mat4 normalMatrix;
} push;

void main() {
    vec4 positionWorld = push.model * vec4(position, 1.0);
    gl_Position = ubo.projection * ubo.view * positionWorld;

    fragColor = color;
	fragUV = uv;
	fragPosWorld = positionWorld.xyz;

	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
}
