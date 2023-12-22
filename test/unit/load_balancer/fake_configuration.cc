#include "fake_configuration.h"

#include "load_balancer.h"
#include "udp_socket.h"

namespace load_balancer::test {

void FakeConfiguration::SetMaxRps(size_t rps) {
  params_[LoadBalancer::kMaxRpsKey] = rps;
}

void FakeConfiguration::SetServersAddresses(
    const std::vector<udp::UdpEndPoint<ProtocolFamily::kIpV4>> &end_points
) {
  params_[LoadBalancer::kServersKey] = end_points;
}

void FakeConfiguration::SetReceiverPort(uint16_t port) {
  params_[LoadBalancer::kReceiverPortKey] = port;
}

void FakeConfiguration::SetSenderPort(uint16_t port) {
  params_[LoadBalancer::kSenderPortKey] = port;
}

}  // namespace load_balancer::test
