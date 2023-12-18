#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <netinet/in.h>
#include <sys/socket.h>

namespace socket_wrapper {

/**
 * \brief Определяет множество возможных протоколов.
 */
enum class ProtocolName {
  kTcp = IPPROTO_TCP,  ///< Протокол TCP.
  kUdp = IPPROTO_UDP,  ///< Протокол UDP.
};

/**
 * \brief Тип сокета.
 */
enum class SocketType {
  kDatagram = SOCK_DGRAM,  ///< На основе двустороннего соединения.
  kStream = SOCK_STREAM,  ///< На основе датаграмм без установления соединения.
};

/**
 * \brief Семейство протоколов.
 */
enum class ProtocolFamily : int {
  kIpV4 = AF_INET,   ///< IPv4.
  kIpV6 = AF_INET6,  ///< IPv6.
};

/**
 * \brief Конкретный протокол, определяющий тип сокета, семейство протоколов.
 */
struct Protocol {
  const SocketType socket_type;
  const ProtocolFamily family;
  const ProtocolName name;

 protected:
  Protocol(const SocketType socket_type, const ProtocolFamily family, const ProtocolName name)
      : socket_type(socket_type), family(family), name(name) {
  }
};

}  // namespace socket_wrapper

#endif  // PROTOCOL_H
