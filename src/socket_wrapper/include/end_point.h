#ifndef END_POINT_H
#define END_POINT_H

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <charconv>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>

namespace socket_wrapper {

/**
 * \brief Конечная точка, определяющая адрес в указанном протоколе.
 *
 * \tparam Proto тип протокола (см. @link Protocol @endlink).
 */
template <typename Proto>
class EndPoint {
 public:
  using ProtocolType = Proto;

  /**
   * \brief Преобразовать закодированный адрес конечной точки в экзмепляр @link EndPoint @endlink.
   */
  static EndPoint ParseEndPoint(sockaddr addr, socklen_t addr_len);

  EndPoint(const EndPoint &other);
  EndPoint(EndPoint &&other) noexcept;
  EndPoint(const std::string &address, uint16_t port);
  explicit EndPoint(uint16_t port = 0);
  ~EndPoint();

  EndPoint &operator=(EndPoint other);

  [[nodiscard]] const std::string &GetAddress() const;
  [[nodiscard]] const std::string &GetPort() const;
  [[nodiscard]] std::weak_ptr<const sockaddr> GetAddressImpl() const;
  [[nodiscard]] socklen_t GetAddressLen() const;

  friend std::ostream &operator<<(std::ostream &os, const EndPoint &end_point) {
    return os << end_point.address_ << ":" << end_point.port_;
  }

 private:
  struct AddrinfoDeleter {
    void operator()(addrinfo *addrinfo) const;
  };

  std::string address_;
  std::string port_;
  std::unique_ptr<addrinfo, AddrinfoDeleter> addrinfo_;
  std::shared_ptr<sockaddr> sockaddr_;

  void SetFields(addrinfo *addrinfo);

  friend void Swap(EndPoint &first, EndPoint &second) {
    using std::swap;
    std::swap(first.sockaddr_, second.sockaddr_);
    std::swap(first.addrinfo_, second.addrinfo_);
    std::swap(first.address_, second.address_);
    std::swap(first.port_, second.port_);
  }
};

template <typename Proto>
EndPoint<Proto> EndPoint<Proto>::ParseEndPoint(const sockaddr addr, const socklen_t addr_len) {
  std::array<char, 50> address_buf = {};
  std::array<char, 6> port_buf = {};
  getnameinfo(
      &addr,
      addr_len,
      address_buf.data(),
      address_buf.size(),
      port_buf.data(),
      port_buf.size(),
      NI_NUMERICHOST | NI_NUMERICSERV
  );
  return EndPoint(
      std::string(address_buf.begin(), std::ranges::find(address_buf, '\0')), atoi(port_buf.data())
  );
}

template <typename Proto>
EndPoint<Proto>::EndPoint(const EndPoint &other)
    : EndPoint(other.GetAddress(), atoi(other.GetPort().c_str())) {
}

template <typename Proto>
EndPoint<Proto>::EndPoint(const std::string &address, const uint16_t port) {
  addrinfo hints = {};
  Proto protocol;
  hints.ai_family = static_cast<int>(protocol.family);
  hints.ai_socktype = static_cast<int>(protocol.socket_type);
  hints.ai_flags = AI_NUMERICSERV;
  hints.ai_protocol = static_cast<int>(protocol.name);
  addrinfo *addrinfo;
  if (const int err = getaddrinfo(address.data(), std::to_string(port).data(), &hints, &addrinfo)) {
    throw std::runtime_error(std::format("Can't create end point: {}", gai_strerror(err)));
  }
  SetFields(addrinfo);
}

template <typename Proto>
EndPoint<Proto>::~EndPoint() {
  sockaddr_.reset();
}

template <typename Proto>
EndPoint<Proto> &EndPoint<Proto>::operator=(EndPoint other) {
  Swap(*this, other);
  return *this;
}

template <typename Proto>
const std::string &EndPoint<Proto>::GetAddress() const {
  return address_;
}

template <typename Proto>
const std::string &EndPoint<Proto>::GetPort() const {
  return port_;
}

template <typename Proto>
std::weak_ptr<const sockaddr> EndPoint<Proto>::GetAddressImpl() const {
  return sockaddr_;
}

template <typename Proto>
socklen_t EndPoint<Proto>::GetAddressLen() const {
  return addrinfo_->ai_addrlen;
}

template <typename Proto>
void EndPoint<Proto>::AddrinfoDeleter::operator()(addrinfo *addrinfo) const {
  freeaddrinfo(addrinfo);
}

template <typename Proto>
void EndPoint<Proto>::SetFields(addrinfo *addrinfo) {
  addrinfo_.reset(addrinfo);
  sockaddr_.reset(addrinfo_->ai_addr, [](auto *) {});
  std::array<char, 32> address_buf{};
  address_buf.fill(0);
  std::array<char, 6> port_buf{};
  port_buf.fill(0);
  if (const int err = getnameinfo(
          addrinfo_->ai_addr,
          addrinfo_->ai_addrlen,
          address_buf.data(),
          address_buf.size(),
          port_buf.data(),
          port_buf.size(),
          NI_NUMERICHOST | NI_NUMERICSERV
      )) {
    throw std::runtime_error(std::format("Can't create end point: {}", gai_strerror(err)));
  }
  address_.append(address_buf.begin(), std::ranges::find(address_buf, '\0'));
  port_.append(port_buf.begin(), std::ranges::find(port_buf, '\0'));
}

template <typename Proto>
EndPoint<Proto>::EndPoint(const uint16_t port) {
  addrinfo hints = {};
  Proto protocol;
  hints.ai_family = static_cast<int>(protocol.family);
  hints.ai_socktype = static_cast<int>(protocol.socket_type);
  hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
  hints.ai_protocol = static_cast<int>(protocol.name);
  addrinfo *addrinfo;
  if (const int err = getaddrinfo(nullptr, std::to_string(port).data(), &hints, &addrinfo)) {
    throw std::runtime_error(std::format("Can't create end point: {}", gai_strerror(err)));
  }
  SetFields(addrinfo);
}

template <typename Proto>
EndPoint<Proto>::EndPoint(EndPoint &&other) noexcept {
  Swap(*this, other);
}

}  // namespace socket_wrapper

#endif  // END_POINT_H
