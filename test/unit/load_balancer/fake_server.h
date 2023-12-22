#ifndef FAKE_SERVER_H
#define FAKE_SERVER_H

#include <string>
#include <vector>

#include "load_balancer.h"

namespace load_balancer::test {
/**
 * \brief Класс, имитирующий реальный сервер.
 *
 * Сохраняет все полученные сообщения для дальнейшей проверки в тестах.
 */
class FakeServer {
 public:
  using SocketType = LoadBalancer::SocketType;
  using EndPointType = LoadBalancer::EndPointType;

  explicit FakeServer(std::uint16_t port);
  ~FakeServer();

  [[nodiscard]] const std::vector<std::pair<std::string, EndPointType>> &GetReceived() const;
  [[nodiscard]] EndPointType GetEndPoint() const;

 private:
  SocketType socket_;
  std::jthread thread_;
  std::vector<std::pair<std::string, EndPointType>> received_;
  mutable std::mutex mutex_;

  void Worker();
};

}  // namespace load_balancer::test

#endif  // FAKE_SERVER_H
