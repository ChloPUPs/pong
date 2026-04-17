default:
	g++ src/main.cpp -o bin/pong -Wall -Werror -std=c++20 -O1 -L ./lib -I ./include -lraylib -lgdi32 -lwinmm
release:
	g++ src/main.cpp -o bin/pong -Wall -Werror -std=c++20 -O1 -L ./lib -I ./include -lraylib -lgdi32 -lwinmm -mwindows
