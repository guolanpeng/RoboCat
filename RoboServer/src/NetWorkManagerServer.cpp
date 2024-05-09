#include "NetWorkManagerServer.h"
#include "StringUtils.h"
#include "Server.h"
#include "World.h"
#include "ScoreBoardManager.h"

NetworkManagerServer *NetworkManagerServer::sInstance;

NetworkManagerServer::NetworkManagerServer()
    : mNewPlayerId(1),
      mNewNetworkId(1),
      mTimeBetweenStatePackets(0.033f),
      mClientDisconnectTimeout(3.f)
{
}

bool NetworkManagerServer::StaticInit(uint16_t inPort)
{
    sInstance = new NetworkManagerServer();
    return sInstance->Init(inPort);
}

void NetworkManagerServer::HandleConnectionReset(const SocketAddress &inFromAddress)
{
    // 只是立即断开客户端连接
    auto it = mAddressToClientMap.find(inFromAddress);
    if (it != mAddressToClientMap.end())
    {
        HandleClientDisconnected(it->second);
    }
}

void NetworkManagerServer::ProccessPacket(InputMemoryBitStream &inInputStream, const SocketAddress &inFromAddress)
{
    // 尝试获取此地址的客户端代理
    // 将此传递给客户端代理以进行处理
    auto it = mAddressToClientMap.find(inFromAddress);
    if (it == mAddressToClientMap.end())
    {
        // 如果没有找到一个，那么是一个新的客户端，是一个HELO吗？如果是，创建一个客户端代理
        HandlePacketFromNewClient(inInputStream, inFromAddress);
    }
    else
    {
        ProcessPacket(it->second, inInputStream);
    }
}

void NetworkManagerServer::ProcessPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream &inInputStream)
{
    inClientProxy->UpdateLastPacketTime();

    // 读取packet type
    uint32_t packetType;
    inInputStream.Read(packetType);
    switch (packetType)
    {
    case kHelloCC:
        // 客户端发送的hello包
        // 读取客户端的名字
        SendWelcomePacket(inClientProxy);
        break;
    case kInputCC:
        // LOG("Got input packet from  playerID %d and PlayerName %s", inClientProxy->GetPlayerId(), inClientProxy->GetName().c_str());
        HandleInputPacket(inClientProxy, inInputStream);
        break;
    default:
        // 打印未知的数据包
        LOG("Unknown packet type %ud from %s", packetType, inClientProxy->GetSocketAddress().ToString().c_str());

        break;
    }
}

void NetworkManagerServer::HandlePacketFromNewClient(InputMemoryBitStream &inInputStream, const SocketAddress &inFromAddress)
{
    // 读取packet type
    uint32_t packetType;
    inInputStream.Read(packetType);

    if (packetType == kHelloCC)
    {
        std::string name;
        inInputStream.Read(name);
        // 创建一个新的客户端代理
        ClientProxyPtr newClientProxy = std::make_shared<ClientProxy>(inFromAddress, name, mNewPlayerId++);
        mAddressToClientMap[inFromAddress] = newClientProxy;
        mPlayerIdToClientMap[newClientProxy->GetPlayerId()] = newClientProxy;

        // 告诉服务器有新的客户端连接，生成一个猫等等...
        // 如果我们有一个通用的消息系统，这将是一个很好的用途...
        // 相反，我们将直接告诉服务器
        static_cast<Server *>(Engine::sInstance.get())->HandleNewClient(newClientProxy);

        SendWelcomePacket(newClientProxy);

        for (const auto &pair : mNetworkIdToGameObjectMap)
        {
            newClientProxy->GetReplicationManagerServer().RelicateCreate(pair.first, pair.second->GetAllStateMask());
        }
    }
    else
    {
        LOG("Bad incoming packet type %ud from unknown client at address %s", packetType, inFromAddress.ToString().c_str());
    }
}

void NetworkManagerServer::SendWelcomePacket(ClientProxyPtr inClientProxy)
{
    OutputMemoryBitStream welcomePacket;

    welcomePacket.Write(kWelcomeCC);
    welcomePacket.Write(inClientProxy->GetPlayerId());

    LOG("Server Welcoming,new client %s as player %d", inClientProxy->GetSocketAddress().ToString().c_str(), inClientProxy->GetPlayerId());

    SendPacket(welcomePacket, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::RespawmCats()
{
    for (auto it = mAddressToClientMap.begin(); it != mAddressToClientMap.end(); ++it)
    {
        ClientProxyPtr clientProxy = it->second;

        clientProxy->RespawnIfNecessary();
    }
}

void NetworkManagerServer::SendOutgoingPackets()
{
    /// 每当客户端的移动到来时，我们就发送一个状态包给客户端
    for (auto it = mAddressToClientMap.begin(); it != mAddressToClientMap.end(); ++it)
    {
        ClientProxyPtr clientProxy = it->second;
        // 如果客户端的最后移动时间戳有变化，我们就发送一个状态包给客户端
        if (clientProxy->IsLastMoveTimestampDirty())
        {
            SendStatePacketToClient(clientProxy);
        }
    }
}

void NetworkManagerServer::UpdateAllClients()
{
    for (auto it = mAddressToClientMap.begin(); it != mAddressToClientMap.end(); ++it)
    {
        ClientProxyPtr clientProxy = it->second;
        SendStatePacketToClient(clientProxy);
    }
}

void NetworkManagerServer::SendStatePacketToClient(ClientProxyPtr inClientProxy)
{
    // 构建一个状态包
    OutputMemoryBitStream statePacket;

    statePacket.Write(kStateCC);

    WriteLastMoveTimestampIfDirty(statePacket, inClientProxy);

    AddScoreBoardToPacket(statePacket);

    inClientProxy->GetReplicationManagerServer().Write(statePacket);
    SendPacket(statePacket, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::WriteLastMoveTimestampIfDirty(OutputMemoryBitStream &inOutputStream, ClientProxyPtr inClientProxy)
{
    bool isTimestampDirty = inClientProxy->IsLastMoveTimestampDirty();
    inOutputStream.Write(isTimestampDirty);
    if (isTimestampDirty)
    {
        inOutputStream.Write(inClientProxy->GetUnprocessedMoveList().GetLastMoveTimestamp());
        inClientProxy->SetIsLastMoveTimestampDirty(false);
    }
}

// 我们应该向服务器询问这个吗？还是自己运行世界？
void NetworkManagerServer::AddWorldStateToPacket(OutputMemoryBitStream &inOutputStream)
{
    const auto &gameObjects = World::sInstance->GetGameObjects();

    // 现在开始写入对象-我们需要记住有多少个吗？我们可以先检查...
    inOutputStream.Write(gameObjects.size());

    for (GameObjectPtr gameObject : gameObjects)
    {
        inOutputStream.Write(gameObject->GetNetworkId());
        inOutputStream.Write(gameObject->GetClassId());
        gameObject->Write(inOutputStream, 0xffffffff); // 0xffffffff表示所有的状态都是脏的
    }
}

void NetworkManagerServer::AddScoreBoardToPacket(OutputMemoryBitStream &inOutputStream)
{
    ScoreBoardManager::sInstance->Write(inOutputStream);
}

int NetworkManagerServer::GetNewNetworkId()
{
    int toRet = mNewNetworkId++;
    if (mNewNetworkId < toRet)
    {
        LOG("Network ID Wrap Around!!! You have been playing too long...", 0);
    }
    return toRet;
}

void NetworkManagerServer::HandleInputPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream &inInputStream)
{
    uint32_t moveCount = 0;
    Move move;
    inInputStream.Read(moveCount, 2);

    // LOG("Got %d moves from playerID and PlayerName %d %s", moveCount, inClientProxy->GetPlayerId(), inClientProxy->GetName().c_str());

    for (; moveCount > 0; --moveCount)
    {
        if (move.Read(inInputStream))
        {
            if (inClientProxy->GetUnprocessedMoveList().AddMove(move))
            {
                inClientProxy->SetIsLastMoveTimestampDirty(true);
            }
        }
    }
}

ClientProxyPtr NetworkManagerServer::GetClientProxy(int PlayerId) const
{
    auto it = mPlayerIdToClientMap.find(PlayerId);
    if (it != mPlayerIdToClientMap.end())
    {
        return it->second;
    }
    return nullptr;
}

void NetworkManagerServer::HandleClientDisconnected(ClientProxyPtr inClientProxy)
{
    // 从映射中删除
    mPlayerIdToClientMap.erase(inClientProxy->GetPlayerId());
    mAddressToClientMap.erase(inClientProxy->GetSocketAddress());

    LOG("Client disconnected %s", inClientProxy->GetSocketAddress().ToString().c_str());

    // 通知服务器
    static_cast<Server *>(Engine::sInstance.get())->HandleLostClient(inClientProxy);

    // 如果是最后一个客户端，退出游戏
    if (mAddressToClientMap.empty())
    {
        Engine::sInstance->SetShouldKeepRunning(false);
    }
}

void NetworkManagerServer::RegisterGameObject(GameObjectPtr inGameObject)
{
    // 分配网络ID
    int newNetworkId = GetNewNetworkId();
    inGameObject->SetNetworkId(newNetworkId);

    // 添加从网络ID到游戏对象的映射
    mNetworkIdToGameObjectMap[newNetworkId] = inGameObject;

    // 通知所有客户端
    for (const auto &pair : mAddressToClientMap)
    {
        pair.second->GetReplicationManagerServer().RelicateCreate(newNetworkId, inGameObject->GetAllStateMask());
    }
}

void NetworkManagerServer::UnregisterGameObject(GameObject *inGameObject)
{
    int networkId = inGameObject->GetNetworkId();
    mNetworkIdToGameObjectMap.erase(networkId);

    // 通知所有客户端代理停止复制
    for (const auto &pair : mAddressToClientMap)
    {
        pair.second->GetReplicationManagerServer().ReplicateDestroy(networkId);
    }
}

void NetworkManagerServer::SetStateDirty(uint32_t inNetworkId, uint32_t inDirtyState)
{
    // 通知所有客户端代理
    for (const auto &pair : mAddressToClientMap)
    {
        pair.second->GetReplicationManagerServer().SetStateDirty(inNetworkId, inDirtyState);
    }
}

void NetworkManagerServer::CheckForDisconnects()
{
    vector<ClientProxyPtr> clientsToDC;

    float minAllowedLastPacketFromClientTime = Timing::sInstance.GetFrameStartTime() - mClientDisconnectTimeout;
    for (const auto &pair : mAddressToClientMap)
    {
        // 打印数据包的时间和minAllowedLastPacketFromClientTime
        // LOG("LastPacketFromClientTime %f minAllowedLastPacketFromClientTime %f", pair.second->GetLastPacketFromClientTime(), minAllowedLastPacketFromClientTime);

        if (pair.second->GetLastPacketFromClientTime() < minAllowedLastPacketFromClientTime)
        {
            // 打印端口连接的客户端信息
            LOG("Client %s timed out", pair.second->GetSocketAddress().ToString().c_str());
            // 如果客户端的最后数据包时间小于允许的最小时间，那么断开连接
            // 不能在迭代器中删除，所以只是记住以后删除
            clientsToDC.push_back(pair.second);
        }
    }

    for (ClientProxyPtr client : clientsToDC)
    {
        HandleClientDisconnected(client);
        return;
    }
}
