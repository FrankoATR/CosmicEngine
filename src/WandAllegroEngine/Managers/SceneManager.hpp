#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include <allegro5/allegro5.h>
#include <vector>
#include <iostream>

class GameScene;

namespace WandEngine
{

    class SceneManager
    {
    private:
        std::vector<GameScene *> sceneStack;
        bool isRunning = true;
        ALLEGRO_COLOR BackBufferColor;

        void StartSceneLoading(GameScene *scene);
        
        SceneManager();
        ~SceneManager();
        SceneManager(const SceneManager &) = delete;
        SceneManager &operator=(const SceneManager &) = delete;

    public:
        static SceneManager &GetInstance()
        {
            static SceneManager instance;
            return instance;
        }

        void PushScene(GameScene *scene);
        void ReplaceScene(GameScene *scene);
        void PopScene();

        void Update(double deltaTime);
        void Draw();

        bool Empty() const;
        void Clear();

        bool IsSceneLoaded() const;
        bool Running() const;

        void SetBackBufferColor(ALLEGRO_COLOR color);

    };

}

#endif // SCENEMANAGER_HPP
