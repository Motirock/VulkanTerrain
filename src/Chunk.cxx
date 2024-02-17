#include "Chunk.h"

#include <iostream>

Chunk::Chunk(int x, int y, int z) : x(x), y(y), z(z) {
    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int k = 0; k < CHUNK_SIZE; k++) {
                getBlock(i, j, k) = Block(0);
            }
        }
    }
}

Block& Chunk::getBlock(int x, int y, int z) {
    return blockGrid[x*CHUNK_SIZE*CHUNK_SIZE+y*CHUNK_SIZE+z];
}

void Chunk::setBlock(int x, int y, int z, Block &block) {
    blockGrid[x*CHUNK_SIZE*CHUNK_SIZE+y*CHUNK_SIZE+z] = block;
    blocksChanged = true;
}

float Chunk::getElevation(int x, int y, int z, const siv::PerlinNoise &terrainNoise, const int WORLD_Z_SIZE) {
    return (terrainNoise.noise2D_01(x/16.0f, y/16.0f)*0.6 + terrainNoise.noise3D_01(x/16.0f, y/16.0f, z/16.0f)*0.4) * WORLD_Z_SIZE;
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
                
                //Bottom bedrock layer
                if (z == 0) {
                    std::cout << getElevation(x, y, z, terrainNoise, WORLD_Z_SIZE) << std::endl;
                    Block block = Block(2);
                    setBlock(i, j, k, block);
                    continue;
                }

                int blockID = 0;
                
                if (getElevation(x, y, z, terrainNoise, WORLD_Z_SIZE) >= z) {
                    blockID = getElevation(x, y, z+1.0f, terrainNoise, WORLD_Z_SIZE) >= z+1.0f ? 3 : 4;
                }
                Block block = Block(blockID);
                setBlock(i, j, k, block);
                
            }
        }
    }
}

void Chunk::generateMesh(Orientation orientation, Chunk *borderingChunk) {
    if (!blocksChanged)
        return;
    
    //Edge chunk
    if (borderingChunk == NULL) {
    switch (orientation) {
    case POSITIVE_X:
        positiveXFaces.clear();
        for (int i = 0; i < CHUNK_SIZE; i++) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (getBlock(i, j, k).ID != 0 && i != CHUNK_SIZE - 1 && getBlock(i + 1, j, k).ID == 0) {
                        positiveXFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_X, getBlock(i, j, k).ID));
                    }
                }
            }
        }
        break;
    case NEGATIVE_X:
        negativeXFaces.clear();
        for (int i = CHUNK_SIZE-1; i >= 0; i--) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (getBlock(i, j, k).ID != 0 && i != 0 && getBlock(i - 1, j, k).ID == 0) {
                        negativeXFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_X, getBlock(i, j, k).ID));
                    }
                }
            }
        }
        break;
    case POSITIVE_Y:
        positiveYFaces.clear();
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (getBlock(i, j, k).ID != 0 && j != CHUNK_SIZE - 1 && getBlock(i, j + 1, k).ID == 0) {
                        positiveYFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Y, getBlock(i, j, k).ID));
                    }
                }
            }
        }
        break;
    case NEGATIVE_Y:
        negativeYFaces.clear();
        for (int j = CHUNK_SIZE-1; j >= 0; j--) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (getBlock(i, j, k).ID != 0 && j != 0 && getBlock(i, j-1, k).ID == 0) {
                        negativeYFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Y, getBlock(i, j, k).ID));
                    }
                }
            }
        }
        break;
    case POSITIVE_Z:
        positiveZFaces.clear();
        for (int k = 0; k < CHUNK_SIZE; k++) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int j = 0; j < CHUNK_SIZE; j++) {
                    if (getBlock(i, j, k).ID != 0 && k != CHUNK_SIZE - 1 && getBlock(i, j, k+1).ID == 0) {
                        positiveZFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Z, getBlock(i, j, k).ID));
                    }
                }
            }
        }
        break;
    case NEGATIVE_Z:
        negativeZFaces.clear();
        for (int k = CHUNK_SIZE-1; k >= 0; k--) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int j = 0; j < CHUNK_SIZE; j++) {
                    if (getBlock(i, j, k).ID != 0 && k != 0 && getBlock(i, j, k-1).ID == 0) {
                        negativeZFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Z, getBlock(i, j, k).ID));
                    }
                }
            }
        }
        break;
    }
    } 
    //Non-edge chunk
    else {
    switch (orientation) {
    case POSITIVE_X:
        positiveXFaces.clear();
        for (int i = 0; i < CHUNK_SIZE; i++) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (getBlock(i, j, k).ID == 0)
                        continue;
                    if (i == CHUNK_SIZE - 1 && borderingChunk->getBlock(0, j, k).ID == 0)
                        positiveXFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_X, getBlock(i, j, k).ID));
                    else if (i < CHUNK_SIZE-1 && getBlock(i+1, j, k).ID == 0)
                        positiveXFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_X, getBlock(i, j, k).ID));
                }
            }
        }
        break;
    case NEGATIVE_X:
        negativeXFaces.clear();
        for (int i = CHUNK_SIZE-1; i >= 0; i--) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (getBlock(i, j, k).ID == 0)
                        continue;
                    if (i == 0 && borderingChunk->getBlock(CHUNK_SIZE-1, j, k).ID == 0)
                        negativeXFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_X, getBlock(i, j, k).ID));
                    else if (i > 0 && getBlock(i-1, j, k).ID == 0)
                        negativeXFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_X, getBlock(i, j, k).ID));
                }
            }
        }
        break;
    case POSITIVE_Y:
        positiveYFaces.clear();
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (getBlock(i, j, k).ID == 0)
                        continue;
                    if (j == CHUNK_SIZE - 1 && borderingChunk->getBlock(i, 0, k).ID == 0)
                        positiveYFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Y, getBlock(i, j, k).ID));
                    else if (j < CHUNK_SIZE-1 && getBlock(i, j+1, k).ID == 0)
                        positiveYFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Y, getBlock(i, j, k).ID));
                }
            }
        }
        break;
    case NEGATIVE_Y:
        negativeYFaces.clear();
        for (int j = CHUNK_SIZE-1; j >= 0; j--) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (getBlock(i, j, k).ID == 0)
                        continue;
                    if (j == 0 && borderingChunk->getBlock(i, CHUNK_SIZE-1, k).ID == 0)
                        negativeYFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Y, getBlock(i, j, k).ID));
                    else if (j > 0 && getBlock(i, j-1, k).ID == 0)
                        negativeYFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Y, getBlock(i, j, k).ID));
                }
            }
        }
        break;
    case POSITIVE_Z:
        positiveZFaces.clear();
        for (int k = 0; k < CHUNK_SIZE; k++) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int j = 0; j < CHUNK_SIZE; j++) {
                    if (getBlock(i, j, k).ID == 0)
                        continue;
                    if (k == CHUNK_SIZE - 1 && borderingChunk->getBlock(i, j, 0).ID == 0)
                        positiveZFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Z, getBlock(i, j, k).ID));
                    else if (k < CHUNK_SIZE-1 && getBlock(i, j, k+1).ID == 0)
                        positiveZFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Z, getBlock(i, j, k).ID));
                }
            }
        }
        break;
    case NEGATIVE_Z:
        negativeZFaces.clear();
        for (int k = CHUNK_SIZE-1; k >= 0; k--) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int j = 0; j < CHUNK_SIZE; j++) {
                    if (getBlock(i, j, k).ID == 0)
                        continue;
                    if (k == 0 && borderingChunk->getBlock(i, j, CHUNK_SIZE-1).ID == 0)
                        negativeZFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Z, getBlock(i, j, k).ID));
                    else if (k > 0 && getBlock(i, j, k-1).ID == 0)
                        negativeZFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Z, getBlock(i, j, k).ID));
                }
            }
        }
        break;
    }
    }

    std::cout << "Generated mesh for chunk " << x << " " << y << " " << z << " " << positiveXFaces.size() << std::endl;
} 