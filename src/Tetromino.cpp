#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#include "Tetromino.h"

Tetromino::Tetromino(char tetromino) {
    mTetrominoShape = tetromino;

    if(tetromino == 'O' || tetromino == 'o') {
        mShape = {{'O', 'O'},
                  {'O', 'O'}};
    }
    else if(tetromino == 'L' || tetromino == 'l') {
        mShape = {{'L', 'X'},
                  {'L', 'X'},
                  {'L', 'L'}};
    }
    else if(tetromino == 'J' || tetromino == 'j') {
        mShape = {{'X', 'J'},
                  {'X', 'J'},
                  {'J', 'J'}};
    }
    else if(tetromino == 'Z' || tetromino == 'z') {
        mShape = {{'Z', 'Z', 'X'},
                  {'X', 'Z', 'Z'}};
    }
    else if(tetromino == 'S' || tetromino == 's') {
        mShape = {{'X', 'S', 'S'},
                  {'S', 'S', 'X'}};
    }
    else if(tetromino == 'I' || tetromino == 'i') {
        mShape = {{'I'},
                  {'I'},
                  {'I'},
                  {'I'}};
    }
    else if(tetromino == 'T' || tetromino == 't') {
        mShape = {{'X', 'T', 'X'},
                  {'T', 'T', 'T'}};
    }

    mPosX = -1;
    mPosY = -1;
}

Tetromino::Tetromino() {
}

void Tetromino::incrementX() {
    mPosX++;
}

void Tetromino::incrementY() {
    mPosY++;
}

void Tetromino::decrementY() {
    mPosY--;
}

void Tetromino::setX(int x) {
    mPosX = x;
}

void Tetromino::display() {
    for(auto x : mShape) {
        for(auto y : x) {
            std::cout << y << " ";
        }
        std::cout << std::endl;
    }
}

void Tetromino::rotate() {
    int arraySize = mShape.size();
    int arrayElementsSize = mShape[0].size();
    std::vector<std::vector<char>> transposedShape;
    for(int i = 0 ; i < arrayElementsSize ; i++) {
        std::vector<char> charVector;
        for(int j = 0 ; j < arraySize ; j++) {
            charVector.push_back('X');
        }
        transposedShape.push_back(charVector);
    }

    for(int i = 0 ; i < arrayElementsSize ; i++) {
        for(int j = 0 ; j < arraySize ; j++) {
            transposedShape[i][j] = mShape[(arraySize - 1) - j][i];
        }
    }
    mShape = transposedShape;
}

std::vector<std::vector<char>> Tetromino::getShape() {
    return mShape;
}