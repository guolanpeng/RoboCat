#include "MouseServer.h"
#include "ScoreBoardManager.h"

MouseServer::MouseServer()
{
}

void MouseServer::HandleDying()
{
  NetworkManagerServer::sInstance->UnregisterGameObject(this);
}

bool MouseServer::HandleCollisionWithCat(RoboCat *inCat)
{
  SetDoesWantToDie(true);

  ScoreBoardManager::sInstance->IncScore(inCat->GetPlayerId(), 1);

  return false;
}