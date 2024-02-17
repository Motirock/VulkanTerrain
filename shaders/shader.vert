#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 cameraPosition;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 fragPosition;
layout(location = 4) out float distanceFromCamera;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    normal = inNormal;
    fragPosition = vec3(ubo.model * vec4(inPosition, 1.0));
    distanceFromCamera = length(inPosition - ubo.cameraPosition);
}