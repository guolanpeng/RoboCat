#include "NetWorkManager.h"
#include "SocketUtil.h"
#include "StringUtils.h"

NetworkManager::NetworkManager() : mBytesSentThisFrame(0),
                                   mDropPacketChance(0.0f),
                                   mSimulatedLatency(0.0f)
{
}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::Init(uint16_t inPort)
{
    mSocket = SocketUtil::CreateUDPSocket(INET);
    SocketAddress ownAddress(INADDR_ANY, inPort);
    mSocket->Bind(ownAddress);

    mBytesReceivedPerSecond = WeightedTimedMovingAverage(1.0f);
    mBytesSentPerSecond = WeightedTimedMovingAverage(1.0f);

    if (mSocket == nullptr)
    {
        return false;
    }

    if (mSocket->SetNonBlockingMode(true) == -1)
    {
        return false;
    }

    return true;
}

void NetworkManager::ProcessIncomingPackets()
{
    ReadIncomingPacketsIntoQueue(); // 读取数据包到队列
    ProcessQueuedPackets();         // 处理队列中的数据包
    UpdateBytesSentLastFrame();     // 更新上一帧发送的字节数
}

void NetworkManager::ReadIncomingPacketsIntoQueue()
{
    char packetMem[1500];
    int packetSize = sizeof(packetMem);
    InputMemoryBitStream inputStream(packetMem, packetSize * 8);
    SocketAddress fromAddress;

    int receivedPacketCount = 0;
    int totalReadByteCount = 0;

    while (receivedPacketCount < kMaxPacketsPerFrame)
    {
        int readByteCount = mSocket->ReceiveFrom(packetMem, packetSize, fromAddress);
        if (readByteCount == 0)
        {
            HandleConnectionReset(fromAddress);
            break;
        }
        else if (readByteCount == -1)
        {
            if (errno == EWOULDBLOCK)
            {
                break;
            }
            else
            {
                HandleConnectionReset(fromAddress);
            }
        }
        else if (readByteCount > 0)
        {
            // LOG("Read %d bytes from %s", readByteCount, fromAddress.ToString().c_str());
            inputStream.ResetToCapacity(readByteCount);
            ++receivedPacketCount;
            totalReadByteCount += readByteCount;

            if (RoboMath::GetRandomFloat() >= mDropPacketChance)
            {
                float simulatedReceivedTime = Timing::sInstance.GetTimef() + mSimulatedLatency;
                mPacketQueue.emplace(simulatedReceivedTime, inputStream, fromAddress);
            }
            else
            {
                // 丢包
            }
        }
        else
        {
            // 这是什么情况？
        }
    }
    if (totalReadByteCount > 0)
    {
        mBytesReceivedPerSecond.Update(static_cast<float>(totalReadByteCount));
    }
}

void NetworkManager::ProcessQueuedPackets()
{
    while (!mPacketQueue.empty())
    {
        ReceivedPacket &nextPacket = mPacketQueue.front();
        if (Timing::sInstance.GetTimef() > nextPacket.GetReceivedTime())
        {
            //LOG("Processing packet from %s", nextPacket.GetFromAddress().ToString().c_str());
            ProccessPacket(nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress());
            mPacketQueue.pop();
        }
        else
        {
            // 丢包
        }
    }
}

void NetworkManager::SendPacket(const OutputMemoryBitStream &inOutputStream, const SocketAddress &inToAddress)
{
    int writtenByteCount = mSocket->SendTo(inOutputStream.GetBufferPtr(), inOutputStream.GetByteLength(), inToAddress);
    if (writtenByteCount > 0)
    {
        mBytesSentThisFrame += writtenByteCount;
    }
}

void NetworkManager::UpdateBytesSentLastFrame()
{
    mBytesSentPerSecond.Update(static_cast<float>(mBytesSentThisFrame));
    mBytesSentThisFrame = 0;
}

NetworkManager::ReceivedPacket::ReceivedPacket(float inReceivedTime, const InputMemoryBitStream &inPacketBuffer, const SocketAddress &inFromAddress)
    : mReceivedTime(inReceivedTime),
      mPacketBuffer(inPacketBuffer),
      mFromAddress(inFromAddress)
{
}

void NetworkManager::AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
    mNetworkIdToGameObjectMap[inGameObject->GetNetworkId()] = inGameObject;
}

void NetworkManager::RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject)
{
    mNetworkIdToGameObjectMap.erase(inGameObject->GetNetworkId());
}
