#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include <cstdint>
#include <queue>
#include <thread>

#include "configuration/configuration.h"
#include "udp_socket.h"

namespace load_balancer {

using namespace socket_wrapper;

/**
 * \brief Балансировщик нагрузки.
 *
 * Принмает UDP-датаграммы с определенного порта и перенаправляет их на несколько, указанных в
 * конфигурации, узлов. При этом нагрузка (количество входящих запросов) ограничивается заданной
 * частотой.
 */
class LoadBalancer {
 public:
  static constexpr auto ProtoFamily = ProtocolFamily::kIpV4;
  using EndPointType = udp::UdpEndPoint<ProtoFamily>;
  using SocketType = udp::UdpSocket<ProtoFamily>;

  /// Ключ в конфигурации, задающий значение максимального количества входящих запросов в секунду.
  static constexpr auto kMaxRpsKey = "max_rps";
  /// Значение максимального количества входящих запросов в секунду по умолчанию.
  static constexpr std::size_t kDefaultMaxRps = 1000;
  /// Ключ в конфигурации, задающий адреса серверов, принимающих запросы.
  static constexpr auto kServersKey = "servers";
  /// Ключ в конфигурации, задающий порт балансировщика, на который принимаются входящие запросы.
  static constexpr auto kReceiverPortKey = "receiver_port";
  /// Значение порта балансировщика, на который принимаются входящие запросы, по умолчанию.
  static constexpr std::uint16_t kDefaultReceiverPort = 10000;
  /// Ключ в конфигурации, задающий порт балансировщика, с которого перенаправляются принятые
  /// запросы.
  static constexpr auto kSenderPortKey = "sender_port";
  /// Значение порта балансировщика, с которого перенаправляются принятые запросы.
  static constexpr std::uint16_t kDefaultSenderPort = 10001;
  static constexpr std::size_t kDefaultThreadCount = 2;

  explicit LoadBalancer(std::shared_ptr<config::Configuration> configuration);
  LoadBalancer(const LoadBalancer &other) = delete;
  LoadBalancer &operator=(const LoadBalancer &other) = delete;
  ~LoadBalancer();

  /**
   * \brief Запустить балансировку нагрузки.
   */
  void Start();
  /**
   * \brief Остановить прием новых запросов.
   */
  void Stop();
  /**
   * \brief Ожидание остановки балансировки.
   */
  void Join() const;
  /**
   * \brief Конечная точка, с которой балансировщик принимает запросы.
   */
  EndPointType ReceiverEndPoint() const;
  /**
   * \brief Конечная точка, с которой балансировщик отправляет
   */
  EndPointType SenderEndPoint() const;

 private:
  using ServerEndPoints = std::vector<EndPointType>;
  using Clock = std::chrono::steady_clock;
  using TimePoint = std::chrono::time_point<Clock>;

  const std::shared_ptr<config::Configuration> configuration_;
  ServerEndPoints server_end_points_;
  std::size_t max_rps_ = kDefaultMaxRps;

  std::size_t next_server_ = 0;
  mutable std::mutex next_server_mutex_;

  std::uint16_t receiver_port_ = kDefaultReceiverPort;
  SocketType receiver_;

  std::uint16_t sender_port_ = kDefaultSenderPort;
  SocketType sender_;
  mutable std::mutex send_msg_mutex_;

  std::queue<TimePoint> request_times_;
  mutable std::mutex requests_mutex_;

  size_t thread_count_ = kDefaultThreadCount;
  std::vector<std::jthread> threads_;

  std::atomic_bool stopped_ = true;

  /**
   * \brief Прием и перенаправление запросов.
   */
  void Worker();
  /**
   * \brief Обновить значения параметров, значениями из конфигурации.
   */
  void UpdateConfigParameters();
  /**
   * \brief Получить индекс следующего сервера для перенаправления ему запроса.
   *
   * После этого индекс будет указывать на следующий за выбранным сервер.
   */
  size_t GetNextServerIndex();
  /**
   * \brief Добавить время получения нового запроса.
   * \return true - если добавление прошло успешно, false - если кол-во запросов, поступивших в
   * систему больше @link max_rps_ заданного@endlink.
   */
  bool AddRequest();
};

}  // namespace load_balancer

#endif  // LOAD_BALANCER_H
