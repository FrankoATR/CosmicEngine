#include <CosmicEngine/managers/game_manager.hpp>
#include <CosmicEngine/utils/log.hpp>
#include <CosmicEngine/utils/logger.hpp>
#include "cosmic_project_config.hpp"
#include "cosmic_project_entry.hpp"

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
			ProjectConfig::kWindowTitle,
			ProjectConfig::kWindowIconPath,
			ProjectConfig::kInitialScreenWidth,
			ProjectConfig::kInitialScreenHeight,
			ProjectConfig::kBaseRenderWidth,
			ProjectConfig::kBaseRenderHeight,
			ProjectConfig::kStartFullscreen,
			ProjectConfig::kStartVsync};

		GameManager::GetInstance().init(params);
		GameManager::GetInstance().setFirstScene(CreateCosmicProjectFirstScene());
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