#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <utility>

#include "Block.h"

struct StructureType {
    int ID = 0;
    std::string name;
    std::vector<std::pair<Block, glm::vec3>> blocks;
    std::pair<int, int> xBounds, yBounds, zBounds;

    StructureType(int ID, std::string name, std::vector<std::pair<Block, glm::vec3>> blocks, 
        std::pair<int, int> xBounds, std::pair<int, int> yBounds, std::pair<int, int> zBounds);
    StructureType();
};

#endif