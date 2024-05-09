#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "SocketAddress.h"
#include <unistd.h>
#include <fcntl.h>

// 类型安全的UDPSocket类
class UDPSocket
{
public:
  ~UDPSocket();
  int Bind(const SocketAddress &inBindAddress);
  int SendTo(const void *inData, int inLen, const SocketAddress &inTo);
  int ReceiveFrom(void *inBuffer, int inLen, SocketAddress &outFrom);
  // 设置阻塞模式
  int SetNonBlockingMode(bool inShouldBeNonBlocking);

private:
  friend class SocketUtil;
  UDPSocket(int inSocket) : mSocket(inSocket){};
  int mSocket;
};

typedef std::shared_ptr<UDPSocket> UDPSocketPtr;

#endif // UDPSOCKET_H