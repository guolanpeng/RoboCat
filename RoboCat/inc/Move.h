#ifndef MOVE_H
#define MOVE_H
#include "InputState.h"

class Move
{
public:
  Move(){};
  Move(const InputState &inInputState, float inTimestamp, float inDeltaTime) : mInputState(inInputState),
                                                                               mTimestamp(inTimestamp),
                                                                               mDeltaTime(inDeltaTime){};
  const InputState &GetInputState() const { return mInputState; }
  float GetTimestamp() const { return mTimestamp; }
  float GetDeltaTime() const { return mDeltaTime; }

  bool Write(OutputMemoryBitStream &inOutputStream) const;
  bool Read(InputMemoryBitStream &inInputStream);

private:
  InputState mInputState;
  float mTimestamp; // 时间戳
  float mDeltaTime; // 时间间隔
};

#endif // MOVE_H