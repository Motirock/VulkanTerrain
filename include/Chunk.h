#ifndef CHUNK_H
#define CHUNK_H

#include <vector>

#include "PerlinNoise.hpp"
#include "Block.h"
#include <map>

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

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t maxVertexCount = 0;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    void *vertexData;
    void *indexData;

    Block blockGrid[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

    std::vector<Face> positiveXFaces = std::vector<Face>();
    std::vector<Face> negativeXFaces = std::vector<Face>();
    std::vector<Face> positiveYFaces = std::vector<Face>();
    std::vector<Face> negativeYFaces = std::vector<Face>();
    std::vector<Face> positiveZFaces = std::vector<Face>();
    std::vector<Face> negativeZFaces = std::vector<Face>();

    Chunk(int x, int y, int z);

    Block& getBlock(int x, int y, int z);
    void setBlock(int x, int y, int z, Block &block); 

    float getElevation(int x, int y, int z, const siv::PerlinNoise &terrainNoise, const int WORLD_Z_SIZE);

    void generateBlocks( const siv::PerlinNoise &terrainNoise, const int WORLD_Z_SIZE);
    void generateMesh(Orientation orientation, Chunk *borderingChunk);

    void uploadFace(Face &face, std::map<int, BlockType> &blockTypes);
    void uploadFaces(Orientation orientation, std::map<int, BlockType> &blockTypes);
};

#endif