#ifndef MOUSESERVER_H
#define MOUSESERVER_H
#include "Mouse.h"
#include "NetWorkManagerServer.h"

class MouseServer : public Mouse
{
public:
  static GameObjectPtr StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new MouseServer()); }
  void HandleDying() override;
  virtual bool HandleCollisionWithCat(RoboCat *inCat) override;

protected:
  MouseServer();
};

#endif