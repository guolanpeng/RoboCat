#include "ReplicationManagerServer.h"
#include "NetWorkManagerServer.h"
#include "StringUtils.h"

void ReplicationManagerServer::RelicateCreate(int inNetworkId, uint32_t inInitialDirtyState)
{
  mNetworkIdToReplicationCommandMap[inNetworkId] = ReplicationCommand(inInitialDirtyState);
}

void ReplicationManagerServer::ReplicateDestroy(int inNetworkId)
{
  mNetworkIdToReplicationCommandMap[inNetworkId].SetDestroy();
}

void ReplicationManagerServer::RemoveFromReplication(int inNetworkId)
{
  mNetworkIdToReplicationCommandMap.erase(inNetworkId);
}

void ReplicationManagerServer::SetStateDirty(int inNetworkId, uint32_t inDirtyState)
{
  mNetworkIdToReplicationCommandMap[inNetworkId].AddDirtyState(inDirtyState);
}

void ReplicationManagerServer::Write(OutputMemoryBitStream &inOutputStream)
{
  // 遍历每个复制命令并执行某些操作...
  for (auto &pair : mNetworkIdToReplicationCommandMap)
  {
    ReplicationCommand &replicationCommand = pair.second;
    if (replicationCommand.HasDirtyState())
    {
      int networkId = pair.first;
      // 打印网络ID
      // LOG("Replicating networkId %d", networkId);
      //  写入网络ID
      inOutputStream.Write(networkId);
      // only need 2 bits for action...
      ReplicationAction action = replicationCommand.GetAction();
      inOutputStream.Write(action, 2);

      if (networkId <= 20)
      {
        LOG("Replicating networkId %d action %d", networkId, action);
      }

      uint32_t writtenState = 0;
      uint32_t dirtyState = replicationCommand.GetDirtyState();

      switch (action)
      {
      case RA_Create:
        writtenState = WriteCreateAction(inOutputStream, networkId, dirtyState);
        // 中文注释：一旦传输了创建操作，将来对该对象的复制应该是更新而不是创建
        replicationCommand.SetAction(RA_Update);
        break;
      case RA_Update:
        writtenState = WriteUpdateAction(inOutputStream, networkId, dirtyState);
        break;
      case RA_Destroy:
        // 中文注释：如果是销毁操作，不需要写入任何东西
        writtenState = WriteDestroyAction(inOutputStream, networkId, dirtyState);
        // 将销毁操作添加到要删除的复制命令列表中
        mNetworkIdsToRemove.emplace_back(networkId);
        // q:emplace_back()和push_back()的区别？
        // a:emplace_back()是C++11的新特性，它是在容器的末尾直接构造一个对象，而push_back()是将一个对象拷贝到容器的末尾。
        break;
      }
      // 假设所有内容都已写入-不要让这太难
      replicationCommand.ClearDirtyState(writtenState);
    }
  }
  // 删除销毁对象的复制命令
  if (!mNetworkIdsToRemove.empty())
  {
    for (int networkId : mNetworkIdsToRemove)
    {
      RemoveFromReplication(networkId);
    }
    mNetworkIdsToRemove.clear();
  }
}

uint32_t ReplicationManagerServer::WriteCreateAction(OutputMemoryBitStream &inOutputStream, int inNetworkId, uint32_t inDirtyState)
{
  GameObjectPtr gameObject = NetworkManagerServer::sInstance->GetGameObject(inNetworkId);

  inOutputStream.Write(gameObject->GetClassId());
  static int MOUSCount = 0, CATCount = 0;
  if (gameObject->GetClassId() == 'MOUS')
  {
    ++MOUSCount;
    // LOG("Replicating create for networkId %d, MOUS", inNetworkId);
  }
  else
  {
    ++CATCount;
    // LOG("Replicating create for networkId %d, CAT", inNetworkId);
  }
  LOG("MOUSCount: %d, CATCount: %d", MOUSCount, CATCount);
  return gameObject->Write(inOutputStream, inDirtyState);
}

uint32_t ReplicationManagerServer::WriteUpdateAction(OutputMemoryBitStream &inOutputStream, int inNetworkId, uint32_t inDirtyState)
{
  GameObjectPtr gameObject = NetworkManagerServer::sInstance->GetGameObject(inNetworkId);
  LOG("Replicating update for networkId %d", inNetworkId);
  // 如果我们在另一边找不到gameObject，我们将无法读取写入的数据（因为我们不知道哪个类写入了它）
  // 所以我们需要知道要跳过多少字节。
  // 这意味着我们需要字节，每个新对象都需要字节对齐

  uint32_t writtenState = gameObject->Write(inOutputStream, inDirtyState);

  return writtenState;
}

uint32_t ReplicationManagerServer::WriteDestroyAction(OutputMemoryBitStream &inOutputStream, int inNetworkId, uint32_t inDirtyState)
{
  (void)inOutputStream;
  (void)inNetworkId;
  (void)inDirtyState;

  return inDirtyState;
}