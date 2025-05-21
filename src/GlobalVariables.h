#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#ifndef GLOBAL_H
#define GLOBAL_H

class Stage;

extern const int SCREEN_WIDTH, SCREEN_HEIGHT ;
extern SDL_Renderer* gRenderer;
extern SDL_Window* gWindow;
extern TTF_Font* gFont;
extern Mix_Chunk* gLineClearSound;
extern Mix_Music* gMainStageMusic;

extern Uint64 timer;

extern Uint64 gAnimationTimer;

extern bool quit;

extern bool gDrawTetromino;

#endif