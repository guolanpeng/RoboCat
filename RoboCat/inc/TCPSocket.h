#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "SocketAddress.h"

// 类型安全的TCPSOCKET类
class TCPSocket
{
public:
  ~TCPSocket(){};
  int Connect(const SocketAddress &inAddress);
  int Bind(const SocketAddress &inToAddress);
  int Listen(int inBackLog = 32);
  std::shared_ptr<TCPSocket> Accept(SocketAddress &inFromAddress);
  int Send(const void *inData, int inLen);
  int Receive(void *inBuffer, int inLen);

private:
  friend class SocketUtil;
  TCPSocket(int inSocket) : mSocket(inSocket) {}
  int mSocket;
};

typedef std::shared_ptr<TCPSocket> TCPSocketPtr;


#endif // SOCKETADDRESS_H