#ifndef FACE_H
#define FACE_H

#include <glm/glm.hpp>

#include <utility>
#include <string>

#include "VkUtils.h"

enum Orientation {POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z, NUMBER_OF_ORIENTATIONS};

//enum BlockID {AIR, DIRT, GRASS, STONE, SAND, WOOD, SLAB, GRAVEL, COBBLE, PLANKS, ICE, NUMBER_OF_BLOCK_IDS};

struct BlockType {
    int ID;
    std::string name;
    std::pair<float, float> textureCoordinates[NUMBER_OF_ORIENTATIONS];

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