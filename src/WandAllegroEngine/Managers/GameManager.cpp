#include "GameManager.hpp"
#include "SceneManager.hpp"
#include "ObjectManager.hpp"
#include "BodyManager.hpp"
#include "InputManager.hpp"

WandEngine::GameManager::GameManager() : ScreenSize(Size(0, 0)), Window(nullptr), event_queue(nullptr),
										 lastTime(0), currentTime(0), deltaTime(0)
{
	this->redraw = true;
	this->ShowBodys = false;
	this->BackBufferColor = al_map_rgb(0, 0, 0);
}

bool WandEngine::GameManager::Init()
{
	if (!al_init())
	{
		al_show_native_message_box(NULL, "Error", "ERROR: initialize", ":/", NULL, ALLEGRO_MESSAGEBOX_YES_NO);
		return false;
	}

	al_install_audio();
	al_init_acodec_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();
	al_init_image_addon();

	al_reserve_samples(1);

	al_set_new_display_flags(ALLEGRO_OPENGL);
	al_set_new_display_option(ALLEGRO_OPENGL_MAJOR_VERSION, 3, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_OPENGL_MINOR_VERSION, 0, ALLEGRO_SUGGEST);

	Window = al_create_display(ScreenSize.Width, ScreenSize.Height);

	al_set_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, true);

	al_set_window_title(Window, "WandEngine");

	event_queue = al_create_event_queue();

	if (!event_queue)
	{
		al_show_native_message_box(NULL, "Error", "ERROR: initialize", ":/", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return false;
	}

	FPS = al_create_timer(1.0 / 60);

	al_register_event_source(event_queue, al_get_timer_event_source(FPS));
	al_register_event_source(event_queue, al_get_display_event_source(Window));

	al_start_timer(FPS);

	WandEngine::InputManager::GetInstance().Init();

	return true;
}

void WandEngine::GameManager::Update()
{

	lastTime = al_get_time();

	while (WandEngine::SceneManager::GetInstance().Running())
	{
		currentTime = al_get_time();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		WandEngine::InputManager::GetInstance().Update();

		while (al_get_next_event(event_queue, &Event))
		{
			InputManager::GetInstance().ProcessEvent(Event);

			switch (Event.type)
			{
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				WandEngine::SceneManager::GetInstance().Clear();
				break;

			case ALLEGRO_EVENT_TIMER:
				if (Event.timer.source == FPS)
					redraw = true;

				break;

			default:
				break;
			}
		}

		WandEngine::SceneManager::GetInstance().Update(deltaTime);
		WandEngine::ObjectManager::GetInstance().Update(deltaTime);
		WandEngine::BodyManager::GetInstance().Update();

		if (redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;

			if (WandEngine::SceneManager::GetInstance().IsSceneLoaded())
			{
				al_clear_to_color(BackBufferColor);

				WandEngine::ObjectManager::GetInstance().Draw();

				if (ShowBodys)
				{
					WandEngine::BodyManager::GetInstance().Draw();
				}

				al_flip_display();
			}
		}
	}
}

void WandEngine::GameManager::ToggleShowBody()
{
	this->ShowBodys = !this->ShowBodys;
}

void WandEngine::GameManager::SetBackBufferColor(ALLEGRO_COLOR color)
{
	this->BackBufferColor = color;
}

void WandEngine::GameManager::SetFirstScene(GameScene *scene)
{
	if (WandEngine::SceneManager::GetInstance().Empty())
	{
		WandEngine::SceneManager::GetInstance().PushScene(scene);
	}
}

void WandEngine::GameManager::SetWindows_Size(Size ScreenSize)
{
	if (!IsFullScreen())
	{
		if (al_resize_display(Window, ScreenSize.Width, ScreenSize.Height))
		{
			this->ScreenSize = ScreenSize;
			ALLEGRO_TRANSFORM transform;
			al_identity_transform(&transform);
			al_scale_transform(&transform, ScreenSize.Width / 1920.0, ScreenSize.Height / 1080.0);
			al_use_transform(&transform);
		}
	}
}

bool WandEngine::GameManager::IsFullScreen()
{
	int flags = al_get_display_flags(Window);
	return ((flags & ALLEGRO_FULLSCREEN) || (!flags & ALLEGRO_FULLSCREEN_WINDOW));
}

void WandEngine::GameManager::SetWindows_FullScreenMode()
{
	al_set_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, true);
	// al_toggle_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, true);
}

void WandEngine::GameManager::SetWindows_WindowsMode()
{
	// al_toggle_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, false);
	al_set_display_flag(Window, ALLEGRO_FULLSCREEN_WINDOW, false);
}

void WandEngine::GameManager::Clear()
{
	al_unregister_event_source(event_queue, al_get_display_event_source(Window));
	al_unregister_event_source(event_queue, al_get_timer_event_source(FPS));

	al_destroy_timer(FPS);

	if (Window)
	{
		al_destroy_display(Window);
	}

	if (event_queue)
	{
		al_destroy_event_queue(event_queue);
	}

	al_uninstall_audio();
	al_init_acodec_addon();
	al_shutdown_font_addon();
	al_shutdown_ttf_addon();
	al_shutdown_primitives_addon();
	al_shutdown_image_addon();

	std::cout << "Game clear" << std::endl;
}

WandEngine::GameManager::~GameManager()
{
	Clear();
	std::cout << "Game destroyed" << std::endl;
}