#include "RoboCat.h"
#include "World.h"

// 缩放硬编码为100...如果我们想要锁定玩家在屏幕上，这个值可以从缩放中计算出来
const float HALF_WORLD_HEIGHT = 3.6f;
const float HALF_WORLD_WIDTH = 6.4f;

RoboCat::RoboCat() : GameObject(),
                     mMaxRotationSpeed(5.f),
                     mMaxLinearSpeed(50.f),
                     mVelocity(Vector3::Zero),
                     mWallRestitution(0.1f),
                     mCatRestitution(0.1f),
                     mThrustDir(0.f),
                     mPlayerId(0),
                     mIsShooting(false),
                     mHealth(10)
{
    SetCollisionRadius(0.5f);
}

void RoboCat::ProcessInput(float inDeltaTime, const InputState &inInputState)
{
    // 处理输入
    float newRotation = GetRotation() + inInputState.GetDesiredHorizontalDelta() * mMaxRotationSpeed * inDeltaTime;
    SetRotation(newRotation);

    float inputForwardDelta = inInputState.GetDesiredVerticalDelta();
    mThrustDir = inputForwardDelta;

    mIsShooting = inInputState.IsShooting();
}

void RoboCat::AdjustVelocityByThrust(float inDeltaTime)
{
    // 只需根据推力方向设置速度——没有推力会导致速度为0
    // 模拟加速会使客户端预测变得更复杂
    Vector3 forwardVector = GetForwardVector();
    mVelocity = forwardVector * (mThrustDir * inDeltaTime * mMaxLinearSpeed);
}

void RoboCat::SimulateMovement(float inDeltaTime)
{
    AdjustVelocityByThrust(inDeltaTime);

    SetLocation(GetLocation() + mVelocity * inDeltaTime);

    ProcessCollisions();
}

void RoboCat::Update()
{
    // 更新逻辑
}

void RoboCat::ProcessCollisions()
{
    // 现在只是从侧面弹开...
    ProcessCollisionsWithScreenWalls();

    float sourceRadius = GetCollisionRadius();
    Vector3 sourceLocation = GetLocation();

    // 现在让我们遍历世界，看看我们撞到了什么...
    // 注意:由于我们的游戏中的对象数量很少，这是可以的。
    // 但在一个真正的游戏中，对每个其他对象进行碰撞检查是不高效的。
    // 最好使用四叉树或其他结构来最小化需要测试的碰撞数量。
    for (auto goIt = World::sInstance->GetGameObjects().begin(), end = World::sInstance->GetGameObjects().end(); goIt != end; ++goIt)
    {
        GameObject *target = goIt->get();

        if (target != this && !target->DoesWantToDie())
        {
            // simple collision test for spheres- are the radii summed less than the distance?
            // 球体的简单碰撞测试——半径之和是否小于距离？
            Vector3 targetLocation = target->GetLocation();
            float targetRadius = target->GetCollisionRadius();

            Vector3 delta = targetLocation - sourceLocation;
            float distSqr = delta.LengthSq2D();
            float collisionDist = sourceRadius + targetRadius;
            if (distSqr < (collisionDist * collisionDist))
            {
                // 首先，告诉其他对象我们碰到了它
                if (target->HandleCollisionWithCat(this))
                {
                    // 好的，你撞到了什么！所以，将你的位置投影得足够远，以便你不会发生碰撞
                    Vector3 dirToTarget = delta;
                    dirToTarget.Normalize2D();
                    Vector3 acceptableDeltaFromSourceToTarget = dirToTarget * collisionDist; // 这是我们可以移动的最大距禋
                    // 重要说明——我们只移动这只猫。另一只猫可以自己移动
                    SetLocation(targetLocation - acceptableDeltaFromSourceToTarget);

                    Vector3 relVel = mVelocity;
                    // 如果其他对象是一只猫，它可能有速度，所以可能有相对速度...
                    RoboCat *targetCat = target->GetAsCat();
                    if (targetCat)
                    {
                        relVel -= targetCat->mVelocity;
                    }

                    // 用对象之间的方向得到速度，以确定它们是否相互靠近
                    // 如果是这样，反弹的大小是多少（因为它们都只是球）
                    float relVelDotDir = Dot2D(relVel, dirToTarget);

                    if (relVelDotDir > 0.f)
                    {
                        Vector3 impulse = relVelDotDir * dirToTarget;

                        if (targetCat) // 如果我们撞到了另一只猫，我们会反弹
                        {
                            mVelocity -= impulse;
                            mVelocity *= mCatRestitution;
                        }
                        else // 否则，我们撞到了墙壁
                        {
                            mVelocity -= impulse * 2.f;
                            mVelocity *= mWallRestitution;
                        }
                    }
                }
            }
        }
    }
}

void RoboCat::ProcessCollisionsWithScreenWalls()
{
    Vector3 location = GetLocation();
    float x = location.mX;
    float y = location.mY;

    float vx = mVelocity.mX;
    float vy = mVelocity.mY;

    float radius = GetCollisionRadius();

    // 如果猫撞到墙上，快速解决方案是将其推开
    if ((y + radius) >= HALF_WORLD_HEIGHT && vy > 0)
    {
        mVelocity.mY = -vy * mWallRestitution;
        location.mY = HALF_WORLD_HEIGHT - radius;
        SetLocation(location);
    }
    else if (y <= (-HALF_WORLD_HEIGHT - radius) && vy < 0)
    {
        mVelocity.mY = -vy * mWallRestitution;
        location.mY = -HALF_WORLD_HEIGHT - radius;
        SetLocation(location);
    }

    if ((x + radius) >= HALF_WORLD_WIDTH && vx > 0)
    {
        mVelocity.mX = -vx * mWallRestitution;
        location.mX = HALF_WORLD_WIDTH - radius;
        SetLocation(location);
    }
    else if (x <= (-HALF_WORLD_WIDTH - radius) && vx < 0)
    {
        mVelocity.mX = -vx * mWallRestitution;
        location.mX = -HALF_WORLD_WIDTH - radius;
        SetLocation(location);
    }
}

uint32_t RoboCat::Write(OutputMemoryBitStream &inOutputStream, uint32_t inDirtyState) const
{
    uint32_t writtenState = 0;

    if (inDirtyState & ECRS_PlayerId)
    {
        inOutputStream.Write((bool)true);
        inOutputStream.Write(GetPlayerId());

        writtenState |= ECRS_PlayerId;
    }
    else
    {
        inOutputStream.Write((bool)false);
    }

    if (inDirtyState & ECRS_Pose)
    {
        inOutputStream.Write((bool)true);

        Vector3 velocity = mVelocity;
        inOutputStream.Write(velocity.mX);
        inOutputStream.Write(velocity.mY);

        Vector3 location = GetLocation();
        inOutputStream.Write(location.mX);
        inOutputStream.Write(location.mY);

        inOutputStream.Write(GetRotation());

        writtenState |= ECRS_Pose;
    }
    else
    {
        inOutputStream.Write((bool)false);
    }

    // 总是写mThrustDir——它只是两位
    if (mThrustDir != 0.f)
    {
        inOutputStream.Write(true);
        inOutputStream.Write(mThrustDir > 0.f);
    }
    else
    {
        inOutputStream.Write(false);
    }

    if (inDirtyState & ECRS_Color)
    {
        inOutputStream.Write((bool)true);
        inOutputStream.Write(getColor());

        writtenState |= ECRS_Color;
    }
    else
    {
        inOutputStream.Write((bool)false);
    }

    if (inDirtyState & ECRS_Health)
    {
        inOutputStream.Write((bool)true);
        inOutputStream.Write(mHealth, 4);

        writtenState |= ECRS_Health;
    }
    else
    {
        inOutputStream.Write((bool)false);
    }

    return writtenState;
}