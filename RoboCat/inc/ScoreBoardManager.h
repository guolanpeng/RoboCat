#ifndef SCOREBOARDMANAGER_H
#define SCOREBOARDMANAGER_H
#include "RoboCatShared.h"

class ScoreBoardManager
{
public:
    static void StaticInit();

    static std::unique_ptr<ScoreBoardManager> sInstance;

    class Entry
    {
    public:
        Entry() {}
        Entry(uint32_t inPlayerId, const std::string &inName, const Vector3 &inColor);

        const Vector3 &GetColor() const { return mColor; }
        uint32_t GetPlayerId() const { return mPlayerId; }
        const std::string &GetName() const { return mName; }
        const std::string &GetNameScore() const { return mFormattedNameScore; }
        int GetScore() const { return mScore; }

        void SetScore(int inScore);

        bool Write(OutputMemoryBitStream &inStream) const;
        bool Read(InputMemoryBitStream &inStream);
        static uint32_t GetSerializedSize();

    private:
        Vector3 mColor;

        uint32_t mPlayerId;
        std::string mName;

        int mScore;
        std::string mFormattedNameScore; // 这是一个格式化的字符串，包含了玩家的名字和分数
    };

    Entry *GetEntry(uint32_t inPlayerId);
    bool RemoveEntry(uint32_t inPlayerId);
    void AddEntry(uint32_t inPlayerId, const std::string &inName);
    void IncScore(uint32_t inPlayerId, int inAmount);

    bool Write(OutputMemoryBitStream &inStream) const;
    bool Read(InputMemoryBitStream &inStream);

    const vector<Entry> &GetEntries() const { return mEntries; }

private:
    ScoreBoardManager();

    vector<Entry> mEntries;

    vector<Vector3> mDefaultColors;
};

#endif