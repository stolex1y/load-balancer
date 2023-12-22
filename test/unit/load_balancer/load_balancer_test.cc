#include "load_balancer.h"

#include <gtest/gtest.h>

#include "fake_client.h"
#include "fake_configuration.h"
#include "fake_server.h"

namespace load_balancer::test {

using namespace socket_wrapper;
using namespace load_balancer::config;

using namespace std::chrono;
using namespace std::chrono_literals;

using SocketType = LoadBalancer::SocketType;
using EndPointType = LoadBalancer::EndPointType;

using Servers = std::vector<std::unique_ptr<FakeServer>>;

class LoadBalancerTest : public testing::Test {
 public:
  static constexpr size_t kReceiverPort = 60001;
  static constexpr size_t kSenderPort = 60002;

  std::shared_ptr<FakeConfiguration> config;
  std::unique_ptr<LoadBalancer> load_balancer;

  explicit LoadBalancerTest();

  void SetUpLoadBalancer();
  [[nodiscard]] Servers SetUpFakeServers(uint16_t port_start, size_t count) const;
};

LoadBalancerTest::LoadBalancerTest() : config(std::make_shared<FakeConfiguration>()) {
  config->SetReceiverPort(kReceiverPort);
  config->SetSenderPort(kSenderPort);
}

void LoadBalancerTest::SetUpLoadBalancer() {
  load_balancer = std::make_unique<LoadBalancer>(config);
  load_balancer->Start();
}

Servers LoadBalancerTest::SetUpFakeServers(const std::uint16_t port_start, const size_t count)
    const {
  Servers servers;
  std::vector<EndPointType> server_end_points;
  for (size_t i = 0, port = port_start; i < count; ++i, ++port) {
    servers.emplace_back(std::make_unique<FakeServer>(port));
    server_end_points.emplace_back(servers.back()->GetEndPoint());
  }
  config->SetServersAddresses(server_end_points);
  return servers;
}

static void VerifyServerRecivedCount(
    const Servers &servers, const size_t expected_count, const size_t expected_per_server_count
) {
  size_t actual_messages_count = 0;
  for (const auto &server : servers) {
    const auto cur_server_message_count = server->GetReceived().size();
    actual_messages_count += cur_server_message_count;
    EXPECT_EQ(expected_per_server_count, cur_server_message_count);
  }
  EXPECT_EQ(expected_count, actual_messages_count);
}

TEST_F(LoadBalancerTest, UniformLoadDistribution) {
  constexpr auto server_count = 10;
  constexpr auto server_port_start = 60010;
  constexpr auto message_count_per_server = 10;
  constexpr auto messages_count = server_count * message_count_per_server;

  const auto servers = SetUpFakeServers(server_port_start, server_count);
  config->SetMaxRps(SIZE_MAX);
  SetUpLoadBalancer();

  const FakeClient client(60000, load_balancer->ReceiverEndPoint());
  const auto messages = client.Send(messages_count);
  std::this_thread::sleep_for(1s);

  VerifyServerRecivedCount(servers, messages_count, message_count_per_server);
}

TEST_F(LoadBalancerTest, LoadLimitation) {
  constexpr auto server_count = 10;
  constexpr auto server_port_start = 60010;
  constexpr auto message_count_per_server = 10;
  constexpr auto max_rps = server_count * message_count_per_server;
  constexpr auto messages_count = max_rps;

  const auto servers = SetUpFakeServers(server_port_start, server_count);
  config->SetMaxRps(max_rps);
  SetUpLoadBalancer();

  const FakeClient client(60000, load_balancer->ReceiverEndPoint());
  const auto messages = client.Send(messages_count);
  const auto rejected = client.Send(messages_count);
  std::this_thread::sleep_for(1s);

  VerifyServerRecivedCount(servers, messages_count, message_count_per_server);
}

}  // namespace load_balancer::test
