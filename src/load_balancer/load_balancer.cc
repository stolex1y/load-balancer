#include "load_balancer.h"

#include <mutex>

#include "configuration/converters.h"
#include "invalid_socket_exception.h"

using namespace std::chrono_literals;

namespace load_balancer {

LoadBalancer::LoadBalancer(std::shared_ptr<config::Configuration> configuration)
    : configuration_(std::move(configuration)) {
  UpdateConfigParameters();

  receiver_ = SocketType(receiver_port_);
  sender_ = SocketType(sender_port_);
}

void LoadBalancer::Start() {
  bool was_stopped = true;
  if (!stopped_.compare_exchange_strong(was_stopped, false)) {
    return;
  }
  for (std::size_t i = 0; i < thread_count_; ++i) {
    threads_.emplace_back([this] {
      Worker();
      std::cout << "End receiving in thread\n";
    });
  }
}

void LoadBalancer::Stop() {
  bool was_stopped = false;
  if (!stopped_.compare_exchange_strong(was_stopped, true)) {
    return;
  }
  stopped_.notify_all();
  receiver_.Close();
  threads_.clear();
  sender_.Close();
}

void LoadBalancer::Join() const {
  stopped_.wait(false);
}

void LoadBalancer::Worker() {
  while (true) {
    try {
      const auto [datagram, sender] = receiver_.ReceiveFrom();
      if (!AddRequest()) {
        continue;
      }
      const auto server_idx = GetNextServerIndex();
      sender_.SendTo(datagram, server_end_points_[server_idx]);
    } catch ([[maybe_unused]] const InvalidSocketException &ex) {
      std::cout << "Stop requests receiving.\n";
      return;
    } catch (const std::exception &ex) {
      std::cerr << "Error in load balancer: " << ex.what() << ".\n";
    } catch (...) {
      std::cerr << "Error in load balancer: uknown exception.\n";
    }
  }
}

void LoadBalancer::UpdateConfigParameters() {
  server_end_points_ = configuration_->GetParam(kServersKey, server_end_points_);
  max_rps_ = configuration_->GetParam(kMaxRpsKey, max_rps_);
  receiver_port_ = configuration_->GetParam(kReceiverPortKey, receiver_port_);
  sender_port_ = configuration_->GetParam(kSenderPortKey, sender_port_);
}

size_t LoadBalancer::GetNextServerIndex() {
  std::lock_guard lock(next_server_mutex_);
  const auto server_idx = next_server_;
  next_server_ = (next_server_ + 1) % server_end_points_.size();
  return server_idx;
}

bool LoadBalancer::AddRequest() {
  std::lock_guard lock(requests_mutex_);
  const auto now = Clock::now();
  while (!request_times_.empty() && now - request_times_.front() >= 1s) {
    request_times_.pop();
  }
  if (request_times_.size() >= max_rps_) {
    return false;
  }
  request_times_.emplace(now);
  return true;
}

}  // namespace load_balancer
