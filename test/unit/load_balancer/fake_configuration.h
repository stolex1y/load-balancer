#ifndef FAKE_CONFIGURATION_H
#define FAKE_CONFIGURATION_H

#include "configuration/configuration.h"
#include "udp_socket.h"

namespace load_balancer::test {

using namespace socket_wrapper;

/**
 * \brief Конфигуратор параметров для тестов.
 */
class FakeConfiguration : public config::Configuration {
 public:
  void SetMaxRps(size_t rps);
  void SetServersAddresses(const std::vector<udp::UdpEndPoint<ProtocolFamily::kIpV4>> &end_points);
  void SetReceiverPort(uint16_t port);
  void SetSenderPort(uint16_t port);
};

}  // namespace load_balancer::test

#endif  // FAKE_CONFIGURATION_H
