#include "RoboCatServer.h"
#include "MoveList.h"
#include "Yarn.h"
#include "GameObjectRegistry.h"
#include "ScoreBoardManager.h"

RoboCatServer::RoboCatServer() : mCatControlType(ECCT_Human),
                                 mTimeOfNextShot(0.f),
                                 mTimeBetweenShots(0.2f)  
{
}

void RoboCatServer::HandleDying()
{
  NetworkManagerServer::sInstance->UnregisterGameObject(this);
}

void RoboCatServer::Update()
{
  RoboCat::Update();

  Vector3 oldLocation = GetLocation();
  Vector3 oldVelocity = GetVelocity();
  float oldRotation = GetRotation();

  ClientProxyPtr client = NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());

  if (client)
  {
    MoveList &moveList = client->GetUnprocessedMoveList();
    for (const Move &unprocessedMove : moveList)
    {
      const InputState &currentState = unprocessedMove.GetInputState();
      float deltaTime = unprocessedMove.GetDeltaTime();
      
      ProcessInput(deltaTime, currentState);
      SimulateMovement(deltaTime);
    }
    moveList.Clear();
  }

  HandleShooting();

  if (!RoboMath::Is2DVectorEqual(oldLocation, GetLocation()) ||
      !RoboMath::Is2DVectorEqual(oldVelocity, GetVelocity()) ||
      oldRotation != GetRotation())
  {
    NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ECRS_Pose);
  }
}

void RoboCatServer::HandleShooting()
{
  float time = Timing::sInstance.GetFrameStartTime();
  if (mIsShooting && Timing::sInstance.GetFrameStartTime() > mTimeOfNextShot)
  {
    mTimeOfNextShot = time + mTimeBetweenShots;

    YarnPtr yarn = std::static_pointer_cast<Yarn>(GameObjectRegistry::sInstance->CreateGameObject('YARN'));
    yarn->InitFromShooter(this);
  }
}

void RoboCatServer::TakeDamage(int inDamagingPlayerId)
{
  mHealth--;
  if (mHealth <= 0.f)
  {
    // 为伤害玩家加分
    ScoreBoardManager::sInstance->IncScore(inDamagingPlayerId, 1);

    // and you want to die
    SetDoesWantToDie(true);

    // tell the client proxy to make you a new cat
    ClientProxyPtr clientProxy = NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());
    if (clientProxy)
    {
      clientProxy->HandleCatDied();
    }
  }
  // tell the world our health dropped
  NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ECRS_Health);
}