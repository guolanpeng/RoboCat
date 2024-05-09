#include "Yarn.h"
#include "RoboCat.h"
#include "Timing.h"

Yarn::Yarn()
    : mVelocity(Vector3::Zero),
      mMuzzleSpeed(3.f),
      mPlayerId(0)
{
    SetScale(GetScale() * 0.25f);
    SetCollisionRadius(0.125f);
}

uint32_t Yarn::Write(OutputMemoryBitStream &inOutputStream, uint32_t inDirtyState) const
{
    uint32_t writtenState = 0;

    if (inDirtyState & EYRS_Pose)
    {
        inOutputStream.Write(true);
        Vector3 location = GetLocation();
        inOutputStream.Write(location.mX);
        inOutputStream.Write(location.mY);

        Vector3 velocity = GetVelocity();
        inOutputStream.Write(velocity.mX);
        inOutputStream.Write(velocity.mY);

        inOutputStream.Write(GetRotation());

        writtenState |= EYRS_Pose;
    }
    else
    {
        inOutputStream.Write(false);
    }

    if (inDirtyState & EYRS_Color)
    {
        inOutputStream.Write(true);
        inOutputStream.Write(getColor());
        writtenState |= EYRS_Color;
    }
    else
    {
        inOutputStream.Write(false);
    }

    if (inDirtyState & EYRS_PlayerId)
    {
        inOutputStream.Write(true);
        inOutputStream.Write(mPlayerId, 8);
        writtenState |= EYRS_PlayerId;
    }
    else
    {
        inOutputStream.Write(false);
    }
    return writtenState;
}

bool Yarn::HandleCollisionWithCat(RoboCat *inCat)
{
    (void)inCat;

    return false;
}

void Yarn::InitFromShooter(RoboCat *inShooter)
{

    setColor(inShooter->getColor());
    SetPlayerId(inShooter->GetPlayerId());

    Vector3 forward = inShooter->GetForwardVector();
    SetVelocity(inShooter->GetVelocity() + forward * mMuzzleSpeed);
    SetLocation(inShooter->GetLocation() /* + forward * 0.5f */);

    SetRotation(inShooter->GetRotation());
}

void Yarn::Update()
{
    float deltaTime = Timing::sInstance.GetDeltaTime();

    SetLocation(GetLocation() + mVelocity * deltaTime);

    // 我们让猫处理碰撞
}