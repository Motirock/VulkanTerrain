#include "Chunk.h"

#include <iostream>

Chunk::Chunk(int x, int y, int z) : x(x), y(y), z(z) {
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                blockGrid[i][j][k] = Block(0);
            }
        }
    }
}

void Chunk::generateBlocks(const siv::PerlinNoise &terrainNoise, const int WORLD_Z_SIZE) {
    int x = 0;
    int y = 0;
    int z = 0;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                x = this->x + i;
                y = this->y + j;
                z = this->z + k;
                int blockID = terrainNoise.noise3D_01(x/16.0f, y/16.0f, z/16.0f) >= (float) (z)/WORLD_Z_SIZE  ? 11 : 0;
                blockGrid[i][j][k] = Block(blockID);
            }
        }
    }
}

void Chunk::generateMesh() {
    std::pair <float, float> textureCoordinates = std::pair<float, float>(0.0f, 0.0f);
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                if (blockGrid[i][j][k].ID != 0) {
                    if (i == CHUNK_SIZE - 1 || blockGrid[i + 1][j][k].ID == 0) {
                        positiveXFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_X, blockGrid[i][j][k].ID));
                    }
                    if (i == 0 || blockGrid[i - 1][j][k].ID == 0) {
                        negativeXFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_X, blockGrid[i][j][k].ID));
                    }
                    if (j == CHUNK_SIZE - 1 || blockGrid[i][j + 1][k].ID == 0) {
                        positiveYFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Y, blockGrid[i][j][k].ID));
                    }
                    if (j == 0 || blockGrid[i][j - 1][k].ID == 0) {
                        negativeYFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Y, blockGrid[i][j][k].ID));
                    }
                    if (k == CHUNK_SIZE - 1 || blockGrid[i][j][k + 1].ID == 0) {
                        positiveZFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Z, blockGrid[i][j][k].ID));
                    }
                    if (k == 0 || blockGrid[i][j][k - 1].ID == 0) {
                        negativeZFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Z, blockGrid[i][j][k].ID));
                    }
                }
            }
        }
    }
} 