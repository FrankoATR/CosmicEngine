#include <WandEngine/Managers/GameManager.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include "Scenes/MainScene.hpp"

using namespace WandEngine;

int main(){
	int screenWidth = 960;
	int screenHeight = 540;
	//int screenWidth = 1920;
	//int screenHeight = 1080;
	if(GameManager::GetInstance().Init(screenWidth, screenHeight)){
		GameManager::GetInstance().SetFirstScene(new MainScene);
		GameManager::GetInstance().Update();
		GameManager::GetInstance().Clear();
	}
	else{
		#ifndef NDEBUG
			std::cerr << "ERROR" << std::endl;
		#endif
	}

	std::cout << "END PROGRAM SUCCESFULLY MAIN" << std::endl;

	return 0;
}