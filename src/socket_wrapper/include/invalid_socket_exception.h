#ifndef INVALID_SOCKET_EXCEPTION_H
#define INVALID_SOCKET_EXCEPTION_H
#include <stdexcept>

namespace socket_wrapper {

/**
 * \brief Исключение, свидетельствующее об использовании невалидного дескриптора
 * (см. errno = @link EBADF @endlink).
 */
class InvalidSocketException : public std::runtime_error {
 public:
  explicit InvalidSocketException(const std::string &msg)
      : std::runtime_error(msg + " Socket isn't valid.") {
  }
};

}  // namespace socket_wrapper

#endif  // INVALID_SOCKET_EXCEPTION_H
