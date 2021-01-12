#include "libs.h"
#include "Game.h"


int main()
{
	Game game("Final Project",
		1080, 1080,
		4, 4,
		false);

	//MAIN LOOP
	while (!game.getWindowShouldClose())
	{
		//UPDATE INPUT ---
		game.update();
		game.render();	
	}

	return 0;
}