#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H
#include <unordered_map>
#include <cstdint>
#include <queue>
#include <list>
#include "GameObject.h"
#include "SocketAddress.h"
#include "UDPSocket.h"
#include "WeightedTimedMovingAverage.h"

typedef std::unordered_map<int, GameObjectPtr> IntToGameObjectMap;

class NetworkManager
{
public:
  static const uint32_t kHelloCC = 'HELO';
  static const uint32_t kWelcomeCC = 'WLCM';
  static const uint32_t kStateCC = 'STAT';
  static const uint32_t kInputCC = 'INPT';
  static const int kMaxPacketsPerFrame = 10; // 每帧最大数据包数

  NetworkManager();
  virtual ~NetworkManager();

  bool Init(uint16_t inPort);
  void ProcessIncomingPackets();

  virtual void ProccessPacket(InputMemoryBitStream &inInputStream, const SocketAddress &inFromAddress) = 0;
  virtual void HandleConnectionReset(const SocketAddress &inFromAddress) { (void)inFromAddress; };

  void SendPacket(const OutputMemoryBitStream &inOutputStream, const SocketAddress &inToAddress);

  const WeightedTimedMovingAverage &GetBytesReceivedPerSecond() const { return mBytesReceivedPerSecond; }
  const WeightedTimedMovingAverage &GetBytesSentPerSecond() const { return mBytesSentPerSecond; }

  void SetDropPacketChance(float inChance) { mDropPacketChance = inChance; }
  float GetDropPacketChance() const { return mDropPacketChance; }
  void SetSimulatedLatency(float inLatency) { mSimulatedLatency = inLatency; }
  float GetSimulatedLatency() const { return mSimulatedLatency; }

  inline GameObjectPtr GetGameObject(int inNetworkId) const
  {
    auto it = mNetworkIdToGameObjectMap.find(inNetworkId);
    if (it != mNetworkIdToGameObjectMap.end())
    {
      return it->second;
    }
    return GameObjectPtr();
  };

  void AddToNetworkIdToGameObjectMap(GameObjectPtr inGameObject);
  void RemoveFromNetworkIdToGameObjectMap(GameObjectPtr inGameObject);

protected:
  IntToGameObjectMap mNetworkIdToGameObjectMap; // 网络ID到游戏对象的映射

private:
  class ReceivedPacket
  {
  public:
    ReceivedPacket(float inReceivedTime, const InputMemoryBitStream &inPacketBuffer, const SocketAddress &inFromAddress);

    float GetReceivedTime() const { return mReceivedTime; }
    InputMemoryBitStream &GetPacketBuffer() { return mPacketBuffer; }
    const SocketAddress &GetFromAddress() const { return mFromAddress; }

  private:
    float mReceivedTime;
    InputMemoryBitStream mPacketBuffer;
    SocketAddress mFromAddress;
  };

  void UpdateBytesSentLastFrame();
  void ProcessQueuedPackets();
  void ReadIncomingPacketsIntoQueue();

  std::queue<ReceivedPacket, std::list<ReceivedPacket>> mPacketQueue;

  UDPSocketPtr mSocket;

  WeightedTimedMovingAverage mBytesReceivedPerSecond; // 每秒接收的字节数
  WeightedTimedMovingAverage mBytesSentPerSecond;     // 每秒发送的字节数

  int mBytesSentThisFrame; // 这一帧发送的字节数

  float mDropPacketChance; // 丢包率
  float mSimulatedLatency; // 模拟延迟
};

#endif // NETWORKMANAGER_H