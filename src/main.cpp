#include <CosmicEngine/managers/game_manager.hpp>
#include <CosmicEngine/utils/log.hpp>
#include <CosmicEngine/utils/logger.hpp>
#include "scenes/main_scene.hpp"

using namespace CosmicEngine;

int main()
{
	try
	{
		#ifdef NDEBUG
				Logger::init(false, false);
		#else
				Logger::init(true, true);
		#endif

		Logger::info("************************************* [PROGRAM START] **************************************");

		GameManagerInitParams params{
			GAME_INITIAL_SCREEN_WIDTH,
			GAME_INITIAL_SCREEN_HEIGHT,
			GAME_BASE_RENDER_SCREEN_WIDTH,
			GAME_BASE_RENDER_SCREEN_HEIGHT};

		GameManager::GetInstance().init(params);
		GameManager::GetInstance().setFirstScene(new MainScene);
		GameManager::GetInstance().update();
		GameManager::GetInstance().shutdown();

		Logger::info("******************************** [END PROGRAM SUCCESSFULLY] ********************************");
		Logger::shutdown();
		return 0;
	}
	catch (const std::exception &e)
	{
		std::cerr << "[Fatal] " << e.what() << std::endl;
		Logger::shutdown();
		return 1;
	}
}