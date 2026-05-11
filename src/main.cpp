#include "WandAllegroEngine/Managers/GameManager.hpp"
#include "Scenes/MainScene.hpp"

using namespace WandEngine;

int main(){

	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	if(GameManager::GetInstance().Init()){
		GameManager::GetInstance().SetWindows_Size(WAND_SIZE(ScreenWidth, ScreenHeight));
		GameManager::GetInstance().SetFirstScene(new MainScene);
		GameManager::GetInstance().Update();
		GameManager::GetInstance().Clear();
	}
	else{
		std::cerr << "ERROR" << std::endl;
	}

	std::cout << "END PROGRAM SUCCESFULLY MAIN" << std::endl;

	return 0;
}