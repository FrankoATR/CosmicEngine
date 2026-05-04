#include "GameScene.h"

GameScene::GameScene(GameManager* Game) : Game(Game), ProgressLoadingScene(0.0f)
{

}


void GameScene::UpdateLoadingScene(){

}



void GameScene::SetProgressLoadingScene(float Progress){
    if(Progress > 1.0)
        this->ProgressLoadingScene = 1.0;
    else if(Progress < 0.0){
        this->ProgressLoadingScene = 0.0;
    }
    else{
        this->ProgressLoadingScene = Progress;
    }
}


float GameScene::GetProgressLoadingScene(){
    return this->ProgressLoadingScene;
}


bool GameScene::IsProgressLoadinSceneComplete(){
    return (this->ProgressLoadingScene == 1.0);
}





void GameScene::Clean(){
    this->Game->gameBodyManager->Clear();
    this->Game->gameObjectManager->Clear();
    this->Game->resourceManager->clear();

}


GameScene::~GameScene()
{
    std::cout << "GameScene Destroyed" << std::endl;
    Clean();

}