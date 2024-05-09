#include "SocketUtil.h"

bool SocketUtil::StaticInit()
{
  return true;
}

void SocketUtil::CleanUp()
{
}

fd_set *SocketUtil::FillSetFromVector(fd_set &outSet, const std::vector<TCPSocketPtr> *inSockets, int &ioNaxNfds)
{
  if (inSockets)
  {
    FD_ZERO(&outSet);

    for (const TCPSocketPtr &socket : *inSockets)
    {
      FD_SET(socket->mSocket, &outSet);
      ioNaxNfds = std::max(ioNaxNfds, socket->mSocket);
    }
    return &outSet;
  }
  else
  {
    return nullptr;
  }
}

void SocketUtil::FillSetFromSet(std::vector<TCPSocketPtr> *outSockets, const std::vector<TCPSocketPtr> *inSockets, const fd_set &inSet)
{
  if (inSockets && outSockets)
  {
    outSockets->clear();
    for (const TCPSocketPtr &socket : *inSockets)
    {
      if (FD_ISSET(socket->mSocket, &inSet))
      {
        outSockets->push_back(socket);
      }
    }
  }
}

int SocketUtil::Select(const std::vector<TCPSocketPtr> *inReadSet,
                       std::vector<TCPSocketPtr> *outReadSet,
                       const std::vector<TCPSocketPtr> *inWriteSet,
                       std::vector<TCPSocketPtr> *outWriteSet,
                       const std::vector<TCPSocketPtr> *inExceptSet,
                       std::vector<TCPSocketPtr> *outExceptSet)
{
  fd_set read, write, except;
  int nfds = 0;

  fd_set *readPtr = FillSetFromVector(read, inReadSet, nfds);
  fd_set *writePtr = FillSetFromVector(write, inWriteSet, nfds);
  fd_set *exceptPtr = FillSetFromVector(except, inExceptSet, nfds);

  int toRet = select(nfds + 1, readPtr, writePtr, exceptPtr, nullptr);

  if (toRet > 0)
  {
    FillSetFromSet(outReadSet, inReadSet, read);
    FillSetFromSet(outWriteSet, inWriteSet, write);
    FillSetFromSet(outExceptSet, inExceptSet, except);
  }

  return toRet;
}


UDPSocketPtr SocketUtil::CreateUDPSocket(SecketAdressFamily inFamily)
{
  int s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);
  if (s != -1)
  {
    return UDPSocketPtr(new UDPSocket(s));
  }
  return nullptr;
}

TCPSocketPtr SocketUtil::CreateTCPSocket(SecketAdressFamily inFamily)
{
  int s = socket(inFamily, SOCK_STREAM, IPPROTO_TCP);
  if (s != -1)
  {
    return TCPSocketPtr(new TCPSocket(s));
  }
  return nullptr;
}

