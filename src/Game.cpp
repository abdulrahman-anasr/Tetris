#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib> 

#include "Game.h"
#include "GlobalVariables.h"

double DegreesToRadians( double degrees )
{ 
    return degrees * 3.14 / 180; 
} 

Game::Game() {

    mTotalTetrominos = 7; //Just adding this here to avoid the use of magic numbers
    mRotation = 0;
    mScore = 0;
    mLinesCleared = 0;
    mGameOver = false;
    mCollision = false;
    mRestartRequest = false;
    mGamePaused = false;
    mMoveLock = false;
    mTimeoutCollision = false;
    mBlueBlock.loadFromFile("assets/blue_block/blue_block.png");
    mCyanBlock.loadFromFile("assets/cyan_block/cyan_block.png");
    mGreenBlock.loadFromFile("assets/green_block/green_block.png");
    mPinkBlock.loadFromFile("assets/pink_block/pink_block.png");
    mPurpleBlock.loadFromFile("assets/purple_block/purple_block.png");
    mYellowBlock.loadFromFile("assets/yellow_block/yellow_block.png");
    mRedBlock.loadFromFile("assets/red_block/red_block.png");

    mBackgroundGrid.loadFromFile("assets/grid_background.png");

    mCurrentOptionGameOver = 0;

    mAnimationMode = false;

    //Board is gonna be 20 x 10 , but leaving 4 for space to show the tetromino before placement (empty space pretty much, condition is gonna check for row 20 at most)
    mBoard = new char*[25];
    for(int i = 0 ; i < 25 ; i++) {
        mBoard[i] = new char[10];

        memset(mBoard[i] , 'X' , sizeof(char) * 10);
    }

    //Tetrominos that will be added to the original and current at the beginning (all shapes)
    char tetrominoShapeArray[] = {'O' , 'I' , 'S' , 'Z' , 'L' , 'J' , 'T'};

    //Filling tetromino array with all its associated shapes
    mOriginalTetrominos = new char[7];
    for(int i = 0 ; i < mTotalTetrominos ; i++) {
        mOriginalTetrominos[i] = tetrominoShapeArray[i];
    }

    mCurrentTetrominosCount = 7; //Current tetromino count starts at 7 and decrements
    mCurrentTetrominos = new char[7];
    for(int i = 0 ; i < mTotalTetrominos ; i++) { //Filling current tetromino array that will be drawn from
        mCurrentTetrominos[i] = tetrominoShapeArray[i];
    }

    mCurrentTetrominoSymbol = '*';
}

//Drawing tetrominos function
void Game::drawTetromino() {
    if(mCurrentTetrominoSymbol == '*') {
    mTimeoutCollision = false;
    mRotation = 0;
    /*std::cout << "Entering Drawing loop" << std::endl;*/
    std::srand(time(0));
    int random = rand() % mCurrentTetrominosCount; //Generating random number within range of current tetrominos
    mCurrentTetrominosCount--;

    char tetrominoChosen = mCurrentTetrominos[random];
    //Shift tetrominos so drawing pot is within the current tetrominos (avoid the idea of checking against if this tetromino was already chosen before etc. etc.)
    for(int i = random ; i < mTotalTetrominos - 1 ; i++) {
        mCurrentTetrominos[i] = mCurrentTetrominos[i + 1];
    }

    //If tetrominos are are at 0 refill them
    if(mCurrentTetrominosCount <= 0) {
        refillTetrominos();
    }

    mCurrentTetromino = Tetromino(tetrominoChosen);
    // 'O' , 'I' , 'S' , 'Z' , 'L' , 'J' , 'T'
    mCurrentTetrominoSymbol = tetrominoChosen;
    for(int i = 0 ; i < mCurrentTetromino.getShape().size() ; i++) {
        for(int j = 0 ; j < mCurrentTetromino.getShape()[0].size() ; j++) {
            mBoard[i][j + 3] = mCurrentTetromino.mShape[i][j];
        }
    }

    random = rand() % mCurrentTetrominosCount;
    mCurrentTetrominosCount--;

    tetrominoChosen = mCurrentTetrominos[random];
    for(int i = random ; i < mTotalTetrominos - 1 ; i++) {
        mCurrentTetrominos[i] = mCurrentTetrominos[i + 1];
    }

    if(mCurrentTetrominosCount <= 0) {
        refillTetrominos();
    }

    mNextTetromino = Tetromino(tetrominoChosen);
    mNextTetrominoSymbol = tetrominoChosen;

    }
    else {

        mCurrentTetromino = mNextTetromino;
        mCurrentTetrominoSymbol = mNextTetrominoSymbol;

        for(int i = 0 ; i < mCurrentTetromino.getShape().size() ; i++) {
            for(int j = 0 ; j < mCurrentTetromino.getShape()[0].size() ; j++) {
                mBoard[i][j + 3] = mCurrentTetromino.mShape[i][j];
            }
        }

        int random = rand() % mCurrentTetrominosCount;
        mCurrentTetrominosCount--;

        char tetrominoChosen = mCurrentTetrominos[random];
        for(int i = random ; i < mTotalTetrominos - 1 ; i++) {
            mCurrentTetrominos[i] = mCurrentTetrominos[i + 1];
        }

        if(mCurrentTetrominosCount <= 0) {
            refillTetrominos();
        }

        mNextTetromino = Tetromino(tetrominoChosen);
        mNextTetrominoSymbol = tetrominoChosen;
    }
    
}

//Refill tetrominos when all of them have been picked
void Game::refillTetrominos() {
    for(int i = 0 ; i < mTotalTetrominos ; i++) {
        mCurrentTetrominos[i] = mOriginalTetrominos[i];
    }
    mCurrentTetrominosCount = 7;
}

void Game::display() {
    for(int i = 0 ; i < 25 ; i++) {
        for(int j = 0 ; j < 10 ; j++) {
            std::cout << mBoard[i][j] << ' ' ;
        }
        std::cout << '\n';
    }
    std::cout << std::flush;
}

bool Game::moveTetromino(SDL_Event& e) {
    bool moved = false;
    if(!mGameOver) {
    if(mCurrentTetrominoSymbol == '0' || mAnimationMode) {
        return false;
    }
    bool collision = false;
    if(e.type == SDL_KEYDOWN && ( e.key.keysym.sym == SDLK_s || e.key.keysym.sym == SDLK_DOWN)) {
        bool allowed = true;
        for (int i = 24; i >= 0; i--)
        {
            for (int j = 9; j >= 0; j--)
            {
                if (mBoard[i][j] == mCurrentTetrominoSymbol)
                {
                    int newX = i + 1;
                    if(newX > 24 || ( mBoard[newX][j] != 'X' && mBoard[newX][j] != mCurrentTetrominoSymbol)) {
                        allowed = false;
                    }
                }
            }
        }
        if(allowed) {
        for (int i = 24; i >= 0; i--)
            {
                for (int j = 9; j >= 0; j--)
                {
                    if(mBoard[i][j] == mCurrentTetrominoSymbol) {
                        int newX = i + 1;
                        mBoard[newX][j] = mCurrentTetrominoSymbol;
                        mBoard[i][j] = 'X';
                    }
                }
            }
        }
        else {
            mCollision = true;
        }
        moved = allowed;
    }
    if (e.type == SDL_KEYDOWN && ( e.key.keysym.sym == SDLK_d || e.key.keysym.sym == SDLK_RIGHT))
    {
        bool allowed = true;
        for (int i = 24; i >= 0; i--)
        {
            for (int j = 9; j >= 0; j--)
            {
                if (mBoard[i][j] == mCurrentTetrominoSymbol)
                {
                    int newY = j + 1;
                    if(newY > 24 || ( mBoard[i][newY] != 'X' && mBoard[i][newY] != mCurrentTetrominoSymbol)) {
                        allowed = false;
                    }
                }
            }
        }
        if(allowed) {
        for (int i = 24; i >= 0; i--)
        {
            for (int j = 9; j >= 0; j--)
            {
                if (mBoard[i][j] == mCurrentTetrominoSymbol)
                {
                    int newY = j + 1;
                    if (newY < 25 && mBoard[i][newY] == 'X')
                    {
                        mBoard[i][newY] = mCurrentTetrominoSymbol;
                        mBoard[i][j] = 'X';
                    }
                    else
                    {
                    }
                }
            }
        }
        }
        moved = allowed;
    }
    if (e.type == SDL_KEYDOWN && ( e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_LEFT) )
    {
        bool allowed = true;
        for (int i = 24; i >= 0; i--)
        {
            for (int j = 9; j >= 0; j--)
            {
                if (mBoard[i][j] == mCurrentTetrominoSymbol)
                {
                    int newY = j - 1;
                    if(newY < 0 || ( mBoard[i][newY] != 'X' && mBoard[i][newY] != mCurrentTetrominoSymbol)) {
                        allowed = false;
                    }
                }
            }
        }
        if(allowed) {
        for (int i = 0; i <= 24; i++)
        {
            for (int j = 0; j <= 9; j++)
            {
                if (mBoard[i][j] == mCurrentTetrominoSymbol)
                {
                    int newY = j - 1;
                    if (newY >= 0 && mBoard[i][newY] == 'X')
                    {
                        mBoard[i][newY] = mCurrentTetrominoSymbol;
                        mBoard[i][j] = 'X';
                    }
                    else
                    {
                    }
                }
            }
        }
        }
        moved = allowed;
    }
    if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r) {
        moved = rotateTetromino();
    }
    if(mCollision) {
    }
    }
    if(moved) {
        mCollision = false;
    }
    return moved;
}

void Game::moveTetrominoDown() {
    if(!mGameOver) {
     bool allowed = true;
        for (int i = 24; i >= 0; i--)
        {
            for (int j = 9; j >= 0; j--)
            {
                if (mBoard[i][j] == mCurrentTetrominoSymbol)
                {
                    int newX = i + 1;
                    if(newX > 24 || ( mBoard[newX][j] != 'X' && mBoard[newX][j] != mCurrentTetrominoSymbol)) {
                        allowed = false;
                    }
                }
            }
        }
        if(allowed) {
        for (int i = 24; i >= 0; i--)
            {
                for (int j = 9; j >= 0; j--)
                {
                    if(mBoard[i][j] == mCurrentTetrominoSymbol) {
                        int newX = i + 1;
                        mBoard[newX][j] = mCurrentTetrominoSymbol;
                        mBoard[i][j] = 'X';
                    }
                }
            }
        }
        else {
            mCollision = true;
        }
    }
}
bool Game::rotateTetromino() {
    if(mCurrentTetrominoSymbol == 'O' || gDrawTetromino) {
        return false;
    }
    int previousHeight = mCurrentTetromino.getShape().size();
    int previousWidth = mCurrentTetromino.getShape()[0].size();

    mCurrentTetromino.rotate();

    int tetrominoBeginningPointX = -1;
    int tetrominoBeginningPointY = -1;
    int tetrominoRow = -1;
    for (int i = 0; i <= 24; i++)
    {
        for (int j = 0; j <= 9; j++)
        {
            if(mBoard[i][j] == mCurrentTetrominoSymbol) {
                tetrominoBeginningPointX = i;
                tetrominoBeginningPointY = j;
                tetrominoRow = i;
                break;
            }
        }
        if(tetrominoBeginningPointX != -1) {
            break;
        }
    }
    int currentHeight = mCurrentTetromino.getShape().size();
    int currentWidth = mCurrentTetromino.getShape()[0].size();

    int tetrominoCenterX = tetrominoBeginningPointX + mCurrentTetromino.getShape().size() / 2;
    int tetrominoCenterY = tetrominoBeginningPointY + mCurrentTetromino.getShape()[0].size() / 2;

    if(previousHeight == 4 && previousWidth == 1 && currentHeight == 1 && currentWidth == 4 && mCurrentTetrominoSymbol == 'I') {
        tetrominoBeginningPointX = tetrominoBeginningPointX + 1;
        tetrominoBeginningPointY = tetrominoBeginningPointY - 1;
        
    }
    else if(previousHeight == 1 && previousWidth == 4 && currentHeight == 4 && currentWidth == 1 && mCurrentTetrominoSymbol == 'I') {
        tetrominoBeginningPointX = tetrominoBeginningPointX - 1;
        tetrominoBeginningPointY = tetrominoBeginningPointY + 1;
    }
    else if(previousHeight == 2 && previousWidth == 3 && currentHeight == 3 && currentWidth == 2 && mCurrentTetrominoSymbol == 'S') {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY;
    }
    else if(previousHeight == 3 && previousWidth == 2 && currentHeight == 2 && currentWidth == 3 && mCurrentTetrominoSymbol == 'S') {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY - 1;
    }
    else if(previousHeight == 2 && previousWidth == 3 && currentHeight == 3 && currentWidth == 2 && mCurrentTetrominoSymbol == 'Z') {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY + 1;
    }
    else if(previousHeight == 3 && previousWidth == 2 && currentHeight == 2 && currentWidth == 3 && mCurrentTetrominoSymbol == 'Z') {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY - 2;
    }
    else if(previousHeight == 3 && previousWidth == 2 && currentHeight == 2 && currentWidth == 3 && mCurrentTetrominoSymbol == 'J' && mRotation == 0) {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY - 1;
    }
    else if(previousHeight == 2 && previousWidth == 3 && currentHeight == 3 && currentWidth == 2 && mCurrentTetrominoSymbol == 'J' && mRotation == 1) {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY + 1;
    }
    else if(previousHeight == 3 && previousWidth == 2 && currentHeight == 2 && currentWidth == 3 && mCurrentTetrominoSymbol == 'J' && mRotation == 2) {
        tetrominoBeginningPointX = tetrominoBeginningPointX + 1;
        tetrominoBeginningPointY = tetrominoBeginningPointY - 1;
    }
    else if(previousHeight == 2 && previousWidth == 3 && currentHeight == 3 && currentWidth == 2 && mCurrentTetrominoSymbol == 'J' && mRotation == 3) {
        tetrominoBeginningPointX = tetrominoBeginningPointX - 1;
        tetrominoBeginningPointY = tetrominoBeginningPointY;
    }
    else if(previousHeight == 3 && previousWidth == 2 && currentHeight == 2 && currentWidth == 3 && mCurrentTetrominoSymbol == 'L' && mRotation == 0) {
        tetrominoBeginningPointX = tetrominoBeginningPointX + 1;
        tetrominoBeginningPointY = tetrominoBeginningPointY - 1;
    }
    else if(previousHeight == 2 && previousWidth == 3 && currentHeight == 3 && currentWidth == 2 && mCurrentTetrominoSymbol == 'L' && mRotation == 1) {
        tetrominoBeginningPointX = tetrominoBeginningPointX - 1;
        tetrominoBeginningPointY = tetrominoBeginningPointY;
    }
    else if(previousHeight == 3 && previousWidth == 2 && currentHeight == 2 && currentWidth == 3 && mCurrentTetrominoSymbol == 'L' && mRotation == 2) {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY;
    }
    else if(previousHeight == 2 && previousWidth == 3 && currentHeight == 3 && currentWidth == 2 && mCurrentTetrominoSymbol == 'L' && mRotation == 3) {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY - 1;
    }
    else if(previousHeight == 2 && previousWidth == 3 && currentHeight == 3 && currentWidth == 2 && mCurrentTetrominoSymbol == 'T' && mRotation == 0) {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY;
    }
    else if(previousHeight == 3 && previousWidth == 2 && currentHeight == 2 && currentWidth == 3 && mCurrentTetrominoSymbol == 'T' && mRotation == 1) {
        tetrominoBeginningPointX = tetrominoBeginningPointX + 1;
        tetrominoBeginningPointY = tetrominoBeginningPointY - 1;
    }
    else if(previousHeight == 2 && previousWidth == 3 && currentHeight == 3 && currentWidth == 2 && mCurrentTetrominoSymbol == 'T' && mRotation == 2) {
        tetrominoBeginningPointX = tetrominoBeginningPointX - 1;
        tetrominoBeginningPointY = tetrominoBeginningPointY;
    }
    else if(previousHeight == 3 && previousWidth == 2 && currentHeight == 2 && currentWidth == 3 && mCurrentTetrominoSymbol == 'T' && mRotation == 3) {
        tetrominoBeginningPointX = tetrominoBeginningPointX;
        tetrominoBeginningPointY = tetrominoBeginningPointY - 1;
    }

    int borderCollision = 0;
    bool canRotate = true;
    for(int i = 0 ; i < mCurrentTetromino.getShape().size() ; i++) {
        for(int j = 0 ; j < mCurrentTetromino.getShape()[0].size() ; j++) {
            if(i + tetrominoBeginningPointX < 0 || i + tetrominoBeginningPointX > 24 || j + tetrominoBeginningPointY < 0 || j + tetrominoBeginningPointY > 9 || ( mBoard[i + tetrominoBeginningPointX][j + tetrominoBeginningPointY] != 'X' && mBoard[i + tetrominoBeginningPointX][j + tetrominoBeginningPointY] != mCurrentTetrominoSymbol)) {
                if(j + tetrominoBeginningPointY < 0) {
                    borderCollision = -1;
                }
                else if(j + tetrominoBeginningPointY > 9) {
                    borderCollision = 1;
                }
                canRotate = false;
            }
        }
    }
    if(canRotate) {
    for(int i = 0 ; i < 25 ; i++) {
        for(int j = 0 ; j < 10 ; j++) {
            if(mBoard[i][j] == mCurrentTetrominoSymbol) {
                mBoard[i][j] = 'X';
            }
        }
    }
    for(int i = 0 ; i < mCurrentTetromino.getShape().size() ; i++) {
        for(int j = 0 ; j < mCurrentTetromino.getShape()[0].size() ; j++) {
            if(i + tetrominoBeginningPointX > -1 && i + tetrominoBeginningPointX < 25 && j + tetrominoBeginningPointY > -1 && j + tetrominoBeginningPointY < 25) {
                mBoard[i + tetrominoBeginningPointX ][j + tetrominoBeginningPointY] = mCurrentTetromino.getShape()[i][j];
            }
        }
    }

    mRotation++;
    if(mRotation > 3) {
        mRotation = 0;
    }
    }
    else {
        if(borderCollision) {
            if(borderCollision == 1)
                tetrominoBeginningPointY = tetrominoBeginningPointY - 1;
            else
                tetrominoBeginningPointY = tetrominoBeginningPointY + 1;
            for(int i = 0 ; i < 25 ; i++) {
                for(int j = 0 ; j < 10 ; j++) {
                if(mBoard[i][j] == mCurrentTetrominoSymbol) {
                    mBoard[i][j] = 'X';
                }  
            }
        }
            for(int i = 0 ; i < mCurrentTetromino.getShape().size() ; i++) {
                for(int j = 0 ; j < mCurrentTetromino.getShape()[0].size() ; j++) {
                    if(i + tetrominoBeginningPointX > -1 && i + tetrominoBeginningPointX < 25 && j + tetrominoBeginningPointY > -1 && j + tetrominoBeginningPointY < 25) {
                        mBoard[i + tetrominoBeginningPointX ][j + tetrominoBeginningPointY] = mCurrentTetromino.getShape()[i][j];
                    }
                }
            }

            mRotation++;
            if(mRotation > 3) {
                mRotation = 0;
            }
            canRotate = true;
        }
        else {
            mCurrentTetromino.rotate();
        }
    }
    if(canRotate) {
        mCollision = false;
    }
    return canRotate;
}


/*void Game::startGame() {
    
    while(input != '.') {
        
        std::cout << std::boolalpha << finished << std::endl;
        this->display();
        std::cout << "Score: " << mScore << std::endl;
        std::cout << "Lines Cleared: " << mLinesCleared << std::endl;
        std::cin >> input;
        finished = moveTetromino(input);
        checkScore();
        
    }
}*/

void Game::checkScore() {
    int linesCleared = 0;
    for(int i = 24 ; i >= 0 ; i--) {
        bool lineCompleted = true;
        for(int j = 0 ; j < 10 ; j++) {
            if(mBoard[i][j] == 'X') {
                lineCompleted = false;
                break;
            }
        }
        if(lineCompleted) {
                Mix_Volume(-1 , MIX_MAX_VOLUME);
                Mix_PlayChannel(-1 , gLineClearSound , 0);
            mLinesClearedIndexes.push_back(i);
            linesCleared++;
            mLinesCleared++;
            /*for(int z = i; z >= 1 ; z--) {
                mBoard[z] = mBoard[z - 1];
            }*/
        }

    }

    
    /*for(int i = 24 ; i >= 0 ; i--) {
        if(mBoard[i][0] == '*') {
            for(int j = i ; j >= 1 ; j--) {
                for(int z = 0 ; z < 10 ; z++) {
                    mBoard[j][z] = mBoard[j - 1][z];
                }
                Mix_PlayChannel(-1 , gLineClearSound , 0);
            }
        }
        if(mBoard[i][0] == '*') {
            i++;
        }
    }*/

    if(linesCleared == 1) {
        mScore += 40;
    }
    else if(linesCleared == 2) {
        mScore += 100;
    }
    else if(linesCleared == 3) {
        mScore += 300;
    }
    else if(linesCleared == 4) {
        mScore += 1200;
    }
    if(mLinesClearedIndexes.size() > 0) {
    std::cout << "Size is indeed greater than 1 " << std::endl;
    clearLines();
    }
}

void Game::render() {
    SDL_Color color = {255,255,255,255};
    std::stringstream scoreText;
    scoreText << "Score: " << mScore;
    std::stringstream lineClearedText;
    lineClearedText << "Lines Cleared: " << mLinesCleared;
    mScoreTexture.loadFromRenderedText(scoreText.str().c_str() , color);
    mLinesClearedTexture.loadFromRenderedText(lineClearedText.str().c_str() , color);
    mScoreTexture.render(0 , 50);
    mLinesClearedTexture.render(0 , 80);
    SDL_Rect nextTetrominoRectangle = {5 , 150 , 200 , 200};
    SDL_Rect blackBackgroundRectangle = {0 , 0 , 200 , 200};
    SDL_RenderSetViewport(gRenderer , &nextTetrominoRectangle);
    SDL_SetRenderDrawColor(gRenderer , 0 , 0 , 0 , 0xff);
    SDL_RenderFillRect(gRenderer , &blackBackgroundRectangle);
    SDL_SetRenderDrawColor(gRenderer , 0xff , 0xff , 0xff , 0xff);
    LTexture nextTetrominoTexture;
    std::string nextText = "Next";
    mNextTexture.loadFromRenderedText(nextText.c_str() , color);
    mNextTexture.render((200 - mNextTexture.getWidth()) / 2 , 0);
    switch(mNextTetrominoSymbol) {
        case 'O' :
        nextTetrominoTexture.loadFromFile("assets/red_block/red_O1.png");
        break;

        case 'I' :
        nextTetrominoTexture.loadFromFile("assets/purple_block/purple_I2.png");
        break;

        case 'S' :
        nextTetrominoTexture.loadFromFile("assets/yellow_block/yellow_S1.png");
        break;

        case 'Z' :
        nextTetrominoTexture.loadFromFile("assets/cyan_block/cyan_Z1.png");
        break;

        case 'L' :
        nextTetrominoTexture.loadFromFile("assets/green_block/green_L2.png");
        break;

        case 'J' :
        nextTetrominoTexture.loadFromFile("assets/pink_block/pink_J4.png");
        break;

        case 'T':
        nextTetrominoTexture.loadFromFile("assets/blue_block/blue_T1.png");
        break;

    }
    nextTetrominoTexture.render(( 200 - nextTetrominoTexture.getWidth() ) / 2 , ( 200 - nextTetrominoTexture.getHeight() ) / 2);
    SDL_RenderSetViewport(gRenderer , NULL);
    SDL_Rect gridRectangle = {200 , 50 , 320 , 800};
    LTexture blueBlock;
    // 'O' , 'I' , 'S' , 'Z' , 'L' , 'J' , 'T'
    int xCoordinate = 0 , yCoordinate = 0;
    SDL_RenderSetViewport(gRenderer , &gridRectangle);
    mBackgroundGrid.render(0,0);
    for(int i = 0 ; i < 25 ; i++) {
        for(int j = 0 ; j < 10 ; j++) {
            if(mBoard[i][j] != 'X' && mBoard[i][j] != '2') {
                switch(mBoard[i][j]) {
                    case 'O' : case 'o':
                    mRedBlock.render(xCoordinate , yCoordinate);
                    break;
                    case 'I' : case 'i':
                    mPurpleBlock.render(xCoordinate , yCoordinate);
                    break;
                    case 'S' : case 's':
                    mYellowBlock.render(xCoordinate , yCoordinate);
                    break;
                    case 'Z' : case 'z':
                    mCyanBlock.render(xCoordinate , yCoordinate);
                    break;
                    case 'L' : case 'l':
                    mGreenBlock.render(xCoordinate , yCoordinate);
                    break;
                    case 'J' : case 'j':
                    mPinkBlock.render(xCoordinate , yCoordinate);
                    break;
                    case 'T' : case 't':
                    mBlueBlock.render(xCoordinate , yCoordinate);
                    break;
                    case '*' :
                    break;
                    default:
                    mBlueBlock.render(xCoordinate , yCoordinate);
                    break;
                }
            }
            xCoordinate += 32;
        }
        yCoordinate += 32;
        xCoordinate = 0;
    }

    SDL_RenderSetViewport(gRenderer , NULL);
}
char Game::getCurrentTetrominoSymbol() {
    return mCurrentTetrominoSymbol;
}

bool Game::getCollision() {
    return mCollision;
}

void Game::setCollision(bool status) {
    mCollision = status;
}

bool Game::getGameOver() {
    return mGameOver;
}

void Game::clearLines() {
    if(mLinesClearedIndexes.size() <= 0) {
        mAnimationMode = false;
    }
    mAnimationMode = true;
    bool animationActive = false;
    int index = 0;
    for(int a : mLinesClearedIndexes) {
        bool lineCleared = true;
        int firstRandom = rand() % 10;
        int secondRandom = rand() % 10;
        int firstRandNumber = 0 , secondRandNumber = 0;
        firstRandom = rand() % 10;
        secondRandom = rand() % 10;
        while(mBoard[a][firstRandom] == '*') {
            firstRandom = firstRandNumber;
            firstRandNumber++;
        }
        while(mBoard[a][secondRandom] == '*') {
            secondRandom = secondRandNumber;
            secondRandNumber++;
        }
        mBoard[a][firstRandom] = '*';
        mBoard[a][secondRandom] = '*';
        for(int j = 0 ; j < 10 ; j++) {
            if(mBoard[a][j] != '*') {
                lineCleared = false;
                std::cout << "Condition activated" << std::endl;
                animationActive = true;
            }
        }
        if(lineCleared) {
            mLinesClearedIndexes.erase(mLinesClearedIndexes.begin() + index);
        }
        index++;
    }
    mAnimationMode = animationActive;

    if(!mAnimationMode) {
        for(int i = 24 ; i >= 0 ; i--) {
        if(mBoard[i][0] == '*') {
            for(int j = i ; j >= 1 ; j--) {
                for(int z = 0 ; z < 10 ; z++) {
                    mBoard[j][z] = mBoard[j - 1][z];
                }
            }
        }
        if(mBoard[i][0] == '*') {
            i++;
        }
    }
    }

}

bool Game::getAnimationMode() {
    return mAnimationMode;
}

void Game::displayGameOver() {
    int gameOverBoxWidth = 500 , gameOverBoxHeight = 500;
    SDL_Rect rectangleViewport{( SCREEN_WIDTH - gameOverBoxWidth) / 2 , ( SCREEN_HEIGHT - gameOverBoxHeight) / 2 , gameOverBoxHeight , gameOverBoxWidth};
    SDL_RenderSetViewport(gRenderer , &rectangleViewport);
    SDL_SetRenderDrawColor(gRenderer , 0x00 , 0x00 , 0x00 , 0xff);
    SDL_Rect rectangleGameOver{0 , 0, gameOverBoxWidth , gameOverBoxHeight};
    SDL_RenderFillRect(gRenderer , &rectangleGameOver);
    SDL_Color color = {255 , 255 , 255 , 255};
    gFont = TTF_OpenFont("barcade.otf" , 40);
    SDL_SetRenderDrawColor(gRenderer , 0xff , 0x00 , 0x00 , 0xff);

    mGameOverRestartButton.loadFromRenderedText("Restart" , color);
    mGameOverMenuButton.loadFromRenderedText("Main Menu" , color);
    mGameOverQuitButton.loadFromRenderedText("Quit" , color);

    SDL_Rect rectangleOption;
    switch(mCurrentOptionGameOver) {
        case 0 : {
            rectangleOption = {( ( gameOverBoxWidth - mGameOverRestartButton.getWidth() ) / 2 ) - 10, 50 , mGameOverRestartButton.getWidth() + 20 , mGameOverRestartButton.getHeight() + 10};
        } break;
        case 1 : {
            rectangleOption = {( ( gameOverBoxWidth - mGameOverMenuButton.getWidth() ) / 2 ) - 10 , 200 , mGameOverMenuButton.getWidth() + 20, mGameOverMenuButton.getHeight() + 20};
        } break;
        case 2 : {
            rectangleOption = {( ( gameOverBoxWidth - mGameOverQuitButton.getWidth() ) / 2) - 10, 350 , mGameOverQuitButton.getWidth() + 20 , mGameOverQuitButton.getHeight() + 10};
        } break;
    }

    SDL_RenderFillRect(gRenderer , &rectangleOption);

    mGameOverRestartButton.render(( gameOverBoxWidth - mGameOverRestartButton.getWidth() ) / 2 , 50);

    mGameOverMenuButton.render(( gameOverBoxWidth - mGameOverMenuButton.getWidth() ) / 2 , 200);

    mGameOverQuitButton.render(( gameOverBoxWidth - mGameOverQuitButton.getWidth() ) / 2 , 350);

    gFont = TTF_OpenFont("barcade.otf" , 20);
    SDL_SetRenderDrawColor(gRenderer , 0xff , 0xff , 0xff , 0xff);
    SDL_RenderSetViewport(gRenderer , NULL);

}

bool Game::getRestartRequest() {
    return mRestartRequest;
}

bool Game::getGamePaused() {
    return mGamePaused;
}

void Game::setGamePause() {
    mGamePaused = !mGamePaused;
    return;
}

int Game::getCurrentGameOption() {
    return mCurrentOptionGameOver;
}

void Game::setCurrentGameOption(int number) {
    mCurrentOptionGameOver = number;
}

void Game::setRestartRequest(bool value) {
    mRestartRequest = value;
}

bool Game::getMoveLock() {
    return mMoveLock;
}

void Game::setMoveLock(bool lock) {
    mMoveLock = lock;
}

bool Game::getTimeoutCollision() {
    return mTimeoutCollision;
}

void Game::setTimeoutCollision(bool timeoutCollision) {
    mTimeoutCollision = timeoutCollision;
}

void Game::finalizePosition() {
    for (int i = 0; i <= 24; i++)
        {
            for (int j = 0; j <= 9; j++)
            {
                if(mBoard[i][j] == mCurrentTetrominoSymbol) {
                    if(i < 5) {
                        mGameOver = true;
                    }
                    switch(mBoard[i][j]) {
                    case 'O' :
                    mBoard[i][j] = 'o';
                    break;
                    case 'I' :
                    mBoard[i][j] = 'i';
                    break;
                    case 'S' :
                    mBoard[i][j] = 's';
                    break;
                    case 'Z' :
                    mBoard[i][j] = 'z';
                    break;
                    case 'L' :
                    mBoard[i][j] = 'l';
                    break;
                    case 'J' :
                    mBoard[i][j] = 'j';
                    break;
                    case 'T' :
                    mBoard[i][j] = 't';
                    }
                }
            }
        }
    checkScore();   
}