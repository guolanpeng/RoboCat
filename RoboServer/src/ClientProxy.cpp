#include "ClientProxy.h"
#include "Timing.h"
#include  "Server.h"

namespace
{
  const float kRespawnDelay = 3.f; // 重生延迟
}

ClientProxy::ClientProxy(const SocketAddress &inSocketAddress, const std::string &inName, int inPlayerId)
    : mSocketAddress(inSocketAddress),
      mName(inName),
      mPlayerId(inPlayerId),
      mLastPacketFromClientTime(0.f),
      mTimeToRespawn(0.f),
      mIsLastMoveTimestampDirty(false)
{
  UpdateLastPacketTime();
}

void ClientProxy::UpdateLastPacketTime()
{
  mLastPacketFromClientTime = Timing::sInstance.GetTimef();
}

void ClientProxy::HandleCatDied()
{
  mTimeToRespawn = Timing::sInstance.GetTimef() + kRespawnDelay;
}

void ClientProxy::RespawnIfNecessary()
{
  if (mTimeToRespawn != 0.f && Timing::sInstance.GetTimef() > mTimeToRespawn)
  {
    static_cast<Server *>(Engine::sInstance.get())->SpawnCatForPlayer(mPlayerId);
    mTimeToRespawn = 0.f;
  }
}
