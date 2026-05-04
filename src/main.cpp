#include "WandAllegroEngine/Managers/GameManager.h"

int main(){


	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	GameManager* Game = new GameManager(Size(ScreenWidth, ScreenHeight));

	if(Game->Init()){
		Game->Update();
	}
	
	delete Game;

	return 0;
}

