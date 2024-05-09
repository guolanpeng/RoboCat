#ifndef WEIGHTEDTIMEDMOVINGAVERAGE_H
#define WEIGHTEDTIMEDMOVINGAVERAGE_H
#include "Timing.h"

class WeightedTimedMovingAverage
{
public:
  WeightedTimedMovingAverage(float inDuration = 5.f) : mDuration(inDuration),
                                                       mValue(0.f)
  {
    mTimeLastEntryMade = Timing::sInstance.GetTimef();
  };

  void UpdatePerSecond(float inValue)
  {
    float time = Timing::sInstance.GetTimef();
    float timeSinceLastEntry = time - mTimeLastEntryMade;

    float valueOverTime = mValue / timeSinceLastEntry;

    // now update our value by whatever amount of the duration that was..
    // 现在根据持续时间的多少更新我们的值
    float fractionOfDuration = timeSinceLastEntry / mDuration;

    if (fractionOfDuration > 1.f)
    {
      fractionOfDuration = 1.f;
    }

    mValue = mValue * (1.f - fractionOfDuration) + valueOverTime * fractionOfDuration;

    mTimeLastEntryMade = time;
  }

  void Update(float inValue)
  {
    float time = Timing::sInstance.GetTimef();
    float timeSinceLastEntry = time - mTimeLastEntryMade;

    // now update our value by whatever amount of the duration that was..
    //  现在根据持续时间的多少更新我们的值
    float fractionOfDuration = timeSinceLastEntry / mDuration;
    if (fractionOfDuration > 1.f)
    {
      fractionOfDuration = 1.f;
    }

    mValue = mValue * (1.f - fractionOfDuration) + inValue * fractionOfDuration;

    mTimeLastEntryMade = time;
  }

  float GetValue() const
  {
    return mValue;
  }

private:
  float mTimeLastEntryMade; // 上一次添加的时间
  float mDuration;          // 持续时间
  float mValue;             // 当前值
};
#endif // WEIGHTEDTIMEDMOVINGAVERAGE_H