#include <WandEngine/Managers/GameManager.hpp>
#include <WandEngine/Utils/Configurations.hpp>
#include <WandEngine/Utils/Log.hpp>
#include "Scenes/SecondScene3D.hpp"

using namespace WandEngine;

int main()
{
	RUNTIME_INFO("********************************PROGRAM START********************************");

	if(!GameManager::GetInstance().Init(GAME_INITIAL_SCREEN_WIDTH, GAME_INITIAL_SCREEN_HEIGHT, GAME_BASE_RENDER_SCREEN_WIDTH, GAME_BASE_RENDER_SCREEN_HEIGHT))
		return 0;

	GameManager::GetInstance().SetFirstScene(new SecondScene3D);
	GameManager::GetInstance().Update();
	GameManager::GetInstance().Shutdown();
	
	RUNTIME_INFO("********************************END PROGRAM SUCCESSFULLY********************************");

	return 0;
}