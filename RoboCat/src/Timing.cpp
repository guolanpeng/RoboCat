#include "Timing.h"
#include <chrono>
// Q: 为什么是这个值？0.0166f是什么意思？
// A: 0.0166f是16.6ms，是60fps的帧间隔时间，这个值是一个理想值，用来计算帧间隔时间的，这个值是固定的，不会改变。
float KDesiredFrameTime = 0.0166f;

using namespace std::chrono;

Timing Timing::sInstance;

high_resolution_clock::time_point sStartTime;

Timing::Timing()
{
  sStartTime = high_resolution_clock::now(); // 获取当前时间
}

void Timing::Update()
{
  double currentTime = GetTime();

  mDeltaTime = static_cast<float>(currentTime - mLastFrameStartTime);

  // 限制帧间隔时间
  while (mDeltaTime < KDesiredFrameTime)
  {
    currentTime = GetTime();
    mDeltaTime = static_cast<float>(currentTime - mLastFrameStartTime);
  }

  mLastFrameStartTime = currentTime;
  mFrameStartTime = static_cast<float>(currentTime);
}

double Timing::GetTime() const
{
  auto now = high_resolution_clock::now();
  auto ms = duration_cast<milliseconds>(now - sStartTime).count();
  // 将其转换为double再转回去有点不太好，但没办法。
  return static_cast<double>(ms) / 1000.0;
}