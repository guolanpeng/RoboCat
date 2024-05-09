#ifndef CLIENTPROXY_H
#define CLIENTPROXY_H
#include <string>
#include <memory>
#include "ReplicationManagerServer.h"
#include "SocketAddress.h"
#include "MoveList.h"

class ClientProxy
{
public:
  ClientProxy(const SocketAddress &inSocketAddress, const std::string &inName, int inPlayerId);

  const SocketAddress &GetSocketAddress() const { return mSocketAddress; }
  int GetPlayerId() const { return mPlayerId; }
  const std::string &GetName() const { return mName; }

  void UpdateLastPacketTime();
  float GetLastPacketFromClientTime() const { return mLastPacketFromClientTime; }

  ReplicationManagerServer &GetReplicationManagerServer() { return mReplicationManagerServer; }

  const MoveList &GetUnprocessedMoveList() const { return mUnprocessedMoveList; }
  MoveList &GetUnprocessedMoveList() { return mUnprocessedMoveList; }

  void SetIsLastMoveTimestampDirty(bool inIsDirty) { mIsLastMoveTimestampDirty = inIsDirty; }
  bool IsLastMoveTimestampDirty() const { return mIsLastMoveTimestampDirty; }

  void HandleCatDied();
  void RespawnIfNecessary();

private:
  ReplicationManagerServer mReplicationManagerServer;

  SocketAddress mSocketAddress;
  std::string mName;
  int mPlayerId;

  float mLastPacketFromClientTime; // 最后一次从客户端接收到数据的时间
  float mTimeToRespawn;            // 玩家死亡后重生的时间

  MoveList mUnprocessedMoveList;
  bool mIsLastMoveTimestampDirty; // 最近一次移动的时间戳是否有变化
};

typedef std::shared_ptr<ClientProxy> ClientProxyPtr;
#endif // CLIENTPROXY_H