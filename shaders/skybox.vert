#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inNormal;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    vec4 cameraPos;
    vec4 ambientLightColor;
} ubo;

layout(location = 0) out vec3 outUVW;

void main() {
    outUVW = inPosition;
    
    mat4 viewRot = mat4(mat3(ubo.view)); 
    
    vec4 clipPos = ubo.projection * viewRot * vec4(inPosition, 1.0);
    
    gl_Position = clipPos.xyww;
}
