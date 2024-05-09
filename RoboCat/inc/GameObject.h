#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
#include <cstdint>
#include <memory>
#include "MemoryBitStream.h"
#include "RobotMath.h"

class RoboCat;

// 使用宏来支持对象创建注册表
#define REGISTER_GAMEOBJECT(inCode, inClass)                 \
    enum                                                     \
    {                                                        \
        kClassId = inCode                                    \
    };                                                       \
    virtual uint32_t GetClassId() const { return kClassId; } \
    static GameObject *CreateInstance() { return new inClass(); }

class GameObject
{
public:
    REGISTER_GAMEOBJECT('GOBJ', GameObject)

    GameObject() : mColor(Colors::White),
                   mCollisionRadius(0.5f),
                   mRotation(0.f),
                   mScale(1.f),
                   mIndexInWorld(-1),
                   mDoesWantToDie(false),
                   mNetworkId(0) {}
    virtual ~GameObject(){};

    virtual RoboCat *GetAsCat() { return nullptr; }

    virtual uint32_t GetAllStateMask() const { return 0; }

    virtual bool HandleCollisionWithCat(RoboCat *inCat)
    {
        (void)inCat;
        return false;
    }

    virtual void Update() {}; // 更新

    virtual void HandleDying() {}; // 处理死亡

    void SetIndexInWorld(int inIndex) { mIndexInWorld = inIndex; }
    int GetIndexInWorld() const { return mIndexInWorld; }

    void SetRotation(float inRotation) { mRotation = inRotation; }
    float GetRotation() const { return mRotation; }

    void SetScale(float inScale) { mScale = inScale; }
    float GetScale() const { return mScale; }

    const Vector3 &GetLocation() const { return mLocation; }
    void SetLocation(const Vector3 &inLocation) { mLocation = inLocation; }

    float GetCollisionRadius() const { return mCollisionRadius; }
    void SetCollisionRadius(float inRadius) { mCollisionRadius = inRadius; }

    Vector3 GetForwardVector() const { return Vector3(sinf(mRotation), -cosf(mRotation), 0.f); }// 前向向量

    void setColor(const Vector3 &inColor) { mColor = inColor; }
    const Vector3 &getColor() const { return mColor; }

    bool DoesWantToDie() const { return mDoesWantToDie; }
    void SetDoesWantToDie(bool inWants) { mDoesWantToDie = inWants; }
    

    int GetNetworkId() const { return mNetworkId; }
    void SetNetworkId(int inNetworkId) { mNetworkId = inNetworkId; }

    virtual uint32_t Write(OutputMemoryBitStream &inOutputStream, uint32_t inDirtyState) const
    {
        // 消除编译器警告
        (void)inOutputStream;
        (void)inDirtyState;
        return 0;
    }

    virtual void Read(InputMemoryBitStream &inInputStream)
    {
        // 消除编译器警告
        (void)inInputStream;
    }

private:
    Vector3 mLocation;
    Vector3 mColor;

    float mCollisionRadius; // 碰撞半径

    float mRotation;   // 旋转角度
    float mScale;      // 缩放比例
    int mIndexInWorld; // 在世界中的索引

    bool mDoesWantToDie; // 是否想死

    int mNetworkId; // 网络ID
};

typedef std::shared_ptr<GameObject> GameObjectPtr;
#endif