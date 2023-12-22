#include "configuration/configuration.h"
#include "end_point.h"
#include "load_balancer.h"

using namespace load_balancer;
using namespace load_balancer::config;

int main() {
  const auto configuration = std::make_shared<Configuration>();
  LoadBalancer load_balancer(configuration);
  load_balancer.Start();
  load_balancer.Join();
}
