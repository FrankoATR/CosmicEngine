#include "WandAllegroEngine/Managers/GameManager.hpp"
#include "Scenes/MainScene.hpp"

int main(){


	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	GameManager* Game = new GameManager(Size(ScreenWidth, ScreenHeight));

	if(Game->Init()){
		Game->SetFirstScene(new MainScene(Game));
		Game->Update();
	}
	
	delete Game;

	return 0;
}

