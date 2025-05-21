all:
	g++ -Isrc/include -Lsrc/lib -o main src/LTexture.cpp src/Tetromino.cpp src/Game.cpp src/Main.cpp  -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer