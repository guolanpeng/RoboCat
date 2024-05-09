#include "Server.h"
#include "RoboCatShared.h"
#include "RoboCatServer.h"
#include "YarnServer.h"
#include "MouseServer.h"
#include "StringUtils.h"
#include "ClientProxy.h"

bool Server::StaticInit()
{
    sInstance.reset(new Server());
    return true;
}

Server::Server()
{
    GameObjectRegistry::sInstance->RegisterCreationFunction('RCAT', RoboCatServer::StaticCreate);
    GameObjectRegistry::sInstance->RegisterCreationFunction('YARN', YarnServer::StaticCreate);
    GameObjectRegistry::sInstance->RegisterCreationFunction('MOUS', MouseServer::StaticCreate);

    InitNetworkManager();

    float latency = 0.0f;
    std::string latencyString = StringUtils::GetCommandLineArg(2);
    if (!latencyString.empty())
    {
        latency = std::stof(latencyString);
    }
    NetworkManagerServer::sInstance->SetSimulatedLatency(latency);
}

int Server::Run()
{
    SetupWorld();

    return Engine::Run();
}

bool Server::InitNetworkManager()
{
    std::string portString = StringUtils::GetCommandLineArg(1);

    uint16_t port = std::stoi(portString);
    // 打印端口号
    StringUtils::Log("Server listening on port %d", port);

    return NetworkManagerServer::StaticInit(port);
}

namespace
{
    void CreateRandomMice(int inMouseCount)
    {
        Vector3 mouseMin(-5.f, -3.f, 0.f);
        Vector3 mouseMax(5.f, 3.f, 0.f);

        GameObjectPtr go;
        for (int i = 0; i < 10; ++i)
        {
            go = GameObjectRegistry::sInstance->CreateGameObject('MOUS');
            Vector3 mouseLocation = RoboMath::GetRandomVector(mouseMin, mouseMax);
            go->SetLocation(mouseLocation);
        }
    }
}

void Server::SetupWorld()
{
    CreateRandomMice(10);

    CreateRandomMice(10);
}

void Server::DoFrame()
{
    NetworkManagerServer::sInstance->ProcessIncomingPackets();

    NetworkManagerServer::sInstance->CheckForDisconnects();

    NetworkManagerServer::sInstance->RespawmCats();

    Engine::DoFrame();

    NetworkManagerServer::sInstance->SendOutgoingPackets();
}

void Server::HandleNewClient(ClientProxyPtr inClientProxy)
{
    int playerId = inClientProxy->GetPlayerId();

    ScoreBoardManager::sInstance->AddEntry(playerId, inClientProxy->GetName());
    // 为新的客户端生成RoboCat
    SpawnCatForPlayer(playerId);
}

void Server::SpawnCatForPlayer(int inPlayerId)
{
    RoboCatPtr cat = std::static_pointer_cast<RoboCat>(GameObjectRegistry::sInstance->CreateGameObject('RCAT'));
    cat->setColor(ScoreBoardManager::sInstance->GetEntry(inPlayerId)->GetColor());
    cat->SetPlayerId(inPlayerId);

    cat->SetLocation(Vector3(1.f - static_cast<float>(inPlayerId), 0.f, 0.f));
}
void Server::HandleLostClient(ClientProxyPtr inClientProxy)
{
    // 杀死客户端的RoboCat
    // 从分数板中移除客户端
    int playerId = inClientProxy->GetPlayerId();

    ScoreBoardManager::sInstance->RemoveEntry(playerId);
    RoboCatPtr cat = GetCatForPlayer(playerId);
    if (cat)
    {
        cat->SetDoesWantToDie(true);
    }
}

RoboCatPtr Server::GetCatForPlayer(int inPlayerId)
{
    // run through the objects till we find the cat...
    // it would be nice if we kept a pointer to the cat on the clientproxy
    // but then we'd have to clean it up when the cat died, etc.
    // this will work for now until it's a perf issue
    const auto &gameObjects = World::sInstance->GetGameObjects();
    for (int i = 0, c = gameObjects.size(); i < c; ++i)
    {
        GameObjectPtr go = gameObjects[i];
        RoboCat *cat = go->GetAsCat();
        if (cat && cat->GetPlayerId() == inPlayerId)
        {
            return std::static_pointer_cast<RoboCat>(go);
        }
    }

    return nullptr;
}
