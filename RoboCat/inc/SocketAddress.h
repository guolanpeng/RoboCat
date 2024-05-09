#ifndef SOCKETADDRESS_H
#define SOCKETADDRESS_H

#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>

class SocketAddress
{
public:
  SocketAddress()
  {
    GetSockAddrIn()->sin_family = AF_INET;
    GetSockAddrIn()->sin_addr.s_addr = INADDR_ANY;
    GetSockAddrIn()->sin_port = 0;
  };

  SocketAddress(uint32_t inAddress, uint16_t port)
  {
    GetIP4Ref() = htonl(inAddress);
    // Set port in network byte order
    GetSockAddrIn()->sin_port = htons(port);
    // Set address family
    GetSockAddrIn()->sin_family = AF_INET;
  }

  SocketAddress(const sockaddr &sockAddr)
  {
    memcpy(&m_sockAddr, &sockAddr, sizeof(sockaddr));
  }

  bool operator==(const SocketAddress &other) const
  {
    return (m_sockAddr.sa_family == AF_INET &&
            GetSockAddrIn()->sin_port == other.GetSockAddrIn()->sin_port &&
            GetIP4Ref() == other.GetIP4Ref());
  }

  size_t GetHash() const
  {
    return (GetIP4Ref()) | ((GetSockAddrIn()->sin_port) << 13) | m_sockAddr.sa_family;
  }

  socklen_t GetSize() const { return sizeof(sockaddr); }

  std::string ToString() const
  {
    char buffer[64];
    inet_ntop(AF_INET, &GetIP4Ref(), buffer, 64);
    return std::string(buffer) + ":" + std::to_string(ntohs(GetSockAddrIn()->sin_port));
  } 

private:
  friend class UDPSocket;
  friend class TCPSocket;

  uint32_t &GetIP4Ref()
  {
    return GetSockAddrIn()->sin_addr.s_addr;
  }
  const uint32_t &GetIP4Ref() const
  {
    return GetSockAddrIn()->sin_addr.s_addr;
  }

  sockaddr_in *GetSockAddrIn()
  {
    return reinterpret_cast<sockaddr_in *>(&m_sockAddr);
  }

  const sockaddr_in *GetSockAddrIn() const
  {
    return reinterpret_cast<const sockaddr_in *>(&m_sockAddr);
  }

  sockaddr m_sockAddr;
};

typedef std::shared_ptr<SocketAddress> SocketAddressPtr;

namespace std
{
  template <> struct hash<SocketAddress>
  {
    size_t operator()(const SocketAddress &inAddress) const
    {
      return inAddress.GetHash();
    }
  };
}

#endif // SOCKETADDRESS_H