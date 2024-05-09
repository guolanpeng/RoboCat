#include "RobotMath.h"
#include <random>

const Vector3 Vector3::Zero(0.f, 0.f, 0.f);
const Vector3 Vector3::UnitX(1.f, 0.f, 0.f);
const Vector3 Vector3::UnitY(0.f, 1.f, 0.f);
const Vector3 Vector3::UnitZ(0.f, 0.f, 1.f);

float RoboMath::GetRandomFloat()
{
  static std::random_device rd;                               // 随机数种子
  static std::mt19937 gen(rd());                              // 随机数引擎
  static std::uniform_real_distribution<float> dis(0.f, 1.f); // 均匀分布
  return dis(gen);                                            // 返回一个随机数
}

Vector3 RoboMath::GetRandomVector(const Vector3 &inMin, const Vector3 &inMax)
{
  Vector3 r = Vector3(GetRandomFloat(), GetRandomFloat(), GetRandomFloat());
  return inMin + (inMax - inMin) * r; // 返回一个在[inMin, inMax]范围内的随机向量
}