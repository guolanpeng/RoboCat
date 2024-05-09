#include "TCPSocket.h"

int TCPSocket::Connect(const SocketAddress &inAddress)
{
  int err = connect(mSocket, &inAddress.m_sockAddr, inAddress.GetSize());
  if (err != 0)
  {
    return -1;
  }
  return 0;
}

int TCPSocket::Bind(const SocketAddress &inToAddress)
{
  int err = bind(mSocket, &inToAddress.m_sockAddr, inToAddress.GetSize());
  if (err != 0)
  {
    return -1;
  }
  return 0;
}

int TCPSocket::Listen(int inBackLog)
{
  int err = listen(mSocket, inBackLog);
  if (err != 0)
  {
    return -1;
  }
  return 0;
}

std::shared_ptr<TCPSocket> TCPSocket::Accept(SocketAddress &inFromAddress)
{
  socklen_t length = inFromAddress.GetSize();
  int newSocket = accept(mSocket, &inFromAddress.m_sockAddr, &length);
  if (newSocket != -1)
  {
    return TCPSocketPtr(new TCPSocket(newSocket));
  }
  return nullptr;
}

int TCPSocket::Send(const void *inData, int inLen)
{
  int byteSentCount = send(mSocket, inData, inLen, 0);
  if (byteSentCount <= 0)
  {
    return -1;
  }
  return byteSentCount;
}

int TCPSocket::Receive(void *inBuffer, int inLen)
{
  int readByteCount = recv(mSocket, inBuffer, inLen, 0);
  if (readByteCount <= 0)
  {
    return -1;
  }
  return readByteCount;
}