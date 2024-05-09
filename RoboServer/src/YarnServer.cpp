#include "YarnServer.h"
#include "RoboCat.h"
#include "RoboCatServer.h"

YarnServer::YarnServer()
{
  // 线在游戏中的生命周期为1秒
  mTimeToDie = Timing::sInstance.GetFrameStartTime() + 1.f;
}

void YarnServer::HandleDying()
{
  NetworkManagerServer::sInstance->UnregisterGameObject(this);
}

void YarnServer::Update()
{
  Yarn::Update();

  if (Timing::sInstance.GetFrameStartTime() > mTimeToDie)
  {
    SetDoesWantToDie(true);
  }
}

bool YarnServer::HandleCollisionWithCat(RoboCat *inCat)
{
  if (inCat->GetPlayerId() != GetPlayerId())
  {
    SetDoesWantToDie(true);

    static_cast<RoboCatServer *>(inCat)->TakeDamage(GetPlayerId());
  }
  return false;
}