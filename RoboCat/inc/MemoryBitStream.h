#pragma once
#ifndef MEMORYBITSTREAM_H
#define MEMORYBITSTREAM_H
#include "RobotMath.h"
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <type_traits>
#include <string>

/*
    将InputMemoryBitStream和OutputMemoryBitStream合并为MemoryBitStream
*/

inline uint32_t ConvertToFixed(float inData, float inMin, float inPrecision)
{
    return static_cast<float>((inData - inMin) / inPrecision);
}

inline float ConvertFromFixed(uint32_t inData, float inMin, float inPrecision)
{
    return static_cast<float>(inData) * inPrecision + inMin;
}

class OutputMemoryBitStream
{
public:
    OutputMemoryBitStream() : mBitHead(0),
                              mBuffer(nullptr)
    {
        ReallocBuffer(1500 * 8);
    };
    ~OutputMemoryBitStream() { std::free(mBuffer); };

    void WriteBits(uint8_t inData, uint32_t inBitCount);
    void WriteBits(const void *inData, uint32_t inBitCount);

    const char *GetBufferPtr() const { return mBuffer; }
    uint32_t GetBitLength() const { return mBitHead; }
    uint32_t GetByteLength() const { return (mBitHead + 7) >> 3; }

    void WriteBytes(const void *inData, uint32_t inByteCount) { WriteBits(inData, inByteCount << 3); }

    template <typename T>
    void Write(T inData, uint32_t inBitCount = sizeof(T) * 8)
    {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types");
        WriteBits(&inData, inBitCount);
    }
    void Write(const char *inData, uint32_t inBitCount) { WriteBits(inData, inBitCount); }

    void Write(bool inData) { WriteBits(&inData, 1); }

    void Write(const Vector3 &inVector);
    void Write(const Quaternion &inQuat)
    {
        float precision = (2.f / 65535.f);
        Write(ConvertToFixed(inQuat.mX, -1.f, precision), 16);
        Write(ConvertToFixed(inQuat.mY, -1.f, precision), 16);
        Write(ConvertToFixed(inQuat.mZ, -1.f, precision), 16);
        Write(inQuat.mW < 0);
    };

    void Write(const std::string &inString)
    {
        uint32_t elementCount = static_cast<uint32_t>(inString.size());
        Write(elementCount);
        for (const auto &element : inString)
        {
            Write(element);
        }
    }

private:
    void ReallocBuffer(uint32_t inNewBitCapacity);

    char *mBuffer;
    uint32_t mBitCapacity;
    uint32_t mBitHead;
};

class InputMemoryBitStream
{
public:
    InputMemoryBitStream(char *inBuffer, uint32_t inBitCount) : mBuffer(inBuffer),
                                                                mBitCapacity(inBitCount),
                                                                mBitHead(0),
                                                                mIsBufferOwner(false) {}

    InputMemoryBitStream(const InputMemoryBitStream &inStream) : mBitCapacity(inStream.mBitCapacity),
                                                                 mBitHead(inStream.mBitHead),
                                                                 mIsBufferOwner(true)
    {
        // q:为什么要加7再右移3位？
        // a:因为mBitCapacity是位数，所以要加7再右移3位，得到字节数
        int byteCount = (mBitCapacity + 7) >> 3;
        mBuffer = static_cast<char *>(std::malloc(byteCount));
        std::memcpy(mBuffer, inStream.mBuffer, byteCount);
    }

    ~InputMemoryBitStream()
    {
        if (mIsBufferOwner)
        {
            std::free(mBuffer);
            mBuffer = nullptr;
        }
    }

    const char *GetBufferPtr() const { return mBuffer; }
    uint32_t GetRemainingBitCount() const { return mBitCapacity - mBitHead; }

    void ReadBits(uint8_t &outData, uint32_t inBitCount);
    void ReadBits(void *outData, uint32_t inBitCount);

    void ReadBytes(void *outData, uint32_t inByteCount) { ReadBits(outData, inByteCount << 3); }

    template <typename T>
    void Read(T &outData, uint32_t inBitCount = sizeof(T) * 8)
    {
        static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Read only supports primitive data types");
        ReadBits(&outData, inBitCount);
    }

    void Read(bool &outData) { ReadBits(&outData, 1); }

    void Read(Vector3 &inVector);
    void Read(Quaternion &inQuat);

    void ResetToCapacity(uint32_t inByteCapacity)
    {
        mBitCapacity = inByteCapacity << 3;
        mBitHead = 0;
    }

    void Read(std::string &inString)
    {
        uint32_t elementCount;
        Read(elementCount);
        inString.resize(elementCount);
        for (auto &element : inString)
        {
            Read(element);
        }
    }

private:
    char *mBuffer;
    uint32_t mBitCapacity;
    uint32_t mBitHead;
    bool mIsBufferOwner;
};

#endif