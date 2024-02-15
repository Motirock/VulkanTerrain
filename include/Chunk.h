#ifndef CHUNK_H
#define CHUNK_H

#include <vector>

#include "PerlinNoise.hpp"
#include "Block.h"

using namespace VkUtils;

const uint CHUNK_SIZE = 16;

struct WorldGenerationInfo {
    uint32_t seed;
    float terrainFrequency;
    float snowHeight;
    float treeFrequency;
    float minTreeNoiseValue;
    float seaLevel;

    WorldGenerationInfo(uint32_t seed, float terrainFrequency, float snowHeight, float treeFrequency, float minTreeNoiseValue, float seaLevel) {
        this->seed = seed;
        this->terrainFrequency = terrainFrequency;
        this->snowHeight = snowHeight;
        this->treeFrequency = treeFrequency;
        this->minTreeNoiseValue = minTreeNoiseValue;
        this->seaLevel = seaLevel;
    }
};

struct Chunk {
    int x;
    int y;
    int z;

    bool blocksChanged = true;

    /*std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t maxVertexCount = 0;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    void *vertexData;
    void *indexData;*/

    Block blockGrid[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    std::vector<Face> positiveXFaces = std::vector<Face>();
    std::vector<Face> negativeXFaces = std::vector<Face>();
    std::vector<Face> positiveYFaces = std::vector<Face>();
    std::vector<Face> negativeYFaces = std::vector<Face>();
    std::vector<Face> positiveZFaces = std::vector<Face>();
    std::vector<Face> negativeZFaces = std::vector<Face>();

    void generateBlocks( const siv::PerlinNoise &terrainNoise, const int WORLD_Z_SIZE);

    Chunk(int x, int y, int z);
    void generateMesh(Orientation orientation, Chunk *borderingChunk);
};

#endif