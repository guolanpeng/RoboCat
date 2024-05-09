#include "GameObjectRegistry.h"
#include "World.h"

std::unique_ptr<GameObjectRegistry> GameObjectRegistry::sInstance;


void GameObjectRegistry::StaticInit()
{
    sInstance.reset(new GameObjectRegistry());
}

GameObjectRegistry::GameObjectRegistry()
{

}

void GameObjectRegistry::RegisterCreationFunction(uint32_t inFourCCName, GameObjectCreationFunc inCreationFunction)
{
    auto it = mNameToGameObjectCreationFunctionMap.find(inFourCCName);
    if (it == mNameToGameObjectCreationFunctionMap.end())
    {
        mNameToGameObjectCreationFunctionMap[inFourCCName] = inCreationFunction;
    }
    else
    {
        assert(0);
    }
}


GameObjectPtr GameObjectRegistry::CreateGameObject(uint32_t inFourCCName)
{
    GameObjectCreationFunc creationFunc = mNameToGameObjectCreationFunctionMap[inFourCCName];

    GameObjectPtr gameObject = creationFunc();

    World::sInstance->AddGameObject(gameObject);

    return gameObject;
}