#ifndef VK_UTILS
#define VK_UTILS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <cmath>
#include <functional>

namespace VkUtils {
    enum Orientation {POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z, NUMBER_OF_ORIENTATIONS};

    struct Vertex {
        glm::vec3 pos;
        glm::vec2 texCoord;
        uint32_t normalAndColor;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32_UINT;
            attributeDescriptions[2].offset = offsetof(Vertex, normalAndColor);

            return attributeDescriptions;
        }

        bool operator==(const Vertex& other) const {
            return pos == other.pos && texCoord == other.texCoord && normalAndColor == other.normalAndColor;
        }
    };

    inline uint32_t packNormalAndColor(Orientation orientation, int r, int g, int b) {
        uint32_t normalAndColor = orientation << 24;
        normalAndColor = normalAndColor | (r << 16);
        normalAndColor = normalAndColor | (g << 8);
        normalAndColor = normalAndColor | b;
        return normalAndColor;
    }

    inline uint32_t packNormalAndColor(Orientation orientation, float r, float g, float b) {
        uint32_t normalAndColor = orientation << 24;
        normalAndColor = normalAndColor | ((int) (r*255) << 16);
        normalAndColor = normalAndColor | ((int) (g*255) << 8);
        normalAndColor = normalAndColor | (int) (b*255);
        return normalAndColor;
    }
}

#endif