#include <WandEngine/Managers/GameManager.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include "Scenes/MainScene.hpp"

using namespace WandEngine;

int main(){

	std::cout << "********************************PROGRAM START********************************" << std::endl;

	int screenWidth = 960;
	int screenHeight = 540;

	int baseScreenWidth = 1920;
	int baseScreenHeight = 1080;

	if(GameManager::GetInstance().Init(screenWidth, screenHeight, baseScreenWidth, baseScreenHeight)){
		GameManager::GetInstance().SetFirstScene(new MainScene);
		GameManager::GetInstance().Update();
		GameManager::GetInstance().Shutdown();
	}
	else{
		std::cerr << "********************************ERROR********************************" << std::endl;
	}

	std::cout << "********************************END PROGRAM SUCCESSFULLY********************************" << std::endl;

	return 0;
}