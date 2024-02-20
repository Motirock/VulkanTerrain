#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in int inNormalAndColor;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 fragPosition;
layout(location = 3) out float distanceFromCamera;
layout(location = 4) out vec3 fragColor;

vec3 normals[] = {vec3(1.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f), 
    vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f),
    vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f)};

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
    fragTexCoord = inTexCoord;
    normal = normals[(inNormalAndColor >> 24)];
    fragColor = vec3((inNormalAndColor >> 16) & 0x00FF, (inNormalAndColor >> 8) & 0x0000FF, inNormalAndColor & 0x000000FF)/255.0f;
    fragPosition = vec3(ubo.model * vec4(inPosition, 1.0));
    distanceFromCamera = length(inPosition - ubo.cameraPosition);
}