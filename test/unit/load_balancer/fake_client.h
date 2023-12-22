#ifndef FAKE_CLIENT_H
#define FAKE_CLIENT_H

#include "load_balancer.h"

namespace load_balancer::test {
/**
 * \brief Класс клиента, имитирующего нагрузку на серверы.
 */
class FakeClient {
 public:
  using SocketType = LoadBalancer::SocketType;
  using EndPointType = LoadBalancer::EndPointType;

  /**
   * \param port порт клиента;
   * \param load_balancer_ep адрес балансировщика нагрузки.
   */
  explicit FakeClient(uint16_t port, EndPointType load_balancer_ep);

  /**
   * \brief Отправить одно сообщение балансировщику нагрузки.
   */
  void Send(const std::string &message) const;
  /**
   * \brief Отправить заданное количество сообщений балансировщику.
   * \return отправленные сообщения.
   */
  [[nodiscard]] std::vector<std::string> Send(size_t count) const;

 private:
  SocketType socket_;
  EndPointType target_;
};

}  // namespace load_balancer::test

#endif  // FAKE_CLIENT_H
