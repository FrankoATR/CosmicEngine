#include <CosmicEngine/managers/game_manager.hpp>
#include <CosmicEngine/utils/log.hpp>
#include <CosmicEngine/utils/logger.hpp>
#include "scenes/main_scene.hpp"

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
#include "scenes/network_demo_scene.hpp"
#endif

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
		#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
		GameManager::GetInstance().setFirstScene(new NetworkDemoScene);
		#else
		GameManager::GetInstance().setFirstScene(new MainScene);
		#endif
		GameManager::GetInstance().update();
		GameManager::GetInstance().shutdown();

		Logger::info("******************************** [END PROGRAM SUCCESSFULLY] ********************************");
		Logger::shutdown();
		return 0;
	}
	catch (const std::exception &e)
	{
		RUNTIME_WARNING("[Fatal] " << e.what());
		Logger::shutdown();
		return 1;
	}
}