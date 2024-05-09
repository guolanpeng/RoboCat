#ifndef ENGINE_H
#define ENGINE_H
#include <memory>

class Engine
{
public:
  virtual ~Engine();
  static std::unique_ptr<Engine> sInstance;

  virtual int Run();
  void SetShouldKeepRunning(bool inShouldKeepRunning) { mShouldKeepRunning = inShouldKeepRunning; }

protected:
  Engine();

  virtual void DoFrame();

private:
  int DoRunLoop();

  bool mShouldKeepRunning;
};

#endif // ENGINE_H