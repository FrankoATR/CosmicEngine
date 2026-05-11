#include "GameManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Input/InputManager.hpp"
#include "Timer/TimerManager.hpp"
#include "Resource/ResourceManager.hpp"
#include "Camera/CameraManager.hpp"
#include "Audio/Music/MusicManager.hpp"
#include "Audio/Sound/SoundManager.hpp"
#include "Timer/TimerManager.hpp"
#include "UI/UIManager.hpp"
#include "Object/ObjectManager.hpp"
#include "Body/BodyManager.hpp"
#include "Database/DatabaseManager.hpp"
#include "Light/LightManager.hpp"
#include "Network/NetworkManager.hpp"

#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace WandEngine
{
	GameManager &GameManager::GetInstance()
	{
		static GameManager instance;
		return instance;
	}
	
	GameManager::GameManager()
	{
		std::cout << "Game manager created" << std::endl;
	}

	bool GameManager::Init(int screenWidth, int screenHeight, int baseScreenWidth, int baseScreenHeight)
	{
		window = nullptr;
		fullScreenMode = false;
		VSyncEnable = false;
		targetFPS = 60;
		gameticks = 20.0f;
        fpsTimer = 0.0;
        frameCount = 0;
        currentFPS = 0;

		SetBaseAspectSize(baseScreenWidth, baseScreenHeight);

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		GLFWmonitor *monitor = glfwGetPrimaryMonitor();

		const GLFWvidmode *mode = glfwGetVideoMode(monitor);

		window = glfwCreateWindow(screenWidth, screenHeight, "WANDENGINE", nullptr, nullptr);

		if (window == nullptr)
		{
			std::cerr << "FAILED TO CREATE GLFW WINDOW" << std::endl;
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cerr << "FAILED TO INITIALIZE GLAD" << std::endl;
			return -1;
		}
		glfwSwapInterval(0);


		#if defined(GAME_3D_CONFIGURATION)
			glEnable(GL_DEPTH_TEST);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		#endif

		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0, 0, screenWidth, screenHeight);

		glfwSetWindowUserPointer(window, this);

		
		SetFramebufferSize_Callback([&](int width, int height){
			float currentAspect = (float)width / (float)height;
			int newWidth = width;
			int newHeight = height;
	
			glm::vec2 baseAspect = GetBaseAspectSize();
	
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

		
		SetDrop_Callback([&](int count, const char** paths){
			for (int i = 0; i < count; i++)
			{
				PushDroppedFile(paths[i]);
			}
		});




		InitManagers();


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		std::cout << "Game manager initialized" << std::endl;
		return true;
	}


	bool GameManager::InitManagers() const
	{
		
		if (!MusicManager::GetInstance().Init())
		{
			std::cerr << "FAILED TO INITIALIZE MUSICMANAGER" << std::endl;
			return -1;
		}

		if (!SoundManager::GetInstance().Init())
		{
			std::cerr << "FAILED TO INITIALIZE SOUNDMANAGER" << std::endl;
			return -1;
		}

		InputManager::GetInstance().Init(window);
		UIManager::GetInstance().Init();
		TimerManager::GetInstance().Init();
		SceneManager::GetInstance().Init();
		ResourceManager::GetInstance().Init();
		ObjectManager::GetInstance().Init();
		BodyManager::GetInstance().Init();
		DataBaseManager::GetInstance().Init();
		LightManager::GetInstance().Init();
		CameraManager::GetInstance().Init(baseAspectSize);

		return true;
	}


	void GameManager::Update()
	{
		const double fixedDeltaTime = 1.0 / 144.0;

		double accumulator = 0.0;
		double lastTime = glfwGetTime();
		double lastRenderTimer = lastTime;

		while (!glfwWindowShouldClose(window))
		{
			double currentTime = glfwGetTime();
			double frameTime = currentTime - lastTime;
			lastTime = currentTime;
			accumulator += frameTime;

			while (accumulator >= fixedDeltaTime)
			{
				InputManager::GetInstance().Update(window);
				MusicManager::GetInstance().Update();
				SoundManager::GetInstance().Update();
				TimerManager::GetInstance().Update(fixedDeltaTime * (gameticks / 20.0f));
				SceneManager::GetInstance().Update(fixedDeltaTime * (gameticks / 20.0f));
				accumulator -= fixedDeltaTime;
			}

			double targetFrameTime = 1.0 / targetFPS;

			if (currentTime - lastRenderTimer >= targetFrameTime || VSyncEnable)
			{
				glm::vec3 BackbufferColor(SceneManager::GetInstance().GetBackgroundColor());
				glClearColor(BackbufferColor.r, BackbufferColor.g, BackbufferColor.b, 1.0f);

				#if defined(GAME_2D_CONFIGURATION)
					glClear(GL_COLOR_BUFFER_BIT);
				#elif defined(GAME_3D_CONFIGURATION)
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
				#else
					#error "[GameManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
				#endif

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				SceneManager::GetInstance().Draw();
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


	void GameManager::SetFramebufferSize_Callback(std::function<void(int, int)> callback)
	{
		framebufferSizeCallback = callback;
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
			auto self = static_cast<GameManager*>(glfwGetWindowUserPointer(win));
			if (self && self->framebufferSizeCallback) {
				self->framebufferSizeCallback(w, h);
			}
		});
	}
	
	void GameManager::SetDrop_Callback(std::function<void(int, const char**)> callback)
	{
		dropCallback = callback;
		glfwSetDropCallback(window, [](GLFWwindow* win, int count, const char** paths) {
			auto self = static_cast<GameManager*>(glfwGetWindowUserPointer(win));
			if (self && self->dropCallback) {
				self->dropCallback(count, paths);
			}
		});
	}

	void GameManager::SetMousePosition_Callback(std::function<void(double, double)> callback)
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
	
	void GameManager::SetMouseScroll_Callback(std::function<void(double, double)> callback)
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

	std::vector<std::string> GameManager::GetDroppedFiles()
	{
		std::vector<std::string> tmp = droppedfiles;
		droppedfiles.clear();
		return tmp;
	}

	void GameManager::PushDroppedFile(std::string filepath)
	{
		droppedfiles.push_back(filepath);
	}


	void GameManager::SetTargetFPS(double targetFPS)
	{
		this->targetFPS = targetFPS;
	}

	int GameManager::GetCurrentFPS() const 
	{
		return this->currentFPS;
	}

	void GameManager::SetGameTicks(float gameTicks)
	{
		this->gameticks = gameTicks;
	}

	float GameManager::GetCurrenGameTicks() const
	{
		return gameticks;
	}

	void GameManager::EnableVsync()
	{
		glfwSwapInterval(1);
		VSyncEnable = true;
	}

	void GameManager::DisableVsync()
	{
		glfwSwapInterval(0);
		VSyncEnable = false;
	}

	bool GameManager::IsVSyncEnable()
	{
		return VSyncEnable;
	}

	void GameManager::SetFirstScene(Scene *scene)
	{
		if (SceneManager::GetInstance().Empty())
		{
			SceneManager::GetInstance().PushScene(scene);
		}
	}

	void GameManager::SetWindows_Size(glm::vec2 screenSize)
	{
	}

	GLFWwindow *GameManager::GetWindow() const
	{
		return window;
	}

	glm::vec2 GameManager::GetWindowsSize() const
	{
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		return glm::vec2(w, h);
	}

	void GameManager::SetBaseAspectSize(int baseAspectWidth, int baseAspectHeight)
	{
		this->baseAspectSize = glm::vec2(static_cast<float>(baseAspectWidth), static_cast<float>(baseAspectHeight));
	}

	glm::vec2 GameManager::GetBaseAspectSize() const
	{
		return this->baseAspectSize;
	}

	bool GameManager::IsFullScreen() const
	{
		return this->fullScreenMode;
	}

	void GameManager::SetWindows_FullScreenMode()
	{
		const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
		glfwSetWindowPos(window, 0, 0);
		glfwSetWindowSize(window, mode->width, mode->height);
		fullScreenMode = true;
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);
	}

	void GameManager::SetWindows_WindowsMode(int width, int height)
	{
		glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowPos(window, 100, 100);
		glfwSetWindowSize(window, width, height);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		fullScreenMode = false;
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, width, height);
	}

	void GameManager::EndProgram()
	{
		glfwSetWindowShouldClose(window, true);
	}

	void GameManager::Shutdown()
	{

		glfwTerminate();

		SceneManager::GetInstance().Clear();

		MusicManager::GetInstance().Shutdown();
		SoundManager::GetInstance().Shutdown();

		InputManager::GetInstance().Shutdown();
		UIManager::GetInstance().Shutdown();
		TimerManager::GetInstance().Shutdown();
		SceneManager::GetInstance().Shutdown();
		DataBaseManager::GetInstance().Shutdown();

		NetworkManager::GetInstance().Shutdown();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		std::cout << "Game manager Shutdown" << std::endl;
	}

	GameManager::~GameManager()
	{
		std::cout << "Game manager destroyed" << std::endl;
	}
}
