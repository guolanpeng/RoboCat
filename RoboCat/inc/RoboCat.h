#ifndef ROBOCAT_H
#define ROBOCAT_H
#include "GameObject.h"
#include "InputState.h"
#include <vector>
#include <sys/socket.h>

class RoboCat : public GameObject
{
public:
    REGISTER_GAMEOBJECT('RCAT', GameObject)

    enum ECatReplicationState
    {
        ECRS_Pose = 1 << 0,
        ECRS_Color = 1 << 1,
        ECRS_PlayerId = 1 << 2,
        ECRS_Health = 1 << 3,

        ECRS_AllState = ECRS_Pose | ECRS_Color | ECRS_PlayerId | ECRS_Health
    };

    virtual ~RoboCat(){};

    static GameObject *StaticCreate() { return new RoboCat(); }

    virtual uint32_t GetAllStateMask() const override { return ECRS_AllState; }

    virtual RoboCat *GetAsCat() override { return this; }

    virtual void Update() override;

    void ProcessInput(float inDeltaTime, const InputState &inInputState); // 处理输入
    void SimulateMovement(float inDeltaTime);                             // 模拟移动

    void ProcessCollisions();                // 处理碰撞
    void ProcessCollisionsWithScreenWalls(); // 处理与屏幕墙的碰撞

    void SetPlayerId(uint32_t inPlayerId) { mPlayerId = inPlayerId; } // 设置玩家ID
    uint32_t GetPlayerId() const { return mPlayerId; }                // 获取玩家ID

    void SetVelocity(const Vector3 &inVelocity) { mVelocity = inVelocity; } // 设置速度
    const Vector3 &GetVelocity() const { return mVelocity; }                // 获取速度

    virtual uint32_t Write(OutputMemoryBitStream &stream, uint32_t inDirtyState) const override;

protected:
    RoboCat();

private:
    void AdjustVelocityByThrust(float inDeltaTime); // 根据推力调整速度

    Vector3 mVelocity; // 是一个三维向量。它的三个数值表示这个物体，在世界空间下沿X、Y、Z轴的速度大小。

    float mMaxLinearSpeed;   // 最大线速度
    float mMaxRotationSpeed; // 最大旋转速度

    // bounce fraction when hitting various things 碰撞时的反弹系数
    float mWallRestitution; // 墙壁弹性
    float mCatRestitution;  // 猫弹性

    uint32_t mPlayerId; // 玩家ID

protected:
    /// move down here for padding reasons...//为了填充原因而移动到这里...

    float mLastMoveTimestamp; // 最后移动时间戳

    float mThrustDir; // 推力方向
    int mHealth;      // 生命值

    bool mIsShooting; // 是否射击
};

typedef std::shared_ptr<RoboCat> RoboCatPtr;
#endif
