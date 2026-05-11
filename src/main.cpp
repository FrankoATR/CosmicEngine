#include <WandEngine/Managers/GameManager.hpp>
#include <iostream>
#include "Scenes/MainScene.hpp"

using namespace WandEngine;

int main(){

	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	if(GameManager::GetInstance().Init()){
		//GameManager::GetInstance().SetWindows_Size(WAND_SIZE(1600, 900));
		GameManager::GetInstance().SetWindows_Size(WAND_SIZE(ScreenWidth, ScreenHeight));
		//GameManager::GetInstance().SetWindows_WindowsMode();
		GameManager::GetInstance().SetFirstScene(new MainScene(0, 1));
		GameManager::GetInstance().Update();
		GameManager::GetInstance().Clear();
	}
	else{
		#ifndef NDEBUG
			std::cerr << "ERROR" << std::endl;
		#endif
	}

	#ifndef NDEBUG
		std::cout << "END PROGRAM SUCCESFULLY MAIN" << std::endl;
	#endif





	return 0;
}