#version 450

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inFragPos; 
layout(location = 4) in vec4 inTangent; 

layout(location = 0) out vec4 outColor;

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

layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;

layout(push_constant) uniform Push {
    mat4 modelMatrix;
    vec4 albedoTint;
    float metallicFactor;
    float roughnessFactor;
} push;

const float PI = 3.14159265359;

void main() {
    vec4 albedoSamp = texture(albedoMap, inUV);
    vec3 albedo = albedoSamp.rgb * push.albedoTint.rgb;
    
    vec3 N = normalize(inNormal); 
	vec3 T = normalize(inTangent.xyz);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T) * inTangent.w;

	mat3 TBN = mat3(T, B, N);

	vec3 normalMapSample = texture(normalMap, inUV).rgb;
    normalMapSample = normalMapSample * 2.0 - 1.0;

	vec3 mappedNormal = normalize(TBN * normalMapSample);


    vec3 V = normalize(ubo.cameraPos.xyz - inFragPos);

    vec3 Lo = vec3(0.0); 

    for(int i = 0; i < ubo.numLights; i++) {
        vec3 L = normalize(ubo.lights[i].position.xyz - inFragPos);
        vec3 H = normalize(V + L); 
        
        float distance = length(ubo.lights[i].position.xyz - inFragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = ubo.lights[i].color.xyz * ubo.lights[i].color.w * attenuation;

        float NdotL = max(dot(mappedNormal, L), 0.0);
        
        vec3 diffuse = albedo / PI;
        
        Lo += (diffuse * radiance * NdotL); 
    }

    vec3 ambient = ubo.ambientLightColor.xyz * albedo * ubo.ambientLightColor.w;

    vec3 color = ambient + Lo; 

    outColor = vec4(color, push.albedoTint.a);
}

