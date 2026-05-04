#include "GameSceneManager.h"
#include  "../Models/GameScene.h"

GameSceneManager::GameSceneManager(){

}

static void *InitLoadingScene_ThreadFunc(ALLEGRO_THREAD *thread, void *arg) {
    GameScene *scene = (GameScene *)arg;
    std::cout << "THREAD 1" << std::endl;
    scene->Init();
    std::cout << "THREAD 2" << std::endl;
    return nullptr;
}

GameSceneManager::~GameSceneManager(){
    for (GameScene* scene : sceneStack) {
        if (scene) {
            scene->Clean();
            delete scene;
        }
    }
    sceneStack.clear();
}

void GameSceneManager::ChangeScene() {
    if (sceneChanged && nextScene) {
        if (!sceneStack.empty()) {
            sceneStack.back()->Clean();
            al_destroy_thread(sceneStack.back()->loadingThread);
            delete sceneStack.back();
            sceneStack.pop_back();
        }
        sceneStack.push_back(nextScene);
        //sceneStack.back()->loadingThread = al_create_thread(InitLoadingScene_ThreadFunc, sceneStack.back());
        //al_start_thread(sceneStack.back()->loadingThread);
        sceneStack.back()->Init();
        sceneChanged = false;
    }
}


void GameSceneManager::PushScene(GameScene* scene) {
    if (sceneStack.empty()) {
        sceneStack.push_back(scene);
        //sceneStack.back()->loadingThread = al_create_thread(InitLoadingScene_ThreadFunc, sceneStack.back());
        //al_start_thread(sceneStack.back()->loadingThread);
        sceneStack.back()->Init();
        sceneChanged = false;
    }
    else{
        nextScene = scene;
        sceneChanged = true;
    }
}


void GameSceneManager::PopScene() {
    if (!sceneStack.empty()) {
        sceneStack.back()->Clean();
        sceneStack.pop_back();
        if (!sceneStack.empty()) {
            //sceneStack.back()->loadingThread = al_create_thread(InitLoadingScene_ThreadFunc, sceneStack.back());
            //al_start_thread(sceneStack.back()->loadingThread);
            sceneStack.back()->Init();
        }
    }
}


void GameSceneManager::Update(double deltaTime) {
    if (sceneChanged) {
        ChangeScene();
    }
    if (!sceneStack.empty()) {
        sceneStack.back()->Update(deltaTime);

        /*
        if(sceneStack.back()->IsProgressLoadinSceneComplete()){
            sceneStack.back()->Update(deltaTime);
        }
        else{
            sceneStack.back()->UpdateLoadingScene();
        }
        */
    }
}

bool GameSceneManager::IsSceneLoaded(){
    if(!sceneStack.empty()){
        if(sceneStack.back()->IsProgressLoadinSceneComplete()){
            if(sceneStack.back()->loadingThread){
                al_join_thread(sceneStack.back()->loadingThread, nullptr);
                al_destroy_thread(sceneStack.back()->loadingThread);
                sceneStack.back()->loadingThread = nullptr;
            }
            return true;
        }
        else{
            false;
        }

    }
    return false;
}

bool GameSceneManager::Running() const {
    return isRunning && !sceneStack.empty();
}