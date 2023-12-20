#ifndef SOCKET_H
#define SOCKET_H

#include <cerrno>
#include <cstring>

#include "end_point.h"

namespace socket_wrapper {

/**
 * \brief Класс, определяющий сокет с указанным протоколом.
 */
template <typename Proto>
class Socket {
 public:
  using EndPointType = EndPoint<Proto>;
  using ProtocolType = Proto;

  Socket();
  Socket(const std::string &address, uint16_t port);
  explicit Socket(uint16_t port);
  explicit Socket(EndPointType end_point);

  Socket(const Socket &other) = delete;
  Socket(Socket &&other) noexcept;
  Socket &operator=(const Socket &other) = delete;
  Socket &operator=(Socket &&other) noexcept;

  virtual ~Socket();

  /**
   * \brief Установить соедиение с указанным узлом.
   */
  void Connect(const EndPointType &end_point) const;
  /**
   * \brief Прочитать сообщение, содержащееся во внутреннем буфере, либо ожидать получения, если
   * буфер пуст.
   *
   * \param max_size максимальный размер сообщения.
   */
  [[nodiscard]] std::string Receive(size_t max_size = SIZE_MAX) const;
  /**
   * \brief Отправить сообщение узлу, с которым установлено соединение.
   */
  void Send(const std::string &message) const;
  /**
   * \brief Закрыть сокет.
   */
  void Close() const;
  /**
   * \brief Получить адрес сокета.
   */
  [[nodiscard]] const EndPointType &GetEndPoint() const;

 protected:
  EndPointType end_point_;
  int socket_;

  friend void Swap(Socket &first, Socket &second) {
    using std::swap;
    swap(first.end_point_, second.end_point_);
    swap(first.socket_, second.socket_);
  }

 private:
  /**
   * \brief Связать сокет с адресом @link end_point_ конечной точки@endlink.
   */
  void Bind();
};

template <typename Proto>
Socket<Proto>::Socket() : Socket(EndPointType()) {
}

template <typename Proto>
Socket<Proto>::Socket(const std::string &address, const uint16_t port)
    : Socket(EndPointType(address, port)) {
}

template <typename Proto>
Socket<Proto>::Socket(const uint16_t port) : Socket(EndPointType(port)) {
}

template <typename Proto>
Socket<Proto>::Socket(EndPointType end_point) : end_point_(std::move(end_point)) {
  const Proto protocol;
  socket_ = socket(
      static_cast<int>(protocol.family),
      static_cast<int>(protocol.socket_type),
      static_cast<int>(protocol.name)
  );
  if (socket_ < 0) {
    throw std::runtime_error(strerror(errno));
  }
  Bind();
}

template <typename Proto>
Socket<Proto>::Socket(Socket &&other) noexcept
    : end_point_(std::move(other.end_point_)), socket_(other.socket_) {
  other.socket_ = -1;
}

template <typename Proto>
Socket<Proto> &Socket<Proto>::operator=(Socket &&other) noexcept {
  Swap(*this, other);
  return *this;
}

template <typename Proto>
Socket<Proto>::~Socket() {
  close(socket_);
}

template <typename Proto>
void Socket<Proto>::Connect(const EndPointType &end_point) const {
  if (connect(socket_, end_point.GetAddressImpl().lock().get(), end_point.GetAddressLen())) {
    throw std::runtime_error(std::format(
        "Can't connect to end point ({}:{}): {}",
        end_point.GetAddress(),
        end_point.GetPort(),
        strerror(errno)
    ));
  }
}

template <typename Proto>
std::string Socket<Proto>::Receive(const size_t max_size) const {
  std::array<char, 1024> buffer{};
  size_t left = max_size;
  std::string msg;
  while (const ssize_t recv_count =
             recv(socket_, buffer.data(), std::min(left, buffer.size()), 0)) {
    if (recv_count < 0) {
      throw std::runtime_error(std::format("Can't recv: {}", strerror(errno)));
    }
    msg.append(buffer.data(), recv_count);
    left -= recv_count;
  }
  return msg;
}

template <typename Proto>
void Socket<Proto>::Send(const std::string &message) const {
  ssize_t send_count = 0;
  while (const ssize_t cur_send_cont =
             send(socket_, message.data() + send_count, message.size() - send_count, 0)) {
    if (cur_send_cont < 0) {
      throw std::runtime_error(std::format("Can't send: {}", strerror(errno)));
    }
    send_count += cur_send_cont;
  }
}

template <typename Proto>
void Socket<Proto>::Close() const {
  shutdown(socket_, SHUT_RDWR);
  close(socket_);
}

template <typename Proto>
const EndPoint<Proto> &Socket<Proto>::GetEndPoint() const {
  return end_point_;
}

template <typename Proto>
void Socket<Proto>::Bind() {
  if (bind(socket_, end_point_.GetAddressImpl().lock().get(), end_point_.GetAddressLen())) {
    throw std::runtime_error(std::format(
        "Can't bind to end point ({}:{}): {}",
        end_point_.GetAddress(),
        end_point_.GetPort(),
        strerror(errno)
    ));
  }
  sockaddr binded_addr = {};
  socklen_t binded_addr_len = sizeof(binded_addr);
  getsockname(socket_, &binded_addr, &binded_addr_len);
  end_point_ = EndPointType::ParseEndPoint(binded_addr, binded_addr_len);
}

}  // namespace socket_wrapper

#endif  // SOCKET_H
