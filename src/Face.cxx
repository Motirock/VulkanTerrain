#include "Face.h"

BlockType::BlockType(int ID, std::string name, std::pair<float, float> *textureCoordinates) {
    this->ID = ID;
    this->name = name;
    for (int i = 0; i < NUMBER_OF_ORIENTATIONS; i++) {
        this->textureCoordinates[i] = textureCoordinates[i];
    }
}

BlockType::BlockType() {
    this->ID = 0;
    this->name = "Null Block";
    for (int i = 0; i < NUMBER_OF_ORIENTATIONS; i++) {
        this->textureCoordinates[i] = std::pair<float, float>(0.0f, 0.0f);
    }
}

Face::Face(glm::vec3 facePosition, Orientation faceOrientation, int faceBlockID) 
    : position(facePosition), orientation(faceOrientation), blockID(faceBlockID) {}

Face::Face(float x, float y, float z, Orientation faceOrientation, int faceBlockID) 
    : position(glm::vec3(x, y, z)), orientation(faceOrientation), blockID(faceBlockID) {}