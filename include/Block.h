#ifndef BLOCK_H
#define BLOCK_H

#include "Face.h"

struct Block {
    int ID = 0;

    Block(int blockID);
    Block();

    bool operator == (const Block &other);
    bool operator != (const Block &other); 
};

#endif