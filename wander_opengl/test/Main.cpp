#include <iostream>
#include "Game.hpp"
#include "YGame.hpp"
#include "ZGame.hpp"

int main(int argc, char** argv)
{
	//Game game;
	//if (!game.Initialize()) {
	//	printf("error: failed to initalize game\n");
	//	return -1;
	//}
	//game.RunLoop();
	//game.Shutdown();

	YGame game;
	//ZGame game;
	if (!game.Initialize()) {
		printf("error: failed to initalize game\n");
		return -1;
	}
	game.RunLoop();
	game.Shutdown();
	
	return 0;
}