#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>

#include "GlobalVariables.h"
#include "Tetromino.h"
#include "Game.h"

const int SCREEN_WIDTH = 700 , SCREEN_HEIGHT = 900 ;
SDL_Renderer* gRenderer = NULL;
SDL_Window* gWindow = NULL;
TTF_Font* gFont = NULL;
Mix_Music* gMainStageMusic = NULL;
Mix_Chunk* gLineClearSound = NULL;
Uint64 timer;
Uint64 gAnimationTimer = 0;
bool quit = false;
bool gDrawTetromino = true;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
                else {
                    if(TTF_Init() == -1) {
                        printf("TTF Initialization Error! %s\n" , TTF_GetError());
                        success = false;
                    }
                    if( Mix_OpenAudio(44100 , MIX_DEFAULT_FORMAT , 2 , 2048) < 0) {
                        printf("SDL Mixer isnt working buddy %s\n" , Mix_GetError());
                        success = false;
                    }
                }
			}
		}
	}

    return success;
}

void close()
{
	TTF_CloseFont( gFont );
	gFont = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool loadMedia() {
	gMainStageMusic = Mix_LoadMUS("music/music1.mp3");
	gLineClearSound = Mix_LoadWAV("music/lineclear.mp3");
	gFont = TTF_OpenFont("barcade.otf" , 20);
    bool success = true;
    return success;
}

int main(int argc , char* args[]) {
    if(init()) {
        if(loadMedia()) {
			std::srand(time(0));
            SDL_Event e;

            char input;
            bool movementRecorded = false;
			bool animationModeTimeout = false;
			bool oneTimeActivation = false;
			bool oneTimeSound = false;

			Uint32 timerLimit = 0;
			bool lockDelay = false;
			Uint32 lockDelayTimer = 1;

			bool collided = false;

			bool themeChoice = false;

			int maxMusicChoice;
			std::vector<std::string> allSongs;
			std::vector<std::string> currentSongs;

			int finalThemeChoice;

            Game* game = NULL;
			timer = 0;
			int currentThemeChoice = 0;
			int currentMenuChoice = 0;
			Uint32 timedMovement = 0;
			SDL_Color textColor = {255 , 255 , 255 , 255};

			LTexture backgroundTexture;


			gFont = TTF_OpenFont("modern-tetris.ttf" , 38);
			LTexture logoTexture;
			logoTexture.loadFromFile("assets/logo.png");
			
			LTexture mainMenuBackgroundTexture;
			mainMenuBackgroundTexture.loadFromFile("assets/Menu Background.jpg");

			LTexture startGameButton;
			startGameButton.loadFromRenderedText("Play" , textColor);

			LTexture quitGameButton;
			quitGameButton.loadFromRenderedText("Quit Game" , textColor);

			LTexture chooseThemeTexture;
			chooseThemeTexture.loadFromRenderedText("Choose A Theme" , textColor);

			LTexture cairokeeThemeButton;
			cairokeeThemeButton.loadFromRenderedText("Cairokee" , textColor);

			LTexture arcadeThemeButton;
			arcadeThemeButton.loadFromRenderedText("Arcade" , textColor);

			gFont = TTF_OpenFont("barcade.otf" , 20);
            while(!quit) {
				timer = SDL_GetTicks();
                while(SDL_PollEvent(&e) != 0) {
					if(game == NULL && themeChoice) {
						if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
							currentThemeChoice++;
							if(currentThemeChoice > 1) {
								currentMenuChoice = 0;
							}
						}
						else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
							currentThemeChoice--;
							if(currentThemeChoice < 0) {
								currentThemeChoice = 1;
							}
						}
						else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
							switch(currentThemeChoice) {
								case 0: {
								game = new Game();
								backgroundTexture.loadFromFile("assets/cairokee_background.jpg");
								std::string songText;
								std::ifstream fileReader("text/cairokee_choices.txt");
								while(std::getline(fileReader , songText)) {
									allSongs.push_back(songText);
									currentSongs.push_back(songText);
								}
								themeChoice = false;
								finalThemeChoice = 0;
								}
								break;
								case 1:
								game = new Game();
								backgroundTexture.loadFromFile("assets/Background.jpg");
								std::string songText;
								std::ifstream fileReader("text/arcade_choices.txt");
								while(std::getline(fileReader , songText)) {
									allSongs.push_back(songText);
									currentSongs.push_back(songText);
								}
								std::cout << std::endl;
								themeChoice = false;
								finalThemeChoice = 1;
								currentThemeChoice = 0;backgroundTexture.render(-500 , -50);
								break;
							}
						}
					}
					if(game == NULL && !themeChoice) {
						if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
							currentMenuChoice++;
							if(currentMenuChoice > 1) {
								currentMenuChoice = 0;
							}
						}
						else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
							currentMenuChoice--;
							if(currentMenuChoice < 0) {
								currentMenuChoice = 1;
							}
						}
						else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
							switch(currentMenuChoice) {
								case 0: {
								themeChoice = true;
								break;
								// playButtongDrawTetromino = true;
								}
								break;
								case 1:
								quit = true;
								break;
							}
						}
					}
					if(game != NULL) {
					if(!game->getAnimationMode() && !game->getGamePaused() && !game->getGameOver())
                    	movementRecorded = game->moveTetromino(e);
					else if(game->getGamePaused() || game->getGameOver()){
						int currentOption = game->getCurrentGameOption();
						if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_DOWN) {
            				game->setCurrentGameOption(currentOption + 1);
            				if(currentOption + 1 > 2) {
                				game->setCurrentGameOption(0);
            				}
        				}
        				else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_UP) {
            				game->setCurrentGameOption(currentOption - 1);
            			if(currentOption - 1 < 0) {
                			game->setCurrentGameOption(2);
            			}
        				}
        				else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
            				switch(currentOption) {
                				case 0: {
                    			delete game;
								game = NULL;
								game = new Game();
								gDrawTetromino = true;
                				} break;
                				case 1: {
								delete game;
								game = NULL;
								gDrawTetromino = true;
								allSongs.clear();
								currentSongs.clear();
								Mix_HaltMusic();
                				} break;
                				case 2: {
                    			quit = true;
								Mix_HaltMusic();
                				} break;
            				}
        				}
					}
					if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
						game->setGamePause();
					}
					}
                    if(e.type == SDL_QUIT) {
                        quit = true;
                    }
                }

				if(game != NULL) {
				std::cout << std::boolalpha << lockDelay << std::endl;
				if(Mix_PlayingMusic() == 0) {
					std::cout << "Entered" << std::endl;
					int currentSongsSize = currentSongs.size();
					int randomChoice = rand() % currentSongsSize;
					std::string chosenSong = currentSongs[randomChoice];
					gMainStageMusic = Mix_LoadMUS(chosenSong.c_str());
					Mix_PlayMusic(gMainStageMusic , 0);
					currentSongs.erase(currentSongs.begin() + randomChoice);
					if(currentSongs.size() == 0) {
						for(std::string element : allSongs) {
							std::string elementCopy = element;
							currentSongs.push_back(elementCopy);
						}
					}
				}
				if(game->getRestartRequest()) {
					SDL_RenderSetViewport(gRenderer , NULL);
					Mix_HaltMusic();
					delete game;
					game = new Game();
				}
				if(game->getAnimationMode()) {
					if(gAnimationTimer == 0) {
						gAnimationTimer = timer;
						animationModeTimeout = true;
					}
					if(timer - gAnimationTimer > 20) {
						oneTimeActivation = true;
						gAnimationTimer = 0;
					}
				}
		
				if(!game->getGamePaused() && Mix_PausedMusic() == 1) {
					Mix_ResumeMusic();
				}
				if(!game->getAnimationMode() && !game->getGamePaused() && game != NULL) {
				if(gDrawTetromino) {
                    game->drawTetromino();
					gDrawTetromino = false;
                    game->setCollision(false);
                }
				if(game->getCollision()) {
					if(movementRecorded) {
						lockDelayTimer = 1;
						timerLimit = 0;
					}
					else if(lockDelayTimer > timerLimit) {
						lockDelayTimer = timer;
						timerLimit = lockDelayTimer + 500;
					}
					else {
						if(timer - lockDelayTimer > 500) {
							game->finalizePosition();
							gDrawTetromino = true;
						}
					
					}
				
				}
                SDL_SetRenderDrawColor(gRenderer , 0xff , 0xff , 0xff , 0xff);
                SDL_RenderClear(gRenderer);
				if(finalThemeChoice == 0) {
					backgroundTexture.render(-500 , -50);
				}
				else {
					backgroundTexture.render(-500 , -50);
				}
				
                SDL_Rect rectangle = {200 , 50 , 320 , 800};
                SDL_SetRenderDrawColor(gRenderer , 0 , 0 , 0 , 255);
                SDL_RenderFillRect(gRenderer , &rectangle);
                game->render();
				if(timer - timedMovement > 500) {
					game->moveTetrominoDown();
					timedMovement = timer;
				}
				if(game->getGameOver() || game->getGamePaused()) {
					if(game->getGamePaused()) {
						Mix_PauseMusic();
					}
					game->displayGameOver();
					
				}
                SDL_RenderPresent(gRenderer);
				}
				else {
					SDL_RenderClear(gRenderer);
					if(finalThemeChoice == 0) {
					backgroundTexture.render(-500 , -50);
				}
				else {
					backgroundTexture.render(-500 , -50);
				}
                	SDL_Rect rectangle = {200 , 50 , 320 , 800};
                	SDL_SetRenderDrawColor(gRenderer , 0 , 0 , 0 , 255);
					if(oneTimeActivation) {
						game->clearLines();
						oneTimeActivation = false;
					}
                	SDL_RenderFillRect(gRenderer , &rectangle);
                	game->render();
					if(game->getGamePaused()) {
					Mix_PauseMusic();
					game->displayGameOver();
					}
					SDL_RenderPresent(gRenderer);
				}
				}
				else if(!themeChoice){
					SDL_RenderClear(gRenderer);
					SDL_SetRenderDrawColor(gRenderer , 255 , 255 , 255 , 255);
					mainMenuBackgroundTexture.render(-50 , -50);
					SDL_Rect choiceRectangle;
					switch(currentMenuChoice) {
						case 0: 
						choiceRectangle = {( (SCREEN_WIDTH - startGameButton.getWidth()) / 2 ) - 25, ( ( SCREEN_HEIGHT - startGameButton.getHeight() ) / 2 ) - 200 , startGameButton.getWidth() + 50 , startGameButton.getHeight()};
						break;
						case 1:
						choiceRectangle = {( (SCREEN_WIDTH - quitGameButton.getWidth()) / 2 ) - 25, ( ( SCREEN_HEIGHT - quitGameButton.getHeight() ) / 2 ) - 100 , quitGameButton.getWidth() + 50 , quitGameButton.getHeight()};
						break;
					}
					SDL_SetRenderDrawColor(gRenderer , 0 , 0 , 255 , 255);
					SDL_RenderDrawRect(gRenderer , &choiceRectangle);
					SDL_SetRenderDrawColor(gRenderer , 255 , 255 , 255 , 255);
					logoTexture.render((SCREEN_WIDTH - logoTexture.getWidth() )/ 2 , ( ( SCREEN_HEIGHT - logoTexture.getHeight() ) / 2 ) - 320);
					startGameButton.render( (SCREEN_WIDTH - startGameButton.getWidth()) / 2 , ( ( SCREEN_HEIGHT - startGameButton.getHeight() ) / 2 ) - 200);
					quitGameButton.render( (SCREEN_WIDTH - quitGameButton.getWidth()) / 2 , ( ( SCREEN_HEIGHT - quitGameButton.getHeight() ) / 2 ) - 100);
					SDL_RenderPresent(gRenderer);
				}
				else {
					SDL_RenderClear(gRenderer);
					SDL_SetRenderDrawColor(gRenderer , 255 , 255 , 255 , 255);
					mainMenuBackgroundTexture.render(-50 , -50);
					SDL_Rect choiceRectangle;
					switch(currentThemeChoice) {
						case 0: 
						choiceRectangle = {( (SCREEN_WIDTH - cairokeeThemeButton.getWidth()) / 2 ) - 25, ( ( SCREEN_HEIGHT - cairokeeThemeButton.getHeight() ) / 2 ) - 200 , cairokeeThemeButton.getWidth() + 50 , cairokeeThemeButton.getHeight()};
						break;
						case 1:
						choiceRectangle = {( (SCREEN_WIDTH - arcadeThemeButton.getWidth()) / 2 ) - 25, ( ( SCREEN_HEIGHT - arcadeThemeButton.getHeight() ) / 2 ) - 100 , arcadeThemeButton.getWidth() + 50 , arcadeThemeButton.getHeight()};
						break;
					}
					SDL_SetRenderDrawColor(gRenderer , 0 , 0 , 255 , 255);
					SDL_RenderDrawRect(gRenderer , &choiceRectangle);
					SDL_SetRenderDrawColor(gRenderer , 255 , 255 , 255 , 255);
					chooseThemeTexture.render((SCREEN_WIDTH - chooseThemeTexture.getWidth() )/ 2 , ( ( SCREEN_HEIGHT - chooseThemeTexture.getHeight() ) / 2 ) - 320);
					cairokeeThemeButton.render( (SCREEN_WIDTH - cairokeeThemeButton.getWidth()) / 2 , ( ( SCREEN_HEIGHT - cairokeeThemeButton.getHeight() ) / 2 ) - 200);
					arcadeThemeButton.render( (SCREEN_WIDTH - arcadeThemeButton.getWidth()) / 2 , ( ( SCREEN_HEIGHT - arcadeThemeButton.getHeight() ) / 2 ) - 100);
					SDL_RenderPresent(gRenderer);

				}
            }
            close();
        }
    }
    return EXIT_SUCCESS;
}
