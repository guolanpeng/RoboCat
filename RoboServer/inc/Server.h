#ifndef SERVER_H
#define SERVER_H
#include "ClientProxy.h"
#include "RoboCat.h"
#include "Engine.h"

class Server: public Engine
{
public:
  static bool StaticInit();

  virtual void DoFrame();

  virtual int Run();

  void HandleNewClient(ClientProxyPtr inClientProxy);  // 处理新的客户端连接
  void HandleLostClient(ClientProxyPtr inClientProxy); // 客户端断开连接

  RoboCatPtr GetCatForPlayer(int inPlayerId);
  void SpawnCatForPlayer(int inPlayerId); // 为玩家生成RoboCat

private:
  Server();

  bool InitNetworkManager();
  void SetupWorld();
};

#endif // SERVER_H