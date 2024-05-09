
#ifndef NETWORKMANAGERSERVER_H
#define NETWORKMANAGERSERVER_H
#include <cstdint>
#include "NetWorkManager.h"
#include "MemoryBitStream.h"
#include "ClientProxy.h"

class NetworkManagerServer : public NetworkManager
{
public:
  static NetworkManagerServer *sInstance;

  static bool StaticInit(uint16_t inPort);

  virtual void ProccessPacket(InputMemoryBitStream &inInputStream, const SocketAddress &inFromAddress) override;
  virtual void HandleConnectionReset(const SocketAddress &inFromAddress) override;

  void SendOutgoingPackets();
  void CheckForDisconnects();

  void RegisterGameObject(GameObjectPtr inGameObject);
  inline GameObjectPtr RegisterAndReturn(GameObject *inGameObject)
  {
    GameObjectPtr toRet(inGameObject);
    RegisterGameObject(toRet);
    return toRet;
  };
  
  void UnregisterGameObject(GameObject *inGameObject);
  void SetStateDirty(uint32_t inNetworkId, uint32_t inDirtyState);

  void RespawmCats();

  ClientProxyPtr GetClientProxy(int PlayerId) const;

private:
  NetworkManagerServer();

  void HandlePacketFromNewClient(InputMemoryBitStream &inInputStream, const SocketAddress &inFromAddress);
  void ProcessPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream &inInputStream);

  void SendWelcomePacket(ClientProxyPtr inClientProxy);
  void UpdateAllClients();

  void AddWorldStateToPacket(OutputMemoryBitStream &inOutputStream); // 将世界状态添加到数据包中
  void AddScoreBoardToPacket(OutputMemoryBitStream &inOutputStream); // 将记分板添加到数据包中

  void SendStatePacketToClient(ClientProxyPtr inClientProxy);                                              // 发送状态包给客户端
  void WriteLastMoveTimestampIfDirty(OutputMemoryBitStream &inOutputStream, ClientProxyPtr inClientProxy); // 如果数据包有变化，就写入最后移动的时间戳

  void HandleInputPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream &inInputStream);

  void HandleClientDisconnected(ClientProxyPtr inClientProxy); // 处理客户端断开连接

  int GetNewNetworkId(); // 获取新的网络ID

  typedef std::unordered_map<int, ClientProxyPtr> IntToClientMap;
  typedef std::unordered_map<SocketAddress, ClientProxyPtr> AddressToClientMap;

  IntToClientMap mPlayerIdToClientMap;    // 玩家ID到客户端代理的映射
  AddressToClientMap mAddressToClientMap; // 地址到客户端代理的映射

  int mNewPlayerId;  // 用于分配新的玩家ID
  int mNewNetworkId; // 用于分配新的网络ID

  float mTimeOfLastSatePacket;    // 上一次发送状态包的时间
  float mTimeBetweenStatePackets; // 发送状态包之间的时间间隔
  float mClientDisconnectTimeout; // 客户端断开连接的超时时间
};

#endif // NETWORKMANAGERSERVER_H