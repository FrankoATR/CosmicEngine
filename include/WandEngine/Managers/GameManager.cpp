#include "GameManager.hpp"
#include "Scene/SceneManager.hpp"
#include "Input/InputManager.hpp"
#include "Timer/TimerManager.hpp"
#include "Resource/ResourceManager.hpp"
#include "Camera/CameraManager.hpp"
#include "Audio/Music/MusicManager.hpp"
#include "Audio/Sound/SoundManager.hpp"
#include "Timer/TimerManager.hpp"

#include <iostream>
#include <thread>
#include <chrono>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace WandEngine
{

	GameManager::GameManager() : window(nullptr), fullScreenMode(false), VSyncEnable(false), targetFPS(60), gameticks(20.0f)
	{
	}

	bool GameManager::Init(int screenWidth, int screenHeight)
	{

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

		// glEnable(GL_DEPTH_TEST);  //en 3D
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0, 0, screenWidth, screenHeight);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetDropCallback(window, DropCallback);

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

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");

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
				// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // en 3D
				glClear(GL_COLOR_BUFFER_BIT);

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				SceneManager::GetInstance().Draw();

				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				glfwSwapBuffers(window);
				lastRenderTimer = glfwGetTime();
			}

			glfwPollEvents();
		}
	}

	void GameManager::framebuffer_size_callback(GLFWwindow *window, int width, int height)
	{
		float currentAspect = (float)width / (float)height;
		int newWidth = width;
		int newHeight = height;

		if (currentAspect > (16.0f / 9.0f))
		{
			newWidth = (int)(height * (16.0f / 9.0f));
		}
		else
		{
			newHeight = (int)(width / (16.0f / 9.0f));
		}

		int offsetX = (width - newWidth) / 2;
		int offsetY = (height - newHeight) / 2;
		glfwSetWindowAspectRatio(window, 16, 9);

		glViewport(offsetX, offsetY, newWidth, newHeight);
	}

	void GameManager::DropCallback(GLFWwindow *window, int count, const char **paths)
	{
		for (int i = 0; i < count; i++)
		{
			GameManager::GetInstance().PushDroppedFile(paths[i]);
		}
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

	int GameManager::GetCurrentFPS()
	{
	}

	void GameManager::SetGameTicks(float gameTicks)
	{
		this->gameticks = gameTicks;
	}

	float GameManager::GetCurrenGameTicks()
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

	void GameManager::SetFirstScene(GameScene *scene)
	{
		if (SceneManager::GetInstance().Empty())
		{
			SceneManager::GetInstance().PushScene(scene);
		}
	}

	void GameManager::SetWindows_Size(glm::vec2 screenSize)
	{
	}

	GLFWwindow *GameManager::GetWindow()
	{
		return window;
	}

	glm::vec2 GameManager::GetWindowsSize()
	{
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		return glm::vec2(w, h);
	}

	bool GameManager::IsFullScreen()
	{
		return fullScreenMode;
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

	void GameManager::Clear()
	{

		glfwTerminate();

		InputManager::GetInstance().Clear();
		SceneManager::GetInstance().Clear();

		MusicManager::GetInstance().Shutdown();
		SoundManager::GetInstance().Shutdown();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

#ifndef NDEBUG
		std::cout << "Game cleared" << std::endl;
#endif
	}

	GameManager::~GameManager()
	{
#ifndef NDEBUG
		std::cout << "Game destroyed" << std::endl;
#endif
	}
}
