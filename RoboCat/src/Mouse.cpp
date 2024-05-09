#include "Mouse.h"

Mouse::Mouse()
{
  SetScale(GetScale() * 0.5f);
  SetCollisionRadius(0.25f);
}

bool Mouse::HandleCollisionWithCat(RoboCat *inCat)
{
  (void)inCat;
  return false;
}

//写入颜色和位置,角度
uint32_t Mouse::Write(OutputMemoryBitStream &inOutputStream, uint32_t inDirtyState) const
{
  uint32_t writtenState = 0;

  if (inDirtyState & EMRS_Pose)
  {
    inOutputStream.Write(true);

    Vector3 location = GetLocation();
    inOutputStream.Write(location.mX);
    inOutputStream.Write(location.mY);

    inOutputStream.Write(GetRotation());

    writtenState |= EMRS_Pose;
  }
  else
  {
    inOutputStream.Write(false);
    }

  if (inDirtyState & EMRS_Color)
  {
    inOutputStream.Write(true);

    inOutputStream.Write(getColor());

    writtenState |= EMRS_Color;
  }
  else
  {
    inOutputStream.Write(false);
  }

  return writtenState;
}

void Mouse::Read(InputMemoryBitStream &inInputStream)
{
  bool stateBit;
  inInputStream.Read(stateBit);
  if (stateBit)
  {
    Vector3 location;
    inInputStream.Read(location.mX);
    inInputStream.Read(location.mY);
    SetLocation(location);

    float rotation;
    inInputStream.Read(rotation);
    SetRotation(rotation);
  }

  inInputStream.Read(stateBit);
  if (stateBit)
  {
    Vector3 color;
    inInputStream.Read(color);
    setColor(color);
  }
}