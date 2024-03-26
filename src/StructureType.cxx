#include "StructureType.h"

StructureType::StructureType(int ID, std::string name, std::vector<std::pair<Block, glm::vec3>> blocks, 
    std::pair<int, int> xBounds, std::pair<int, int> yBounds, std::pair<int, int> zBounds)
    : ID(ID), name(name), blocks(blocks), xBounds(xBounds), yBounds(yBounds), zBounds(zBounds) {}

StructureType::StructureType() {}