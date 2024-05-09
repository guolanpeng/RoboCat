#ifndef MOVELIST_H
#define MOVELIST_H
#include <deque>
#include "Move.h"

class MoveList
{
public:
  typedef std::deque<Move>::const_iterator const_iterator;
  typedef std::deque<Move>::const_reverse_iterator const_reverse_iterator;
  
  MoveList(): mLastMoveTimestamp(-1.f) {}
  
  const Move& AddMove(const InputState &inInputState, float inTimestamp);
  bool AddMove(const Move &inMove);
  
  void RemovedProcessedMoves(float inLastMoveProcessedTimestamp);//删除已经处理的Move
  
  float GetLastMoveTimestamp() const { return mLastMoveTimestamp; }
  
  const Move& GetLatestMove() const { return mMoves.back(); }

  void Clear() { mMoves.clear(); }
  bool hasMoves() const { return !mMoves.empty(); }
  int GetMoveCount() const { return mMoves.size(); }

  const_iterator begin() const { return mMoves.begin(); }
  const_iterator end() const { return mMoves.end(); }

  const Move& operator[](int i) const { return mMoves[i]; }
  private:
  float mLastMoveTimestamp;
  std::deque<Move> mMoves;
};

#endif //MOVELIST_H