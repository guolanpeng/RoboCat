#ifndef WORLD_H
#define WORLD_H
#include <memory>
#include <vector>
#include "GameObject.h"

class World
{
public:
    static void StaticInit();

    static std::unique_ptr<World> sInstance;

    void AddGameObject(GameObjectPtr inGameObject);
    void RemoveGameObject(GameObjectPtr inGameObject);

    void Update();

    const std::vector<GameObjectPtr> &GetGameObjects() const { return mGameObjects; }
private:

    World();

    int GeIndexofGameObject(GameObjectPtr inGameObject);

    std::vector<GameObjectPtr> mGameObjects;
};

#endif // WORLD_H