#ifndef YARN_H
#define YARN_H
#include "GameObject.h"

class Yarn : public GameObject
{
public:
    REGISTER_GAMEOBJECT('YARN', GameObject);

    enum EYarnReplicationState
    {
        EYRS_Pose = 1 << 0,
        EYRS_Color = 1 << 1,
        EYRS_PlayerId = 1 << 2,

        EYRS_AllState = EYRS_Pose | EYRS_Color | EYRS_PlayerId
    };

    static GameObject *StaticCreate() { return new Yarn(); }

    virtual uint32_t GetAllStateMask() const override { return EYRS_AllState; }

    virtual uint32_t Write(OutputMemoryBitStream &inOutputStream, uint32_t inDirtyState) const override;

    void SetPlayerId(int inPlayerId) { mPlayerId = inPlayerId; }
    int GetPlayerId() const { return mPlayerId; }

    void SetVelocity(const Vector3 &inVelocity) { mVelocity = inVelocity; }
    const Vector3 &GetVelocity() const { return mVelocity; }

    void InitFromShooter(RoboCat *inShooter);

    virtual void Update() override;

    virtual bool HandleCollisionWithCat(RoboCat *inCat) override;

protected:
    Yarn();

    Vector3 mVelocity; // 速度

    float mMuzzleSpeed; // 枪口速度
    int mPlayerId;      // 玩家ID
};

typedef std::shared_ptr<Yarn> YarnPtr;
#endif