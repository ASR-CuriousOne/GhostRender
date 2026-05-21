#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in vec3 fragPosWorld;
layout(location = 3) in vec3 fragNormalWorld;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

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

void main() {
   vec3 surfaceNormal = normalize(fragNormalWorld);
   vec3 viewDirection = normalize(ubo.cameraPos.xyz - fragPosWorld);

   vec3 finalLighting = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;

   for(int i = 0; i < ubo.numLights; i++) {
        PointLight light = ubo.lights[i];
        
        vec3 lightDirection = light.position.xyz - fragPosWorld;
        float distance = length(lightDirection);
        lightDirection = normalize(lightDirection); 

        float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.005 * distance * distance);

        float cosAngleIncidence = max(dot(surfaceNormal, lightDirection), 0.0);
        vec3 diffuse = light.color.xyz * light.color.w * cosAngleIncidence;

        vec3 halfAngle = normalize(lightDirection + viewDirection);
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm, 0.0, 1.0);
        blinnTerm = pow(blinnTerm, 32.0); 
        vec3 specular = light.color.xyz * light.color.w * blinnTerm;

        finalLighting += (diffuse + specular) * attenuation;
    }

    vec4 textureColor = texture(texSampler, fragUV);
    
    outColor = vec4(finalLighting * textureColor.rgb, textureColor.a);
}

