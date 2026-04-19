/**
 * @file game_manager.cpp
 * @brief Implements the main engine lifetime manager and desktop runtime loop.
 */

#include "game_manager.hpp"
#include "scene/scene_manager.hpp"
#include "input/input_manager.hpp"
#include "timer/timer_manager.hpp"
#include "schedule/schedule_manager.hpp"
#include "resource/resource_manager.hpp"
#include "camera/camera_manager.hpp"
#include "audio/audio_manager.hpp"
#include "animation/animation_manager.hpp"
#include "ui/ui_manager.hpp"
#include "object/object_manager.hpp"
#include "body/body_manager.hpp"
#include "storage/sql/sql_manager.hpp"
#include "storage/json/json_manager.hpp"
#include "light/light_manager.hpp"
#include "network/network_manager.hpp"
#include "event/event_manager.hpp"
#include "../utils/logger.hpp"
#include "../utils/log.hpp"

#include <iostream>
#include <stdexcept>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace CosmicEngine
{
	GameManager &GameManager::GetInstance()
	{
		static GameManager instance;
		return instance;
	}

	void GameManager::init(const GameManagerInitParams& params)
	{
		RUNTIME_LIFECYCLE("Game manager", "created");

		if (initialized)
		{
			RUNTIME_WARNING("[GameManager] init() was called more than once; the second call was ignored.");
			return;
		}

        int screenWidth  = params.screenWidth;
        int screenHeight  = params.screenHeight;
        int baseScreenWidth = params.baseScreenWidth;
        int baseScreenHeight = params.baseScreenHeight;

		window = nullptr;
		fullScreenMode = false;
		vsyncEnabled = false;
		targetFPS = 60;
		gameticks = 20.0f;
        fpsTimer = 0.0;
        frameCount = 0;
        currentFPS = 0;
		imguiInitialized = false;

		try
		{
			setBaseAspectSize(baseScreenWidth, baseScreenHeight);

			if (glfwInit() != GLFW_TRUE)
			{
				Logger::error("Failed to initialize GLFW.");
			}

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

			window = glfwCreateWindow(screenWidth, screenHeight, "CosmicEngine", nullptr, nullptr);

			if (window == nullptr)
			{
				Logger::error("Failed to create the GLFW window.");
			}
			glfwMakeContextCurrent(window);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				Logger::error("Failed to initialize GLAD.");
			}
			glfwSwapInterval(0);


			#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION

			#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
				glEnable(GL_DEPTH_TEST);
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			#endif

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glViewport(0, 0, screenWidth, screenHeight);

			glfwSetWindowUserPointer(window, this);

			setFrameBufferSizeCallback([&](int width, int height){
				float currentAspect = (float)width / (float)height;
				int newWidth = width;
				int newHeight = height;
	
				glm::vec2 baseAspect = getBaseAspectSize();
	
				if (currentAspect > (baseAspect.x / baseAspect.y))
				{
					newWidth = (int)(height * (baseAspect.x / baseAspect.y));
				}
				else
				{
					newHeight = (int)(width / (baseAspect.x / baseAspect.y));
				}
	
				int offsetX = (width - newWidth) / 2;
				int offsetY = (height - newHeight) / 2;
				glfwSetWindowAspectRatio(window, baseAspect.x, baseAspect.y);
	
				glViewport(offsetX, offsetY, newWidth, newHeight);
			});

			{
				int framebufferWidth = 0;
				int framebufferHeight = 0;
				glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
				if (framebufferSizeCallback)
				{
					framebufferSizeCallback(framebufferWidth, framebufferHeight);
				}
			}

			setDropCallback([&](int count, const char** paths){
				for (int i = 0; i < count; i++)
				{
					pushDroppedFile(paths[i]);
				}
			});

			initManagers();

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO &io = ImGui::GetIO();
			(void)io;
			ImGui::StyleColorsDark();
			if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
			{
				Logger::error("Failed to initialize ImGui GLFW backend.");
			}
			if (!ImGui_ImplOpenGL3_Init("#version 330"))
			{
				Logger::error("Failed to initialize ImGui OpenGL backend.");
			}

			imguiInitialized = true;
			initialized = true;
			RUNTIME_LIFECYCLE("Game manager", "initialized");
		}
		catch (...)
		{
			shutdown();
			throw;
		}
	}


	void GameManager::initManagers() const
	{
		if (!AudioManager::GetInstance().init())
		{
			Logger::error("Failed to initialize AudioManager.");
		}


		InputManager::GetInstance().init(window);
		UIManager::GetInstance().init();
		EventManager::GetInstance().init();
		TimerManager::GetInstance().init();
		ScheduleManager::GetInstance().init();
		SceneManager::GetInstance().init();
		if (!ResourceManager::GetInstance().init())
		{
			Logger::error("Failed to initialize ResourceManager built-in assets.");
		}
		AnimationManager::GetInstance().init();
		ObjectManager::GetInstance().init();
		BodyManager::GetInstance().init();
		if (!SQLManager::GetInstance().init())
		{
			Logger::error("Failed to initialize SQLManager.");
		}
		JsonManager::GetInstance().init();
		LightManager::GetInstance().init();
		CameraManager::GetInstance().init(baseAspectSize);
		if (!NetworkManager::GetInstance().init())
		{
			Logger::error("Failed to initialize NetworkManager.");
		}

	}


	void GameManager::update()
	{
		const double fixedDeltaTime = 1.0 / 144.0;

		double accumulator = 0.0;
		double lastTime = glfwGetTime();
		double lastRenderTimer = lastTime;

		// The engine updates gameplay with a fixed step and renders independently at the configured presentation rate.
		while (!glfwWindowShouldClose(window))
		{
			double currentTime = glfwGetTime();
			double frameTime = currentTime - lastTime;
			lastTime = currentTime;
			accumulator += frameTime;

			while (accumulator >= fixedDeltaTime)
			{
				InputManager::GetInstance().update(window);
				AudioManager::GetInstance().update();
				TimerManager::GetInstance().update(fixedDeltaTime * (gameticks / 20.0f));
				ScheduleManager::GetInstance().update(fixedDeltaTime * (gameticks / 20.0f));
				AnimationManager::GetInstance().update(fixedDeltaTime * (gameticks / 20.0f));
				SceneManager::GetInstance().update(fixedDeltaTime * (gameticks / 20.0f));
				accumulator -= fixedDeltaTime;
			}

			double targetFrameTime = 1.0 / targetFPS;

			if (currentTime - lastRenderTimer >= targetFrameTime || vsyncEnabled)
			{
				glm::vec3 BackbufferColor(SceneManager::GetInstance().GetBackgroundColor());
				glClearColor(BackbufferColor.r, BackbufferColor.g, BackbufferColor.b, 1.0f);

				#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
					glClear(GL_COLOR_BUFFER_BIT);
				#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
				#else
					#error "[GameManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
				#endif

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				SceneManager::GetInstance().draw();
				frameCount++;

				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				glfwSwapBuffers(window);
				lastRenderTimer = glfwGetTime();
			}

			fpsTimer += frameTime;

			if (fpsTimer >= 1.0)
			{
				currentFPS = frameCount;
				frameCount = 0;
				fpsTimer = 0.0;
			}

			glfwPollEvents();
		}
	}


	void GameManager::setFrameBufferSizeCallback(std::function<void(int, int)> callback)
	{
		framebufferSizeCallback = callback;
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
			auto self = static_cast<GameManager*>(glfwGetWindowUserPointer(win));
			if (self && self->framebufferSizeCallback) {
				self->framebufferSizeCallback(w, h);
			}
		});
	}
	
	void GameManager::setDropCallback(std::function<void(int, const char**)> callback)
	{
		dropCallback = callback;
		glfwSetDropCallback(window, [](GLFWwindow* win, int count, const char** paths) {
			auto self = static_cast<GameManager*>(glfwGetWindowUserPointer(win));
			if (self && self->dropCallback) {
				self->dropCallback(count, paths);
			}
		});
	}

	void GameManager::setMousePositionCallback(std::function<void(double, double)> callback)
	{
		mousePositionCallback = callback;
		glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos)
		{
			auto self = static_cast<GameManager*>(glfwGetWindowUserPointer(win));
			if (self && self->mousePositionCallback)
			{
				self->mousePositionCallback(xpos, ypos);
			}
		});
	}
	
	void GameManager::setMouseScrollCallback(std::function<void(double, double)> callback)
	{
		mouseScrollCallback = callback;
		glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset)
		{
			auto self = static_cast<GameManager*>(glfwGetWindowUserPointer(win));
			if (self && self->mouseScrollCallback)
			{
				self->mouseScrollCallback(xoffset, yoffset);
			}
		});
	}

	std::vector<std::string> GameManager::getDroppedFiles()
	{
		std::vector<std::string> tmp = droppedfiles;
		droppedfiles.clear();
		return tmp;
	}

	void GameManager::pushDroppedFile(std::string filepath)
	{
		droppedfiles.push_back(filepath);
	}


	void GameManager::setTargetFps(double targetFPS)
	{
		this->targetFPS = targetFPS;
	}

	int GameManager::getCurrentFps() const 
	{
		return this->currentFPS;
	}

	void GameManager::setGameTicks(float gameTicks)
	{
		this->gameticks = gameTicks;
	}

	float GameManager::getCurrenGameTicks() const
	{
		return gameticks;
	}

	void GameManager::enableVsync()
	{
		glfwSwapInterval(1);
		vsyncEnabled = true;
	}

	void GameManager::disableVsync()
	{
		glfwSwapInterval(0);
		vsyncEnabled = false;
	}

	bool GameManager::isVsyncEnabled()
	{
		return vsyncEnabled;
	}

	void GameManager::toggleFullscreen()
	{
		if (fullScreenMode)
		{
			setWindowWindowMode(static_cast<int>(baseAspectSize.x), static_cast<int>(baseAspectSize.y));
		}
		else
		{
			setWindowFullScreenMode();
		}
	}

	void GameManager::setFirstScene(Scene *scene)
	{
		if (SceneManager::GetInstance().empty())
		{
			SceneManager::GetInstance().PushScene(scene);
		}
	}

	void GameManager::setWindowSize(glm::vec2 screenSize)
	{
	}

	GLFWwindow *GameManager::getWindowPtr() const
	{
		return window;
	}

	glm::vec2 GameManager::getWindowSize() const
	{
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		return glm::vec2(w, h);
	}

	void GameManager::setBaseAspectSize(int baseAspectWidth, int baseAspectHeight)
	{
		this->baseAspectSize = glm::vec2(static_cast<float>(baseAspectWidth), static_cast<float>(baseAspectHeight));
	}

	glm::vec2 GameManager::getBaseAspectSize() const
	{
		return this->baseAspectSize;
	}

	bool GameManager::isFullScreen() const
	{
		return this->fullScreenMode;
	}

	void GameManager::setWindowFullScreenMode()
	{
		const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
		glfwSetWindowPos(window, 0, 0);
		glfwSetWindowSize(window, mode->width, mode->height);
		fullScreenMode = true;
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		if (framebufferSizeCallback)
		{
			framebufferSizeCallback(w, h);
		}
		else
		{
			glViewport(0, 0, w, h);
		}
	}

	void GameManager::setWindowWindowMode(int width, int height)
	{
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowPos(window, 100, 100);
		glfwSetWindowSize(window, width, height);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		fullScreenMode = false;
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		if (framebufferSizeCallback)
		{
			framebufferSizeCallback(w, h);
		}
		else
		{
			glViewport(0, 0, w, h);
		}
	}

	void GameManager::endprogram()
	{
		glfwSetWindowShouldClose(window, true);
	}

	void GameManager::shutdown()
	{
		auto safeShutdown = [](const char *name, auto &&callable)
		{
			try
			{
				callable();
			}
			catch (const std::exception &e)
			{
				RUNTIME_WARNING("[GameManager] Shutdown failure in " << name << ": " << e.what());
			}
			catch (...)
			{
				RUNTIME_WARNING("[GameManager] Unknown shutdown failure in " << name << '.');
			}
		};

		safeShutdown("AnimationManager", [] { AnimationManager::GetInstance().shutdown(); });
		safeShutdown("BodyManager", [] { BodyManager::GetInstance().Clear(); });
		safeShutdown("ObjectManager", [] { ObjectManager::GetInstance().Clear(); });
		safeShutdown("LightManager", [] { LightManager::GetInstance().Clear(); });
		safeShutdown("ResourceManager", [] { ResourceManager::GetInstance().Clear(); });
		safeShutdown("ScheduleManager", [] { ScheduleManager::GetInstance().shutdown(); });
		safeShutdown("NetworkManager", [] { NetworkManager::GetInstance().shutdown(); });
		safeShutdown("JsonManager", [] { JsonManager::GetInstance().shutdown(); });
		safeShutdown("SQLManager", [] { SQLManager::GetInstance().shutdown(); });
		safeShutdown("SceneManager", [] { SceneManager::GetInstance().shutdown(); });
		safeShutdown("UIManager", [] { UIManager::GetInstance().shutdown(); });
		safeShutdown("InputManager", [] { InputManager::GetInstance().shutdown(); });
		safeShutdown("AudioManager", [] { AudioManager::GetInstance().shutdown(); });
		safeShutdown("TimerManager", [] { TimerManager::GetInstance().shutdown(); });

		if (imguiInitialized)
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
			imguiInitialized = false;
		}

		if (window)
		{
			glfwDestroyWindow(window);
			window = nullptr;
		}

		glfwTerminate();
		initialized = false;
		fullScreenMode = false;
		vsyncEnabled = false;

		RUNTIME_LIFECYCLE("Game manager", "shutdown");

	}

}
