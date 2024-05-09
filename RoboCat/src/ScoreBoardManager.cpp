#include "ScoreBoardManager.h"

std::unique_ptr<ScoreBoardManager> ScoreBoardManager::sInstance;

void ScoreBoardManager::StaticInit()
{
    sInstance.reset(new ScoreBoardManager());
}

ScoreBoardManager::ScoreBoardManager()
{
    mDefaultColors.push_back(Colors::LightYellow);
    mDefaultColors.push_back(Colors::LightBlue);
    mDefaultColors.push_back(Colors::LightPink);
    mDefaultColors.push_back(Colors::LightGreen);
}

ScoreBoardManager::Entry::Entry(uint32_t inPlayerId, const std::string &inName, const Vector3 &inColor)
    : mPlayerId(inPlayerId),
      mName(inName),
      mColor(inColor)
{
    SetScore(0);
}

void ScoreBoardManager::Entry::SetScore(int inScore)
{
    mScore = inScore;
    char buffer[256];
    sprintf(buffer, "%s: %d", mName.c_str(), mScore);
    mFormattedNameScore = string(buffer);
}

ScoreBoardManager::Entry *ScoreBoardManager::GetEntry(uint32_t inPlayerId)
{
    for (Entry &entry : mEntries)
    {
        if (entry.GetPlayerId() == inPlayerId)
        {
            return &entry;
        }
    }

    return nullptr;
}

bool ScoreBoardManager::RemoveEntry(uint32_t inPlayerId)
{
    for (auto it = mEntries.begin(), end = mEntries.end(); it != end; ++it)
    {
        if (it->GetPlayerId() == inPlayerId)
        {
            mEntries.erase(it);
            return true;
        }
    }

    return false;
}

void ScoreBoardManager::AddEntry(uint32_t inPlayerId, const std::string &inName)
{
    // 如果这个玩家id已经存在，先删除它-拥有两个相同的id是疯狂的
    RemoveEntry(inPlayerId);

    mEntries.emplace_back(inPlayerId, inName, mDefaultColors[inPlayerId % mDefaultColors.size()]);
}

void ScoreBoardManager::IncScore(uint32_t inPlayerId, int inAmount) // 增加分数
{
    Entry *entry = GetEntry(inPlayerId);
    if (entry)
    {
        entry->SetScore(entry->GetScore() + inAmount);
    }
}

bool ScoreBoardManager::Write(OutputMemoryBitStream &inStream) const
{
    int entryCount = mEntries.size();

    // 我们不知道我们的玩家名字，所以很难检查数据包中剩余的空间...
    // 不过现在不是一个问题
    inStream.Write(entryCount);

    for (const Entry &entry : mEntries)
    {
        entry.Write(inStream);
    }

    return true;
}

bool ScoreBoardManager::Entry::Write(OutputMemoryBitStream &inStream) const
{
    bool didSucceed = true;

    inStream.Write(mColor);
    inStream.Write(mPlayerId);
    inStream.Write(mName);
    inStream.Write(mScore);

    return didSucceed;
}

bool ScoreBoardManager::Read(InputMemoryBitStream &inStream)
{
    int entryCount;
    inStream.Read(entryCount);
    // 只需替换这里的所有内容，这不重要...
    mEntries.resize(entryCount);
    for (Entry &entry : mEntries)
    {
        entry.Read(inStream);
    }

    return true;
}

bool ScoreBoardManager::Entry::Read(InputMemoryBitStream &inStream)
{
    bool didSucceed = true;

    inStream.Read(mColor);
    inStream.Read(mPlayerId);
    inStream.Read(mName);
    inStream.Read(mScore);

    int score;
    inStream.Read(score);
    if (didSucceed)
    {
        SetScore(score);
    }

    return didSucceed;
}