#include "MemoryBitStream.h"

void OutputMemoryBitStream::ReallocBuffer(uint32_t inNewBitCapacity)
{
    mBuffer = static_cast<char *>(realloc(mBuffer, inNewBitCapacity >> 3));
    if (mBuffer == nullptr)
    {
        mBitCapacity = 0;
        mBitHead = 0;
    }
    else
    {
        mBitCapacity = inNewBitCapacity;
    }
}

/**
 * Writes a specified number of bits from the given data to the output memory bit stream.
 *
 * @param inData The data to be written.
 * @param inBitCount The number of bits to be written.
 */
void OutputMemoryBitStream::WriteBits(uint8_t inData, uint32_t inBitCount)
{
    uint32_t nextBitHead = mBitHead + static_cast<uint32_t>(inBitCount);
    if (nextBitHead > mBitCapacity)
    {
        ReallocBuffer(std::max(mBitCapacity * 2, nextBitHead));
    }

    uint32_t byteOffset = mBitHead >> 3;
    uint32_t bitOffset = mBitHead & 0x7;

    uint8_t currentMask = ~(0xff << bitOffset);
    mBuffer[byteOffset] = (mBuffer[byteOffset] & currentMask) | (inData << bitOffset);

    uint32_t bitsFreeThisByte = 8 - bitOffset;

    if (bitsFreeThisByte < inBitCount)
    {
        mBuffer[byteOffset + 1] = inData >> bitsFreeThisByte;
    }

    mBitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void *inData, uint32_t inBitCount)
{
    const char *srcByte = static_cast<const char *>(inData);
    while (inBitCount > 8)
    {
        WriteBits(*srcByte, 8);
        ++srcByte;
        inBitCount -= 8;
    }

    if (inBitCount > 0)
    {
        WriteBits(*srcByte, inBitCount);
    }
}

void OutputMemoryBitStream::Write(const Vector3 &inVector)
{
    Write(inVector.mX);
    Write(inVector.mY);
    Write(inVector.mZ);
}

void InputMemoryBitStream::ReadBits(uint8_t &outData, uint32_t inBitCount)
{
    // outData = 0;
    // uint32_t readSoFar = 0;
    // while (readSoFar < inBitCount && mBitHead < mBitCapacity)
    // {
    //     //q:为什么要加7再右移3位？
    //     //a:因为mBitCapacity是位数，所以要加7再右移3位，得到字节数
    //     uint32_t bitsThisByte = std::min(inBitCount - readSoFar, 8 - mBitHead % 8);
    //     uint8_t currentData = static_cast<uint8_t>(mBuffer[mBitHead / 8] >> (mBitHead % 8));
    //     currentData &= ~(0xff << bitsThisByte);
    //     outData |= currentData << readSoFar;
    //     readSoFar += bitsThisByte;
    //     mBitHead += bitsThisByte;
    // }

    uint32_t byteOffset = mBitHead >> 3;
    uint32_t bitOffset = mBitHead & 0x7;

    outData = static_cast<uint8_t>(mBuffer[byteOffset]) >> bitOffset;
    uint32_t bitsFreeThisByte = 8 - bitOffset;
    if (bitsFreeThisByte < inBitCount)
    {
        outData |= static_cast<uint8_t>(mBuffer[byteOffset + 1]) << bitsFreeThisByte;
    }

    outData &= (~(0x00ff << inBitCount));
    mBitHead += inBitCount;
}

void InputMemoryBitStream::ReadBits(void *outData, uint32_t inBitCount)
{
    uint8_t *destByte = reinterpret_cast<uint8_t *>(outData);
    while (inBitCount > 8)
    {
        ReadBits(*destByte, 8);
        ++destByte;
        inBitCount -= 8;
    }

    if (inBitCount > 0)
    {
        ReadBits(*destByte, inBitCount);
    }
}

void InputMemoryBitStream::Read(Vector3 &outVector)
{
    Read(outVector.mX);
    Read(outVector.mY);
    Read(outVector.mZ);
}

void InputMemoryBitStream::Read(Quaternion &outQuat)
{
    float precision = (2.f / 65535.f);

    uint32_t f = 0;

    Read(f, 16);
    outQuat.mX = ConvertFromFixed(f, -1.f, precision);
    Read(f, 16);
    outQuat.mY = ConvertFromFixed(f, -1.f, precision);
    Read(f, 16);
    outQuat.mZ = ConvertFromFixed(f, -1.f, precision);

    outQuat.mW = sqrtf(1.f - outQuat.mX * outQuat.mX - outQuat.mY * outQuat.mY - outQuat.mZ * outQuat.mZ);

    bool isNegative;
    Read(isNegative);

    if (isNegative)
    {
        outQuat.mW *= -1.f;
    }
}