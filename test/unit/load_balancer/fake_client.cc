#include "fake_client.h"

namespace load_balancer::test {

FakeClient::FakeClient(const uint16_t port, EndPointType load_balancer_ep)
    : socket_(port), target_(std::move(load_balancer_ep)) {
}

void FakeClient::Send(const std::string &message) const {
  socket_.SendTo(message, target_);
}

std::vector<std::string> FakeClient::Send(const size_t count) const {
  static size_t message_id = 1;
  std::vector<std::string> messages(count);
  for (size_t i = 0; i < count; ++i) {
    messages[i] = std::to_string(message_id++);
    Send(messages[i]);
  }
  return messages;
}

}  // namespace load_balancer::test
