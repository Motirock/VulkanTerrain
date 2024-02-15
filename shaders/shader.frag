#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 fragPosition;

layout(location = 0) out vec4 outColor;

vec3 lightPosition = vec3(16.0, 16.0, 100000.0); 
vec3 lightColor = vec3(1.0, 1.0, 1.0);
float ambient = 0.1f;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPosition - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec4 sampledColor = texture(texSampler, fragTexCoord);

    vec3 result = (ambient + diffuse) * fragColor * sampledColor.rgb;
    
    outColor = vec4(result, sampledColor.a);
}