#ifndef FACE_H
#define FACE_H

#include <glm/glm.hpp>

#include <utility>
#include <string>

#include "VkUtils.h"

using namespace VkUtils;

struct BlockType {
    int ID;
    std::string name;
    std::pair<float, float> textureCoordinates[NUMBER_OF_ORIENTATIONS];
    bool isFullOpaque = true;
    bool hideSameNeighbors = true;
    bool isReplaceable = false;

    BlockType(int ID, std::string name, std::pair<float, float> *textureCoordinates);
    BlockType();
};

struct Face {
    glm::vec3 position;
    Orientation orientation;
    int blockID;

    Face(glm::vec3 position, Orientation orientation, int blockID);
    Face(float x, float y, float z, Orientation orientation, int blockID);
};

#endif