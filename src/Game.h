#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#pragma once

#include "Tetromino.h"

//Game class - Responsible for running the game - Has all its associated functions and members
class Game {
    public:
        Game();

        //Function responsible for generating a random tetromino
        void drawTetromino();

        //Function responsible for refilling the array with the tetrominos once they all have been selected
        void refillTetrominos();

        void startGame();

        void display();

        bool moveTetromino(SDL_Event &e);

        void moveTetrominoDown();

        bool rotateTetromino();

        void checkScore();

        void render();

        char getCurrentTetrominoSymbol();

        void checkTetrominoPresent();

        bool getCollision();

        void setCollision(bool status);

        void checkGameOver();

        bool getGameOver();

        bool getRestartRequest();

        void displayGameOver();

        void clearLines();

        bool getAnimationMode();

        bool getGamePaused();

        void setGamePause();

        int getCurrentGameOption();

        void setCurrentGameOption(int number);

        void setRestartRequest(bool value);

        bool getMoveLock();

        void setMoveLock(bool lock);

        bool getTimeoutCollision();

        void setTimeoutCollision(bool timeoutCollision);

        void finalizePosition();

    private:
        //Tetris Board(? Idk what its called thats the closest i could think of)
        char** mBoard;

        //Original tetrominos that are used to refill the other ones
        char* mOriginalTetrominos;

        //Current tetrominos that are getting drawn and chosen from
        char* mCurrentTetrominos;

        //Current counter to keep track of the tetronimos that will be drawn from (im tired of the word tetromino)
        int mCurrentTetrominosCount;

        int mTotalTetrominos;

        Tetromino mCurrentTetromino;

        Tetromino mOriginalTetromino;

        Tetromino mNextTetromino;

        char mCurrentTetrominoSymbol;

        char mNextTetrominoSymbol;

        int mRotation;

        int mScore;

        int mLinesCleared;

        bool mGameOver;

        bool mCollision;

        bool mRestartRequest;

        bool mAnimationMode;

        bool mGamePaused;

        bool mMoveLock;

        bool mTimeoutCollision;

        std::vector<int> mLinesClearedIndexes;

        int mCurrentOptionGameOver;

        int mMusicOptions;
        
        LTexture mBlueBlock;
        LTexture mCyanBlock;
        LTexture mGreenBlock;
        LTexture mPinkBlock;
        LTexture mPurpleBlock;
        LTexture mYellowBlock;
        LTexture mRedBlock;
        
        LTexture mScoreTexture;
        LTexture mLinesClearedTexture;
        LTexture mNextTexture;
        LTexture mBackgroundGrid;

        LTexture mGameOverRestartButton;
        LTexture mGameOverQuitButton;
        LTexture mGameOverMenuButton;
        
};