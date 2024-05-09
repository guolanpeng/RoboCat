#ifndef REPLICATIONMANAGERSERVER_H
#define REPLICATIONMANAGERSERVER_H
#include <unordered_map>
#include <vector>
#include "ReplicationCommand.h"
#include "MemoryBitStream.h"

class ReplicationManagerServer
{
public:
  void RelicateCreate(int inNetworkId, uint32_t inInitialDirtyState);
  void ReplicateDestroy(int inNetworkId);
  void SetStateDirty(int inNetworkId, uint32_t inDirtyState);
  void RemoveFromReplication(int inNetworkId);

  void Write(OutputMemoryBitStream &inOutputStream);

private:
  uint32_t WriteCreateAction(OutputMemoryBitStream &inOutputStream, int inNetworkId, uint32_t inDirtyState);
  uint32_t WriteUpdateAction(OutputMemoryBitStream &inOutputStream, int inNetworkId, uint32_t inDirtyState);
  uint32_t WriteDestroyAction(OutputMemoryBitStream &inOutputStream, int inNetworkId, uint32_t inDirtyState);

  std::unordered_map<int, ReplicationCommand> mNetworkIdToReplicationCommandMap;
  std::vector<int> mNetworkIdsToRemove;
};

#endif