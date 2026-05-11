#include "GameScene.hpp"
#include "../Managers/ResourceManager.hpp"
#include "../Managers/BodyManager.hpp"
#include "../Managers/ObjectManager.hpp"

GameScene::GameScene(std::string Name) : ProgressLoadingScene(0.0f), Name(Name)
{

}


void GameScene::UpdateLoadingScene(){

}



void GameScene::SetProgressLoadingScene(float Progress){
    if(Progress > 1.0)
        this->ProgressLoadingScene = 1.0;
    else if(Progress < 0.0)
        this->ProgressLoadingScene = 0.0;
    else
        this->ProgressLoadingScene = Progress;
}


float GameScene::GetProgressLoadingScene(){
    return this->ProgressLoadingScene;
}


bool GameScene::IsProgressLoadinSceneComplete(){
    return (this->ProgressLoadingScene == 1.0);
}


std::string GameScene::GetName(){
    return this->Name;
}


void GameScene::Clear(){
    WandEngine::BodyManager::GetInstance().Clear();
    WandEngine::ObjectManager::GetInstance().Clear();
    WandEngine::ResourceManager::GetInstance().clear();
    std::cout << "Scene Clear: " << GetName() << std::endl;

}


GameScene::~GameScene()
{
    Clear();
    std::cout << "Scene destroyed: " << GetName() << std::endl;
}