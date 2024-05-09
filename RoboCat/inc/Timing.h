#ifndef TIMING_H
#define TIMING_H
#include <cstdint>

class Timing
{
public:
  Timing();

  void Update();

  float GetDeltaTime() const { return mDeltaTime; }

  double GetTime() const;

  // 返回当前时间，单位为秒
  float GetTimef() const
  {
    return static_cast<float>(GetTime());
  }

  float GetFrameStartTime() const { return mFrameStartTime; }

  static Timing sInstance;

private:
  float mDeltaTime;
  uint64_t mDeltaTick;

  double mLastFrameStartTime; // 上一帧开始时间
  float mFrameStartTime;      // 当前帧开始时间
  float mPerfCountDuration;   // 每帧持续时间
};

#endif // TIMING_H