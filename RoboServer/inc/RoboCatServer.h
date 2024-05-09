#ifndef ROBOCATSERVER_H
#define ROBOCATSERVER_H
#include "RoboCat.h"
#include "NetWorkManagerServer.h"

enum ECatControlType
{
  ECCT_Human,
  ECCT_AI,
};

class RoboCatServer : public RoboCat
{
public:
  static GameObjectPtr StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new RoboCatServer()); }

  virtual void HandleDying() override;

  virtual void Update() override;

  void TakeDamage(int inDamagingPlayerId);

protected:
  RoboCatServer();

private:
  void HandleShooting();

  ECatControlType mCatControlType;

  float mTimeOfNextShot; // 下一次射击的时间
  float mTimeBetweenShots; // 射击之间的时间间隔
};

#endif // ROBOCATSERVER_H