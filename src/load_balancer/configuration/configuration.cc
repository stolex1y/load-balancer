#include "configuration.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <ranges>
#include <utility>

namespace load_balancer::config {

Configuration::Configuration(std::string file_name) : file_name_(std::move(file_name)) {
  ReadParamsFromFile();
}

void Configuration::UpdateConfiguration() {
  ReadParamsFromFile();
}

void Configuration::ReadParamsFromFile() {
  std::lock_guard lock(mutex_);
  params_.clear();
  std::ifstream input(file_name_);
  if (!input) {
    return;
  }
  std::string key;
  while (getline(input, key, '=')) {
    if (!key.starts_with("#")) {
      std::string value;
      input >> value;
      if (input.fail()) {
        input.clear();
      } else {
        params_.emplace(key, value.substr(0, value.find('#')));
      }
    }
    input.ignore(std::numeric_limits<std::streamsize>::max(), input.widen('\n'));
  }
}

std::optional<std::any> Configuration::FindParam(const std::string &key) {
  std::shared_lock lock(mutex_);
  const auto value = params_.find(key);
  if (value != params_.end()) {
    return value->second;
  }
  return std::nullopt;
}

}  // namespace load_balancer::config
