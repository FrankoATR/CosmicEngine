#include "GameManager.hpp"
#include "SceneManager.hpp"
#include "InputManager.hpp"
#include "TimerManager.hpp"

namespace WandEngine
{

	GameManager::GameManager() : Window(nullptr), event_queue(nullptr), lastTime(0), currentTime(0), deltaTime(0)
	{
		this->redraw = true;
	}

	bool GameManager::Init()
	{
		if (!al_init())
		{
			al_show_native_message_box(nullptr, "Error", "ERROR: initialize", ":/", nullptr, ALLEGRO_MESSAGEBOX_YES_NO);
			return false;
		}

		al_init_font_addon();
		al_init_ttf_addon();
		al_init_primitives_addon();
		al_init_image_addon();

		al_set_new_display_flags(ALLEGRO_OPENGL);
		al_set_new_display_option(ALLEGRO_OPENGL_MAJOR_VERSION, 3, ALLEGRO_SUGGEST);
		al_set_new_display_option(ALLEGRO_OPENGL_MINOR_VERSION, 0, ALLEGRO_SUGGEST);

		Window = al_create_display(ScreenSize.width, ScreenSize.height);

		al_set_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, true);

		al_set_window_title(Window, "WandEngine");

		event_queue = al_create_event_queue();

		if (!event_queue)
		{
			al_show_native_message_box(nullptr, "Error", "ERROR: initialize", ":/", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
			return false;
		}

		FPS = al_create_timer(1.0 / 60);

		al_register_event_source(event_queue, al_get_timer_event_source(FPS));
		al_register_event_source(event_queue, al_get_display_event_source(Window));

		al_start_timer(FPS);

		InputManager::GetInstance().Init();

		return true;
	}

	void GameManager::Update()
	{

		lastTime = al_get_time();

		while (SceneManager::GetInstance().Running())
		{
			currentTime = al_get_time();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			InputManager::GetInstance().Update();
			TimerManager::GetInstance().Update(currentTime);

			while (al_get_next_event(event_queue, &Event))
			{
				InputManager::GetInstance().ProcessEvent(Event);

				switch (Event.type)
				{
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					SceneManager::GetInstance().Clear();
					break;

				case ALLEGRO_EVENT_TIMER:
					if (Event.timer.source == FPS)
						redraw = true;

					break;

				default:
					break;
				}
			}

			SceneManager::GetInstance().Update(deltaTime);

			if (redraw && al_is_event_queue_empty(event_queue))
			{
				redraw = false;
				SceneManager::GetInstance().Draw();
			}
		}
	}

	void GameManager::SetFirstScene(GameScene *scene)
	{
		if (SceneManager::GetInstance().Empty())
		{
			SceneManager::GetInstance().PushScene(scene);
		}
	}

	void GameManager::SetWindows_Size(WAND_SIZE ScreenSize)
	{
		if (!IsFullScreen())
		{
			if (al_resize_display(Window, ScreenSize.width, ScreenSize.height))
			{
				this->ScreenSize = ScreenSize;
				ALLEGRO_TRANSFORM transform;
				al_identity_transform(&transform);
				al_scale_transform(&transform, ScreenSize.width / 1920.0, ScreenSize.height / 1080.0);
				al_use_transform(&transform);
			}
		}
	}

	WAND_SIZE GameManager::GetWindowsSize()
	{
		return this->ScreenSize;
	}

	bool GameManager::IsFullScreen()
	{
		int flags = al_get_display_flags(Window);
		return ((flags & ALLEGRO_FULLSCREEN) || (!flags & ALLEGRO_FULLSCREEN_WINDOW));
	}

	void GameManager::SetWindows_FullScreenMode()
	{
		al_set_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, true);
		// al_toggle_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, true);
	}

	void GameManager::SetWindows_WindowsMode()
	{
		// al_toggle_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, false);
		al_set_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, false);
	}

	void GameManager::Clear()
	{

		InputManager::GetInstance().Clear();

		if (event_queue)
		{
			if (Window)
			{
				al_unregister_event_source(event_queue, al_get_display_event_source(Window));
			}

			if (FPS)
			{
				al_unregister_event_source(event_queue, al_get_timer_event_source(FPS));
			}

			al_destroy_event_queue(event_queue);
			event_queue = nullptr;
		}

		if (FPS)
		{
			al_destroy_timer(FPS);
			FPS = nullptr;
		}

		if (Window)
		{
			al_destroy_display(Window);
			Window = nullptr;
		}

		al_uninstall_audio();
		al_init_acodec_addon();
		al_shutdown_font_addon();
		al_shutdown_ttf_addon();
		al_shutdown_primitives_addon();
		al_shutdown_image_addon();

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
