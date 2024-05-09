#ifndef ROBOTMATH_H
#define ROBOTMATH_H
#include <cmath>

class Vector3
{
public:
    float mX, mY, mZ;

    Vector3(float x, float y, float z)
        : mX(x), mY(y), mZ(z)
    {
    }

    Vector3()
        : mX(0.f), mY(0.f), mZ(0.f)
    {
    }

    void Set(float x, float y, float z)
    {
        mX = x;
        mY = y;
        mZ = z;
    }

    friend Vector3 operator+(const Vector3 &lhs, const Vector3 &rhs)
    {
        return Vector3(lhs.mX + rhs.mX, lhs.mY + rhs.mY, lhs.mZ + rhs.mZ);
    }
    friend Vector3 operator-(const Vector3 &lhs, const Vector3 &rhs)
    {
        return Vector3(lhs.mX - rhs.mX, lhs.mY - rhs.mY, lhs.mZ - rhs.mZ);
    }

    // Component-wise multiplication //分量乘法
    friend Vector3 operator*(const Vector3 &lhs, const Vector3 &rhs)
    {
        return Vector3(lhs.mX * rhs.mX, lhs.mY * rhs.mY, lhs.mZ * rhs.mZ);
    }

    // Scalar multiplication //标量乘法
    friend Vector3 operator*(float inScalar, const Vector3 &lhs)
    {
        return Vector3(lhs.mX * inScalar, lhs.mY * inScalar, lhs.mZ * inScalar);
    }

    friend Vector3 operator*(const Vector3 &lhs, float inScalar)
    {
        return Vector3(lhs.mX * inScalar, lhs.mY * inScalar, lhs.mZ * inScalar);
    }

    Vector3 &operator*=(float inScalar)
    {
        mX *= inScalar;
        mY *= inScalar;
        mZ *= inScalar;
        return *this;
    }

    Vector3 &operator+=(const Vector3 &inVec)
    {
        mX += inVec.mX;
        mY += inVec.mY;
        mZ += inVec.mZ;
        return *this;
    }

    Vector3 &operator-=(const Vector3 &inVec)
    {
        mX -= inVec.mX;
        mY -= inVec.mY;
        mZ -= inVec.mZ;
        return *this;
    }

    float Length() const
    {
        return sqrtf(mX * mX + mY * mY + mZ * mZ);
    }

    float LengthSq() const
    {
        return mX * mX + mY * mY + mZ * mZ;
    }

    float Length2D() const
    {
        return sqrtf(mX * mX + mY * mY);
    }

    float LengthSq2D() const
    {
        return mX * mX + mY * mY;
    }

    // 向量归一化
    void Normalize()
    {
        float length = Length();
        if (length > 0.f)
        {
            mX /= length;
            mY /= length;
            mZ /= length;
        }
    }

    void Normalize2D()
    {
        float length = Length2D();
        if (length > 0.f)
        {
            mX /= length;
            mY /= length;
        }
    }

    friend float Dot(const Vector3 &lhs, const Vector3 &rhs)
    {
        return lhs.mX * rhs.mX + lhs.mY * rhs.mY + lhs.mZ * rhs.mZ;
    }

    friend float Dot2D(const Vector3 &lhs, const Vector3 &rhs)
    {
        return lhs.mX * rhs.mX + lhs.mY * rhs.mY;
    }

    // 叉乘
    friend Vector3 Cross(const Vector3 &lhs, const Vector3 &rhs)
    {
        return Vector3(lhs.mY * rhs.mZ - lhs.mZ * rhs.mY,
                       lhs.mZ * rhs.mX - lhs.mX * rhs.mZ,
                       lhs.mX * rhs.mY - lhs.mY * rhs.mX);
    }

    // 线性插值
    friend Vector3 Lerp(const Vector3 &start, const Vector3 &end, float alpha)
    {
        return (start + alpha * (end - start));
    }

    static const Vector3 Zero;
    static const Vector3 UnitX;
    static const Vector3 UnitY;
    static const Vector3 UnitZ;
};

// 四元数
class Quaternion
{
public:
    float mX, mY, mZ, mW;
};

template <int tValue, int tBits>
struct GetRequiredBitsHelper
{
    static constexpr int Value = GetRequiredBitsHelper<(tValue >> 1), tBits + 1>::Value;
};

template <int tBits>
struct GetRequiredBitsHelper<0, tBits>
{
    static constexpr int Value = tBits;
};

template <int tValue>
struct GetRequiredBits
{
    static constexpr int Value = GetRequiredBitsHelper<tValue, 0>::Value;
};

namespace RoboMath
{
    const float PI = 3.14159265359f;
    float GetRandomFloat(); // 获取一个随机浮点数

    Vector3 GetRandomVector(const Vector3 &inMin, const Vector3 &inMax); // 获取一个随机向量

    inline bool Is2DVectorEqual(const Vector3 &inA, const Vector3 &inB) // 判断两个二维向量是否相等
    {
        return inA.mX == inB.mX && inA.mY == inB.mY;
    }

    inline float ToDegrees(float inRadians) // 弧度转角度
    {
        return inRadians * 180.f / PI;
    }
}

namespace Colors
{
    static const Vector3 White(1.f, 1.f, 1.f);
    static const Vector3 Black(0.f, 0.f, 0.f);
    static const Vector3 Red(1.f, 0.f, 0.f);
    static const Vector3 Green(0.f, 1.f, 0.f);
    static const Vector3 Blue(0.f, 0.f, 1.f);
    static const Vector3 LightYellow(1.f, 1.f, 0.88f);    // 浅黄色
    static const Vector3 LightBlue(0.68f, 0.85f, 0.9f);   // 浅蓝色
    static const Vector3 LightPink(1.f, 0.71f, 0.76f);    // 浅粉色
    static const Vector3 LightGreen(0.56f, 0.93f, 0.56f); // 浅绿色
}
#endif