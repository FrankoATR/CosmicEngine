#include "WandAllegroEngine/Managers/GameManager.hpp"
#include "Scenes/MainScene.hpp"

int main(){

	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	if(WandEngine::GameManager::GetInstance().Init()){
		WandEngine::GameManager::GetInstance().SetWindows_Size(Size(ScreenWidth, ScreenHeight));
		WandEngine::GameManager::GetInstance().SetFirstScene(new MainScene);
		WandEngine::GameManager::GetInstance().Update();
		WandEngine::GameManager::GetInstance().Clear();
	}
	else{
		std::cerr << "ERROR" << std::endl;
	}

	std::cout << "END PROGRAM SUCCESFULLY MAIN" << std::endl;

	return 0;
}