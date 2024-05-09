#ifndef OBJECTCREATIONREGISTRY_H
#define OBJECTCREATIONREGISTRY_H
#include "GameObject.h"
#include <unordered_map>
#include <cassert>
#include "RoboCat.h"

// 注册表法

typedef GameObjectPtr (*GameObjectCreationFunc)();

class GameObjectRegistry
{
public:
    static void StaticInit();

    static std::unique_ptr<GameObjectRegistry> sInstance;

    void RegisterCreationFunction(uint32_t inFourCCName, GameObjectCreationFunc inCreationFunction);

    GameObjectPtr CreateGameObject(uint32_t inFourCCName);

private:
    GameObjectRegistry();

    std::unordered_map<uint32_t, GameObjectCreationFunc> mNameToGameObjectCreationFunctionMap;
};

#endif