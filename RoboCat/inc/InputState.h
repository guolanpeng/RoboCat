#ifndef INPUTSTATE_H
#define INPUTSTATE_H
#include "MemoryBitStream.h"
// InputState是有InputManager每一帧更新一次
// InputState类用来跟踪某一特定帧上客户端输入的状态

class InputState
{
public:
  InputState() : mDesiredRightAmount(0.f),
                 mDesiredLeftAmout(0.f),
                 mDesiredForwardAmount(0.f),
                 mDesiredBackAmount(0.f),
                 mIsShooting(false){};

  float GetDesiredHorizontalDelta() const
  {
    return mDesiredRightAmount - mDesiredLeftAmout;
  }
  float GetDesiredVerticalDelta() const
  {
    return mDesiredForwardAmount - mDesiredBackAmount;
  }
  bool IsShooting() const
  {
    return mIsShooting;
  }
  bool Write(OutputMemoryBitStream &inOutputStream) const;
  bool Read(InputMemoryBitStream &inInputStream);

private:
  friend class InputManager;
  
  float mDesiredRightAmount, mDesiredLeftAmout; // 右移和左移的量
  float mDesiredForwardAmount, mDesiredBackAmount; // 前进和后退的量
  bool mIsShooting; // 是否射击
};

#endif // INPUTSTATE_H