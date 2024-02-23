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
    blocksChanged = getBlock(x, y, z) != block ? true : blocksChanged;
    blockGrid[x*CHUNK_SIZE*CHUNK_SIZE+y*CHUNK_SIZE+z] = block;
}

void Chunk::setBlock(int x, int y, int z, int ID) {
    Block block = Block(ID);
    blocksChanged = getBlock(x, y, z) != block ? true : blocksChanged;
    blockGrid[x*CHUNK_SIZE*CHUNK_SIZE+y*CHUNK_SIZE+z] = block;
}

float Chunk::getNoiseValue(int x, int y, int z, WorldGenerationInfo &worldGenerationInfo, const siv::PerlinNoise &terrainNoise, const int WORLD_Z_SIZE, NoiseMethod method) {
    float r = 0.0f;
    float level = worldGenerationInfo.seaLevel;
    float threshold = worldGenerationInfo.seaLevelThreshold;
    float exponentBelow = 0.8f;
    float exponentAbove = 1.5f;
    switch (method) {
    case TERRAIN_NOISE:
        r = terrainNoise.octave2D_01(x*worldGenerationInfo.terrainFrequency, y*worldGenerationInfo.terrainFrequency, 6);
        r *= 0.9f;
        r += terrainNoise.noise3D_01(x*worldGenerationInfo.terrainFrequency/50.0f, y*worldGenerationInfo.terrainFrequency/50.0f, z*worldGenerationInfo.terrainFrequency/20.0f)*0.1f;
        
        if (r < threshold)
            r = -pow(fabs(r-threshold)/threshold, exponentBelow)*level+level;
        else
            r = pow((r-threshold)/(1-threshold), exponentAbove)*(1-level)+level;
        
        r *= WORLD_Z_SIZE;
        break;
    case CAVE_NOISE:
        r = terrainNoise.noise3D_01(x/100.0f+terrainNoise.noise3D_01(z/10.0f-10, y/10.0f, x/10.0f)/5.0f, y/100.0f+terrainNoise.noise3D_01(x/10.0f-10, y/10.0f, z/10.0f)/5.0f, z/20.0f+terrainNoise.noise3D_01(y/10.0f-10, z/10.0f, x/10.0f)/2.0f);
        break;
    }
    return r;
}

void Chunk::generateBlocks(WorldGenerationInfo &worldGenerationInfo, const siv::PerlinNoise &terrainNoise, const int WORLD_Z_SIZE, GenerationMethod method) {
    int x = 0;
    int y = 0;
    int z = 0;
    float seaLevel = worldGenerationInfo.seaLevel*WORLD_Z_SIZE;
    float snowLevel = worldGenerationInfo.snowLevel*WORLD_Z_SIZE;

    switch (method) {
    case NORMAL_GENERATION:
        //Basic filling in
        for (int i = 0; i < CHUNK_SIZE; i++) {
            x = this->x + i;
            for (int j = 0; j < CHUNK_SIZE; j++) {
                y = this->y + j;
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    z = this->z + k;
                    
                    int blockID = getNoiseValue(x, y, z, worldGenerationInfo, terrainNoise, WORLD_Z_SIZE, TERRAIN_NOISE) >= z ? 1 : 0;
                    Block block = Block(blockID);
                    setBlock(i, j, k, block);       
                }
            }
        }
        
        //Dirt, grass, snow, and sand
        for (int i = 0; i < CHUNK_SIZE; i++) {
            x = this->x + i;
            for (int j = 0; j < CHUNK_SIZE; j++) {
                y = this->y + j;
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    z = this->z + k;
                    if (getBlock(i, j, k).ID == 1) {
                        if (getNoiseValue(x, y, z+1, worldGenerationInfo, terrainNoise, WORLD_Z_SIZE, TERRAIN_NOISE) < z+1) {
                            if (z >= snowLevel+terrainNoise.noise2D_01(x*worldGenerationInfo.terrainFrequency*100.0f, y*worldGenerationInfo.terrainFrequency*100.0f)*WORLD_Z_SIZE*0.05) {
                                Block block = Block(9);
                                setBlock(i, j, k, block);
                            }
                            else if (z >= seaLevel+terrainNoise.noise2D_01(x*worldGenerationInfo.terrainFrequency*100.0f, y*worldGenerationInfo.terrainFrequency*100.0f)*WORLD_Z_SIZE*0.02-1) {
                                Block block = Block(4);
                                setBlock(i, j, k, block);
                            }
                            else {
                                Block block = Block(5);
                                setBlock(i, j, k, block);
                            }
                        }
                        else {
                            for (int I = 2; I <= (int) (terrainNoise.noise2D_01(y, x)*4.0f+1.0f); I++) {
                                if (getNoiseValue(x, y, z+I, worldGenerationInfo, terrainNoise, WORLD_Z_SIZE, TERRAIN_NOISE) < z+I) {
                                    if (z >= snowLevel+terrainNoise.noise2D_01(x*worldGenerationInfo.terrainFrequency*100.0f, y*worldGenerationInfo.terrainFrequency*100.0f)*WORLD_Z_SIZE*0.05) {
                                        Block block = Block(9);
                                        setBlock(i, j, k, block);
                                    }
                                    else if (z >= seaLevel+terrainNoise.noise2D_01(x*worldGenerationInfo.terrainFrequency*100.0f, y*worldGenerationInfo.terrainFrequency*100.0f)*WORLD_Z_SIZE*0.02-1) {
                                        Block block = Block(3);
                                        setBlock(i, j, k, block);
                                    }
                                    else {
                                        Block block = Block(5);
                                        setBlock(i, j, k, block);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        //Caves
        /*for (int i = 0; i < CHUNK_SIZE; i++) {
            x = this->x + i;
            for (int j = 0; j < CHUNK_SIZE; j++) {
                y = this->y + j;
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    if (getBlock(i, j, k).ID == 0 || getBlock(i, j, k).ID == 6)
                        continue;
                    z = this->z + k;
                    
                    if (getNoiseValue(x, y, z, worldGenerationInfo, terrainNoise, WORLD_Z_SIZE, CAVE_NOISE) < worldGenerationInfo.seaLevel*(1.0f-(float) z/WORLD_Z_SIZE)*0.8) {
                        Block block = Block(0);
                        setBlock(i, j, k, block);       
                    }
                }
            }
        }*/

        //Water
        if (this->z < seaLevel) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                x = this->x + i;
                for (int j = 0; j < CHUNK_SIZE; j++) {
                    y = this->y + j;
                    for (int k = 0; k < CHUNK_SIZE && this->z + k < seaLevel; k++) {
                        if (getBlock(i, j, k).ID == 0) {
                            Block block = Block(6);
                            setBlock(i, j, k, block);
                        }
                    }
                }
            }
        }
        
        //Bottom bedrock layer
        if (this->z == 0) {
            for (int i = 0; i < CHUNK_SIZE; i++) {
                for (int j = 0; j < CHUNK_SIZE; j++) {
                    Block block = Block(2);
                    setBlock(i, j, 0, block);
                }
            }
        }
        //Top bedrock layer (goofy)
        // if (this->z >= 448) {
        //     for (int i = 0; i < CHUNK_SIZE; i++) {
        //         for (int j = 0; j < CHUNK_SIZE; j++) {
        //             Block block = Block(2);
        //             setBlock(i, j, CHUNK_SIZE-1, block);
        //         }
        //     }
        // }
        break;

    case BRAIN_GENERATION:
        for (int i = 0; i < CHUNK_SIZE; i++) {
            for (int j = 0; j < CHUNK_SIZE; j++) {
                for (int k = 0; k < CHUNK_SIZE; k++) {
                    x = this->x + i;
                    y = this->y + j;
                    z = this->z + k;
                    
                    int blockID = 0;
                    if (getNoiseValue(x, y, z, worldGenerationInfo, terrainNoise, WORLD_Z_SIZE, CAVE_NOISE) >= 0.45) {
                        blockID = 1;
                    }
                    else
                        blockID = 0;
                    
                    Block block = Block(blockID);
                    setBlock(i, j, k, block);       
                }
            }
        }
        break;
    }

    std::cout << "Done generating blocks for chunk at " << this->x << " " << this->y << " " << this->z << std::endl;
}

void Chunk::generateMesh(Orientation orientation, Chunk *borderingChunk, std::map<int, BlockType> &blockTypes) {
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
                    if (getBlock(i, j, k).ID != 0 && i != CHUNK_SIZE - 1 && !blockTypes[getBlock(i + 1, j, k).ID].isFullOpaque 
                        && !(getBlock(i + 1, j, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors)) {
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
                    if (getBlock(i, j, k).ID != 0 && i != 0 && !blockTypes[getBlock(i - 1, j, k).ID].isFullOpaque 
                        && !(getBlock(i - 1, j, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors)) {
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
                    if (getBlock(i, j, k).ID != 0 && j != CHUNK_SIZE - 1 && !blockTypes[getBlock(i, j + 1, k).ID].isFullOpaque 
                        && !(getBlock(i, j + 1, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors)) {
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
                    if (getBlock(i, j, k).ID != 0 && j != 0 && !blockTypes[getBlock(i, j-1, k).ID].isFullOpaque 
                        && !(getBlock(i, j-1, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors)) {
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
                    if (getBlock(i, j, k).ID != 0 && k != CHUNK_SIZE - 1 && !blockTypes[getBlock(i, j, k+1).ID].isFullOpaque 
                        && !(getBlock(i, j, k+1).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors)) {
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
                    if (getBlock(i, j, k).ID != 0 && k != 0 && !blockTypes[getBlock(i, j, k-1).ID].isFullOpaque 
                        && !(getBlock(i, j, k-1).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors)) {
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
                    if (i == CHUNK_SIZE - 1 && !blockTypes[borderingChunk->getBlock(0, j, k).ID].isFullOpaque 
                        && !(borderingChunk->getBlock(0, j, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
                        positiveXFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_X, getBlock(i, j, k).ID));
                    else if (i < CHUNK_SIZE-1 && !blockTypes[getBlock(i+1, j, k).ID].isFullOpaque 
                        && !(getBlock(i+1, j, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
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
                    if (i == 0 && !blockTypes[borderingChunk->getBlock(CHUNK_SIZE-1, j, k).ID].isFullOpaque 
                        && !(borderingChunk->getBlock(CHUNK_SIZE-1, j, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
                        negativeXFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_X, getBlock(i, j, k).ID));
                    else if (i > 0 && !blockTypes[getBlock(i-1, j, k).ID].isFullOpaque 
                       && !(getBlock(i-1, j, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
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
                    if (j == CHUNK_SIZE - 1 && !blockTypes[borderingChunk->getBlock(i, 0, k).ID].isFullOpaque 
                        && !(borderingChunk->getBlock(i, 0, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
                        positiveYFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Y, getBlock(i, j, k).ID));
                    else if (j < CHUNK_SIZE-1 && !blockTypes[getBlock(i, j+1, k).ID].isFullOpaque 
                        && !(getBlock(i, j+1, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
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
                    if (j == 0 && !blockTypes[borderingChunk->getBlock(i, CHUNK_SIZE-1, k).ID].isFullOpaque 
                        && !(borderingChunk->getBlock(i, CHUNK_SIZE-1, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
                        negativeYFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Y, getBlock(i, j, k).ID));
                    else if (j > 0 && !blockTypes[getBlock(i, j-1, k).ID].isFullOpaque 
                        && !(getBlock(i, j-1, k).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
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
                    if (k == CHUNK_SIZE - 1 && !blockTypes[borderingChunk->getBlock(i, j, 0).ID].isFullOpaque 
                        && !(borderingChunk->getBlock(i, j, 0).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
                        positiveZFaces.push_back(Face(x+i, y+j, z+k, POSITIVE_Z, getBlock(i, j, k).ID));
                    else if (k < CHUNK_SIZE-1 && !blockTypes[getBlock(i, j, k+1).ID].isFullOpaque 
                        && !(getBlock(i, j, k+1).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
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
                    if (k == 0 && !blockTypes[borderingChunk->getBlock(i, j, CHUNK_SIZE-1).ID].isFullOpaque 
                        && !(borderingChunk->getBlock(i, j, CHUNK_SIZE-1).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
                        negativeZFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Z, getBlock(i, j, k).ID));
                    else if (k > 0 && !blockTypes[getBlock(i, j, k-1).ID].isFullOpaque 
                        && !(getBlock(i, j, k-1).ID == getBlock(i, j, k).ID && blockTypes[getBlock(i, j, k).ID].hideSameNeighbors))
                        negativeZFaces.push_back(Face(x+i, y+j, z+k, NEGATIVE_Z, getBlock(i, j, k).ID));
                }
            }
        }
        break;
    }
    }

    std::cout << "Done generating mesh for chunk at " << x << " " << y << " " << z << " " << positiveXFaces.size() << std::endl;
} 

void Chunk::uploadFace(Face &face, std::map<int, BlockType> &blockTypes) {
        uint32_t vertexCount = vertices.size();
        float textureX = blockTypes[face.blockID].textureCoordinates[face.orientation].first/8.0f, textureY = blockTypes[face.blockID].textureCoordinates[face.orientation].second/8.0f;
        float r = 1.0f, g = 1.0f, b = 1.0f;
        uint32_t normalAndColor = packNormalAndColor(face.orientation, r, g, b);

        switch (face.orientation) {
        case POSITIVE_X:
            switch (face.blockID) {
            case 4:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 1.0f, 0.0f);
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.000f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.000f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.000f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.000f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+2);
                vertexCount += 4;

                textureX = blockTypes[3].textureCoordinates[face.orientation].first/8.0f;
                textureY = blockTypes[3].textureCoordinates[face.orientation].second/8.0f;
                normalAndColor = packNormalAndColor(face.orientation, 1.0f, 1.0f, 1.0f);
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+2);
                return;
            case 6:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 0.2f, 0.6f);
                break;
            case 8:
                normalAndColor = packNormalAndColor(face.orientation, 0.1f, 0.8f, 0.0f);
                break;
            }
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
            indices.push_back(vertexCount+0);
            indices.push_back(vertexCount+1);
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+0);
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+2);
            return;
        case NEGATIVE_X:
            switch (face.blockID) {
            case 4:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 1.0f, 0.0f);
                vertices.push_back(Vertex{glm::vec3(face.position.x-0.000f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x-0.000f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x-0.000f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x-0.000f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+2);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                vertexCount += 4;

                textureX = blockTypes[3].textureCoordinates[face.orientation].first/8.0f;
                textureY = blockTypes[3].textureCoordinates[face.orientation].second/8.0f;
                normalAndColor = packNormalAndColor(face.orientation, 1.0f, 1.0f, 1.0f);
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+2);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                return;
            case 6:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 0.2f, 0.6f);
                break;
            case 8:
                normalAndColor = packNormalAndColor(face.orientation, 0.1f, 0.8f, 0.0f);
                break;
            }
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+1);
            indices.push_back(vertexCount+0);
            indices.push_back(vertexCount+2);
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+0);
            return;
        case POSITIVE_Y:
            switch (face.blockID) {
            case 4:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 1.0f, 0.0f);
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.000f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.000f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.000f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.000f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+2);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                vertexCount += 4;
            
                textureX = blockTypes[3].textureCoordinates[face.orientation].first/8.0f;
                textureY = blockTypes[3].textureCoordinates[face.orientation].second/8.0f;
                normalAndColor = packNormalAndColor(face.orientation, 1.0f, 1.0f, 1.0f);
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+2);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                return;
            case 6:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 0.2f, 0.6f);
                break;
            case 8:
                normalAndColor = packNormalAndColor(face.orientation, 0.1f, 0.8f, 0.0f);
                break;
            }
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+1);
            indices.push_back(vertexCount+0);
            indices.push_back(vertexCount+2);
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+0);
            return;
        case NEGATIVE_Y:
            switch (face.blockID) {
            case 4:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 1.0f, 0.0f);
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y-0.000f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y-0.000f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y-0.000f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y-0.000f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+2);
                vertexCount += 4;

                textureX = blockTypes[3].textureCoordinates[face.orientation].first/8.0f;
                textureY = blockTypes[3].textureCoordinates[face.orientation].second/8.0f;
                normalAndColor = packNormalAndColor(face.orientation, 1.0f, 1.0f, 1.0f);
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+2);
                return;
            case 6:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 0.2f, 0.6f);
                break;
            case 8:
                normalAndColor = packNormalAndColor(face.orientation, 0.1f, 0.8f, 0.0f);
                break;
            }
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
            indices.push_back(vertexCount+0);
            indices.push_back(vertexCount+1);
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+0);
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+2);
            return;
        case POSITIVE_Z:
            switch (face.blockID) {
            case 4:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 1.0f, 0.0f);
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+2);
                return;
            case 6:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 0.2f, 0.6f);
                break;
            case 8:
                normalAndColor = packNormalAndColor(face.orientation, 0.1f, 0.8f, 0.0f);
                break;
            }
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+1.0f), glm::vec2(textureX, textureY), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+1.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor});
            indices.push_back(vertexCount+0);
            indices.push_back(vertexCount+1);
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+0);
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+2);
            return;
        case NEGATIVE_Z:
            switch (face.blockID) {
            case 4:
                textureX = blockTypes[3].textureCoordinates[face.orientation].first/8.0f;
                textureY = blockTypes[3].textureCoordinates[face.orientation].second/8.0f;
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX, textureY), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
                vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor}); 
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+1);
                indices.push_back(vertexCount+0);
                indices.push_back(vertexCount+2);
                indices.push_back(vertexCount+3);
                indices.push_back(vertexCount+0);
                return;
            case 6:
                normalAndColor = packNormalAndColor(face.orientation, 0.0f, 0.2f, 0.6f);
                break;
            case 8:
                normalAndColor = packNormalAndColor(face.orientation, 0.1f, 0.8f, 0.0f);
                break;
            }
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+0.0f, face.position.z+0.0f), glm::vec2(textureX, textureY), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+0.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY+1.0f/8.0f), normalAndColor});
            vertices.push_back(Vertex{glm::vec3(face.position.x+1.0f, face.position.y+1.0f, face.position.z+0.0f), glm::vec2(textureX+1.0f/8.0f, textureY), normalAndColor}); 
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+1);
            indices.push_back(vertexCount+0);
            indices.push_back(vertexCount+2);
            indices.push_back(vertexCount+3);
            indices.push_back(vertexCount+0);
            return;
    }
}

void Chunk::uploadFaces(Orientation orientation, std::map<int, BlockType> &blockTypes) {
    switch (orientation) {
    case POSITIVE_X:
        for (Face &face : positiveXFaces) {
            uploadFace(face, blockTypes);
        }
        break;
    case NEGATIVE_X:
        for (Face &face : negativeXFaces) {
            uploadFace(face, blockTypes);
        }
        break;
    case POSITIVE_Y:
        for (Face &face : positiveYFaces) {
            uploadFace(face, blockTypes);
        }
        break;
    case NEGATIVE_Y:
        for (Face &face : negativeYFaces) {
            uploadFace(face, blockTypes);
        }
        break;
    case POSITIVE_Z:
        for (Face &face : positiveZFaces) {
            uploadFace(face, blockTypes);
        }
        break;
    case NEGATIVE_Z:
        for (Face &face : negativeZFaces) {
            uploadFace(face, blockTypes);
        }
        break;
    }
}

void Chunk::uploadVertices() {
    vertexCount = vertices.size();
    VkDeviceSize bufferSize = vertexCount*sizeof(Vertex);
    memcpy(vertexData, vertices.data(), bufferSize);
}

void Chunk::uploadIndices() {
    indexCount = indices.size();
    VkDeviceSize bufferSize = indexCount*sizeof(uint32_t);
    memcpy(indexData, indices.data(), bufferSize);
}