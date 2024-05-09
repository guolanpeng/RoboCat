#include "UDPSocket.h"
#include "StringUtils.h"

int UDPSocket::Bind(const SocketAddress &inBindAddress)
{
  int err = bind(mSocket, &inBindAddress.m_sockAddr, inBindAddress.GetSize());
  if (err != 0)
  {
    return -1;
  }
  return 0;
}

int UDPSocket::SendTo(const void *inData, int inLen, const SocketAddress &inTo)
{
  int byteSentCount = sendto(mSocket, inData, inLen, 0, &inTo.m_sockAddr, inTo.GetSize());
  if (byteSentCount <= 0)
  {
    return -1;
  }
  return byteSentCount;
}

int UDPSocket::ReceiveFrom(void *inBuffer, int inLen, SocketAddress &outFrom)
{
  socklen_t fromLength = outFrom.GetSize();
  int readByteCount = recvfrom(mSocket, inBuffer, inLen, 0, &outFrom.m_sockAddr, &fromLength);
  
  return readByteCount;
}

int UDPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking)
{
  int flags = fcntl(mSocket, F_GETFL, 0);
  if (inShouldBeNonBlocking)
  {
    flags |= O_NONBLOCK;
  }
  else
  {
    flags &= ~O_NONBLOCK;
  }
  int result = fcntl(mSocket, F_SETFL, flags);
  if (result == -1)
  {
    // Handle error
    return -1;
  }
  else
  {
    // Success
    return 0;
  }
}

UDPSocket::~UDPSocket()
{
  close(mSocket);
}