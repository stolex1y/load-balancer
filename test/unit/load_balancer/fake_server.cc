#include "fake_server.h"

#include <gtest/gtest.h>

namespace load_balancer::test {

FakeServer::FakeServer(const std::uint16_t port) : socket_(port) {
  thread_ = std::jthread([this] {
    Worker();
  });
}

FakeServer::~FakeServer() {
  socket_.Close();
}

const std::vector<std::pair<std::string, FakeServer::EndPointType>> &FakeServer::GetReceived(
) const {
  std::lock_guard lock(mutex_);
  return received_;
}

FakeServer::EndPointType FakeServer::GetEndPoint() const {
  return socket_.GetEndPoint();
}

void FakeServer::Worker() {
  while (true) {
    try {
      auto &&received = socket_.ReceiveFrom();
      std::lock_guard lock(mutex_);
      received_.emplace_back(received);
    } catch (InvalidSocketException &ex) {
      return;
    } catch (...) {
      ASSERT_TRUE(false) << "Catch exception in server.";
    }
  }
}

}  // namespace load_balancer::test
