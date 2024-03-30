#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <cstring>
#include <map>

#include "PerlinNoise.hpp"
#include "Block.h"

using namespace VkUtils;

enum GenerationMethod {
    NORMAL_GENERATION,
};

enum NoiseMethod {
    TERRAIN_NOISE,
    CAVE_NOISE
};

const uint CHUNK_SIZE = 64;

struct WorldGenerationInfo {
    uint32_t seed;
    float terrainFrequency;
    float snowLevel;
    float treeFrequency;
    float minTreeNoiseValue;
    float seaLevel;
    float seaLevelThreshold;

    WorldGenerationInfo(uint32_t seed, float terrainFrequency, float snowLevel, float treeFrequency, float minTreeNoiseValue, float seaLevel, float seaLevelThreshold) {
        this->seed = seed;
        this->terrainFrequency = terrainFrequency;
        this->snowLevel = snowLevel;
        this->treeFrequency = treeFrequency;
        this->minTreeNoiseValue = minTreeNoiseValue;
        this->seaLevel = seaLevel;
        this->seaLevelThreshold = seaLevelThreshold;
    }
};

struct ChunkGeometry {
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    uint32_t maxVertexCount = 0;

    VkDevice device = VK_NULL_HANDLE;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    void *vertexData;
    void *indexData;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    ~ChunkGeometry();

    void uploadVertices();
    void uploadIndices();

    std::string toString();
};

struct Chunk {
    int x;
    int y;
    int z;

    bool blocksChanged = true;

    Block blockGrid[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

    ChunkGeometry geometry;
    std::vector<Face> faces[NUMBER_OF_ORIENTATIONS];

    Chunk(int x, int y, int z);

    Block& getBlock(int x, int y, int z);
    void setBlock(int x, int y, int z, Block &block); 
    void setBlock(int x, int y, int z, int ID);

    float getNoiseValue(int x, int y, int z, WorldGenerationInfo &worldGenerationInfo, const siv::PerlinNoise &terrainNoise, const int WORLD_Z_SIZE, NoiseMethod method);

    void generateBlocks(WorldGenerationInfo &worldGenerationInfo, const siv::PerlinNoise &terrainNoise, const int WORLD_Z_SIZE, GenerationMethod method);
    void generateMesh(Orientation orientation, Chunk *borderingChunk, std::map<int, BlockType> &blockTypes);

    void uploadFace(Face &face, std::map<int, BlockType> &blockTypes);
    void uploadFaces(Orientation orientation, std::map<int, BlockType> &blockTypes);
};

#endif