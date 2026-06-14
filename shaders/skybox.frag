#version 450

layout(location = 0) in vec3 inUVW;

layout(set = 1, binding = 0) uniform sampler2D hdriMap;

layout(location = 0) out vec4 outColor;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec3 n = normalize(v);
    
    vec2 uv = vec2(atan(n.z, n.x), asin(n.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 sphericalUV = SampleSphericalMap(inUVW);
    
    vec3 envColor = textureLod(hdriMap, sphericalUV, 0.0).rgb;
    
    envColor = envColor / (envColor + vec3(1.0));
    
    outColor = vec4(envColor, 1.0);
}
