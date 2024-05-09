#ifndef SOCKETUTIL_H
#define SOCKETUTIL_H

#include <vector>
#include <sys/select.h>
#include "UDPSocket.h"
#include "TCPSocket.h"


enum SecketAdressFamily
{
  INET = AF_INET,
  INET6 = AF_INET6
};

class SocketUtil
{
public:
  static bool StaticInit();
  static void  CleanUp();

  static UDPSocketPtr CreateUDPSocket(SecketAdressFamily inFamily);
  static TCPSocketPtr CreateTCPSocket(SecketAdressFamily inFamily);
 
  static int Select(const std::vector<TCPSocketPtr> *inReadSet,
                      std::vector<TCPSocketPtr> *outReadSet,
                      const std::vector<TCPSocketPtr> *inWriteSet,
                      std::vector<TCPSocketPtr> *outWriteSet,
                      const std::vector<TCPSocketPtr> *inExceptSet,
                      std::vector<TCPSocketPtr> *outExceptSet);

 private:
  inline static fd_set* FillSetFromVector(fd_set &outSet, const std::vector<TCPSocketPtr> *inSockets,int &ioNaxNfds);
  inline static void FillSetFromSet(std::vector<TCPSocketPtr> *outSockets, const std::vector<TCPSocketPtr> *inSockets, const fd_set &inSet);
};




#endif // SOCKETUTIL_H