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
                int blockID = 0;
                if (terrainNoise.noise3D_01(x/16.0f, y/16.0f, z/16.0f) >= (float) (z)/WORLD_Z_SIZE) {
                    blockID = terrainNoise.noise3D_01(x/16.0f, y/16.0f, (z+1.0f)/16.0f) >= (float) (z+1.0f)/WORLD_Z_SIZE ? 1 : 2;
                }
                blockGrid[i][j][k] = Block(blockID);
                
            }
        }
    }
}

void Chunk::generateMesh(Orientation orientation, Chunk *borderingChunk) {
    if (borderingChunk == NULL) {
    switch (orientation) {
    case POSITIVE_X:
        positiveXFaces.clear();
        for (int i = 0; i < CHUNK_SIZE; i++) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (blockGrid[i][j][k].ID != 0 && i != CHUNK_SIZE - 1 && blockGrid[i + 1][j][k].ID == 0) {
                        positiveXFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_X, blockGrid[i][j][k].ID));
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
                    if (blockGrid[i][j][k].ID != 0 && i != 0 && blockGrid[i - 1][j][k].ID == 0) {
                        negativeXFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_X, blockGrid[i][j][k].ID));
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
                    if (blockGrid[i][j][k].ID != 0 && j != CHUNK_SIZE - 1 && blockGrid[i][j + 1][k].ID == 0) {
                        positiveYFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Y, blockGrid[i][j][k].ID));
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
                    if (blockGrid[i][j][k].ID != 0 && j != 0 && blockGrid[i][j - 1][k].ID == 0) {
                        negativeYFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Y, blockGrid[i][j][k].ID));
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
                    if (blockGrid[i][j][k].ID != 0 && k != CHUNK_SIZE - 1 && blockGrid[i][j][k + 1].ID == 0) {
                        positiveZFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Z, blockGrid[i][j][k].ID));
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
                    if (blockGrid[i][j][k].ID != 0 && k != 0 && blockGrid[i][j][k - 1].ID == 0) {
                        negativeZFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Z, blockGrid[i][j][k].ID));
                    }
                }
            }
        }
        break;
    }
    } else {
    switch (orientation) {
    case POSITIVE_X:
        positiveXFaces.clear();
        for (int i = 0; i < CHUNK_SIZE; i++) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (blockGrid[i][j][k].ID == 0)
                        continue;
                    if (i == CHUNK_SIZE - 1 && borderingChunk->blockGrid[0][j][k].ID == 0)
                        positiveXFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_X, blockGrid[i][j][k].ID));
                    else if (i < CHUNK_SIZE-1 && blockGrid[i+1][j][k].ID == 0)
                        positiveXFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_X, blockGrid[i][j][k].ID));
                }
            }
        }
        break;
    case NEGATIVE_X:
        negativeXFaces.clear();
        for (int i = CHUNK_SIZE-1; i >= 0; i--) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (blockGrid[i][j][k].ID == 0)
                        continue;
                    if (i == 0 && borderingChunk->blockGrid[CHUNK_SIZE-1][j][k].ID == 0)
                        negativeXFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_X, blockGrid[i][j][k].ID));
                    else if (i > 0 && blockGrid[i-1][j][k].ID == 0)
                        negativeXFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_X, blockGrid[i][j][k].ID));
                }
            }
        }
        break;
    case POSITIVE_Y:
        positiveYFaces.clear();
        for (int j = 0; j < CHUNK_SIZE; j++) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (blockGrid[i][j][k].ID == 0)
                        continue;
                    if (j == CHUNK_SIZE - 1 && borderingChunk->blockGrid[i][0][k].ID == 0)
                        positiveYFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Y, blockGrid[i][j][k].ID));
                    else if (j < CHUNK_SIZE-1 && blockGrid[i][j+1][k].ID == 0)
                        positiveYFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Y, blockGrid[i][j][k].ID));
                }
            }
        }
        break;
    case NEGATIVE_Y:
        negativeYFaces.clear();
        for (int j = CHUNK_SIZE-1; j >= 0; j--) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (blockGrid[i][j][k].ID == 0)
                        continue;
                    if (j == 0 && borderingChunk->blockGrid[i][CHUNK_SIZE-1][k].ID == 0)
                        negativeYFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Y, blockGrid[i][j][k].ID));
                    else if (j > 0 && blockGrid[i][j-1][k].ID == 0)
                        negativeYFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Y, blockGrid[i][j][k].ID));
                }
            }
        }
        break;
    case POSITIVE_Z:
        positiveZFaces.clear();
        for (int k = 0; k < CHUNK_SIZE; k++) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int j = 0; j < CHUNK_SIZE; j++) {
                    if (blockGrid[i][j][k].ID == 0)
                        continue;
                    if (k == CHUNK_SIZE - 1 && borderingChunk->blockGrid[i][j][0].ID == 0)
                        positiveZFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Z, blockGrid[i][j][k].ID));
                    else if (k < CHUNK_SIZE-1 && blockGrid[i][j][k+1].ID == 0)
                        positiveZFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Z, blockGrid[i][j][k].ID));
                }
            }
        }
        break;
    case NEGATIVE_Z:
        negativeZFaces.clear();
        for (int k = CHUNK_SIZE-1; k >= 0; k--) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int j = 0; j < CHUNK_SIZE; j++) {
                    if (blockGrid[i][j][k].ID == 0)
                        continue;
                    if (k == 0 && borderingChunk->blockGrid[i][j][CHUNK_SIZE-1].ID == 0)
                        negativeZFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Z, blockGrid[i][j][k].ID));
                    else if (k > 0 && blockGrid[i][j][k-1].ID == 0)
                        negativeZFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Z, blockGrid[i][j][k].ID));
                }
            }
        }
        break;
    }
    }






    /*for (int i = 0; i < CHUNK_SIZE; i++) {
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
    }*/
} 