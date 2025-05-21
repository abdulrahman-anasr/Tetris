#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#pragma once

#include "LTexture.h"

class Tetromino {
    public:
        Tetromino(char tetromino);

        Tetromino();

        void rotate();

        std::vector<std::vector<char>> getShape();

        void display();

        void incrementX();

        void incrementY();

        void decrementY();

        void setX(int x);
    /*private:*/
        std::vector<std::vector<char>> mShape;
        char mTetrominoShape;
        int mPosX , mPosY;
};
