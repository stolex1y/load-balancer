#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include "end_point.h"
#include "socket.h"
#include "udp.h"

namespace socket_wrapper::udp {

/**
 * \brief Класс, определяющий сокет для связи по UDP-протоколу.
 *
 * \tparam ProtoFamily семейство протоколов с возможными значениями IPv4, IPv6.
 */
template <ProtocolFamily ProtoFamily>
class UdpSocket : public Socket<UdpProtocol<ProtoFamily>> {
 public:
  using EndPointType = UdpEndPoint<ProtoFamily>;

  UdpSocket() = default;
  UdpSocket(const std::string &address, uint16_t port);
  explicit UdpSocket(uint16_t port);
  explicit UdpSocket(EndPointType end_point);

  UdpSocket(const UdpSocket &other) = delete;
  UdpSocket &operator=(const UdpSocket &other) = delete;

  /**
   * \brief Отправить сообщения указанному получателю.
   */
  void SendTo(const std::string &message, const UdpEndPoint<ProtoFamily> &receiver) const;
  /**
   * \brief Получить сообщение.
   * \param max_size максимальный размер принимаемого сообщения.
   * \return пара: сообщение - отправитель.
   */
  std::pair<std::string, EndPointType> ReceiveFrom(size_t max_size = 1024) const;

 private:
  using SocketType = Socket<UdpProtocol<ProtoFamily>>;
};

template <ProtocolFamily ProtoFamily>
UdpSocket<ProtoFamily>::UdpSocket(const std::string &address, uint16_t port)
    : SocketType(address, port) {
}

template <ProtocolFamily ProtoFamily>
UdpSocket<ProtoFamily>::UdpSocket(uint16_t port) : SocketType(port) {
}

template <ProtocolFamily ProtoFamily>
UdpSocket<ProtoFamily>::UdpSocket(EndPointType end_point) : SocketType(end_point) {
}

template <ProtocolFamily ProtoFamily>
void UdpSocket<ProtoFamily>::SendTo(
    const std::string &message, const UdpEndPoint<ProtoFamily> &receiver
) const {
  ssize_t send_count = 0;
  while (const ssize_t cur_send_cont = sendto(
             SocketType::socket_,
             message.data() + send_count,
             message.size() - send_count,
             0,
             receiver.GetAddressImpl().lock().get(),
             receiver.GetAddressLen()
         )) {
    if (cur_send_cont < 0) {
      throw std::runtime_error(std::format("Can't send: {}", strerror(errno)));
    }
    send_count += cur_send_cont;
  }
}

template <ProtocolFamily ProtoFamily>
std::pair<std::string, UdpEndPoint<ProtoFamily>> UdpSocket<ProtoFamily>::ReceiveFrom(
    const size_t max_size
) const {
  std::string buffer(max_size, '\0');
  sockaddr sender_addr = {};
  socklen_t sender_addr_len = sizeof(sender_addr);
  const int recv_count = recvfrom(
      SocketType::socket_, buffer.data(), buffer.size(), 0, &sender_addr, &sender_addr_len
  );
  if (recv_count < 0) {
    throw std::runtime_error(std::format("Can't recv: {}", strerror(errno)));
  }
  buffer.resize(recv_count);
  auto sender_end_point = EndPointType::ParseEndPoint(sender_addr, sender_addr_len);
  return std::make_pair(std::move(buffer), std::move(sender_end_point));
}

}  // namespace socket_wrapper::udp

#endif  // UDP_SOCKET_H
