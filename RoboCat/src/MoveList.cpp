#include "MoveList.h"

const Move &MoveList::AddMove(const InputState &inInputState, float inTimestamp)
{

  float deltaTime = mLastMoveTimestamp >= 0.f ? inTimestamp - mLastMoveTimestamp : 0.f;

  // emplace_back()函数在容器的末尾直接构造一个对象，不需要拷贝或移动操作
  mMoves.emplace_back(inInputState, inTimestamp, deltaTime);
  mLastMoveTimestamp = inTimestamp;
  return mMoves.back(); // 返回最后一个元素
}

bool MoveList::AddMove(const Move &inMove)
{
  // 调整时差
  float timeStamp = inMove.GetTimestamp();
  float deltaTime = mLastMoveTimestamp >= 0.f ? timeStamp - mLastMoveTimestamp : 0.f;
  mLastMoveTimestamp = timeStamp;
  mMoves.emplace_back(inMove.GetInputState(), timeStamp, deltaTime);
  return true;
}

void MoveList::RemovedProcessedMoves(float inLastMoveProcessedTimestamp)
{
  while (!mMoves.empty() && mMoves.front().GetTimestamp() <= inLastMoveProcessedTimestamp)
  {
    mMoves.pop_front();
  }
}
