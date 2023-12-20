#ifndef SHUT_DOWN_SOCKET_EXCEPTION_H
#define SHUT_DOWN_SOCKET_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace socket_wrapper {
/**
 * \brief Исключение, свидетельствующие о том, что сокет отключил прием или передачу данных.
 */
class ShutDownSocketException : public std::runtime_error {
 public:
  explicit ShutDownSocketException(const std::string &msg)
      : std::runtime_error(msg + " Socket is shut down.") {
  }
};

}  // namespace socket_wrapper

#endif  // SHUT_DOWN_SOCKET_EXCEPTION_H
