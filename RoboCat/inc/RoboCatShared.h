#ifndef ROBOCAT_ROBOCATSHARED_H

#include "memory"

#include "vector"
#include "unordered_map"
#include "string"
#include "list"
#include "queue"
#include "deque"
#include "unordered_set"
#include "cassert"

using std::shared_ptr;
using std::unique_ptr;
using std::vector;
using std::queue;
using std::list;
using std::deque;
using std::unordered_map;
using std::string;
using std::unordered_set;

class RoboCat;
class GameObject;

#include "RobotMath.h"

#include "SocketAddress.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "SocketUtil.h"

#include "MemoryBitStream.h"

#include "InputAction.h"
#include "InputState.h"
#include "Move.h"
#include "MoveList.h"

#include "GameObject.h"
#include "GameObjectRegistry.h"
#include "RoboCat.h"
#include "Timing.h"
#include "Mouse.h"
#include "World.h"
#include "ScoreBoardManager.h"

#include "WeightedTimedMovingAverage.h"
#include "NetWorkManager.h"
#include "ReplicationCommand.h"
#include "Engine.h"

#endif // ROBOCAT_ROBOCATSHARED_H