#ifndef CONVERTERS_H
#define CONVERTERS_H

#include <charconv>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "configuration.h"
#include "end_point.h"

namespace load_balancer::config {

/**
 * \brief Преобразователь строки в список значений заданного типа.
 */
template <typename T>
struct StringConverter<std::vector<T>> {
  using ParsingType = std::vector<T>;
  std::optional<ParsingType> operator()(const std::string &str_value) const;
};

/**
 * \brief Преобразователь строки в указанный числовой тип.
 */
template <typename Number>
requires(std::is_arithmetic_v<Number>) struct StringConverter<Number> {
  std::optional<Number> operator()(const std::string &str_value) const;
};

/**
 * \brief Преобразователь строки в конечную точку.
 */
template <typename Proto>
struct StringConverter<socket_wrapper::EndPoint<Proto>> {
  using ParsingType = socket_wrapper::EndPoint<Proto>;
  std::optional<ParsingType> operator()(const std::string &str_value) const;
};

template <typename T>
std::optional<typename StringConverter<std::vector<T>>::ParsingType>
StringConverter<std::vector<T>>::operator()(const std::string &str_value) const {
  ParsingType result;
  StringConverter<T> element_parser;
  for (const auto part : std::views::split(str_value, std::string_view(","))) {
    auto element = element_parser(std::string(part.begin(), part.end()));
    if (!element) {
      return std::nullopt;
    }
    result.emplace_back(*element);
  }
  return result;
}

template <typename Number>
requires(std::is_arithmetic_v<Number>)
    std::optional<Number> StringConverter<Number>::operator()(const std::string &str_value) const {
  Number value;
  const auto [_, ec] =
      std::from_chars(str_value.data(), str_value.data() + str_value.size(), value);
  if (ec == std::errc()) {
    return value;
  }
  return std::nullopt;
}

template <typename Proto>
std::optional<typename StringConverter<socket_wrapper::EndPoint<Proto>>::ParsingType>
StringConverter<socket_wrapper::EndPoint<Proto>>::operator()(const std::string &str_value) const {
  using Result = socket_wrapper::EndPoint<Proto>;
  const auto divider = str_value.find(':');
  if (divider == std::string::npos) {
    return std::nullopt;
  }
  const auto address = str_value.substr(0, divider);
  const auto str_port = str_value.substr(divider + 1);
  const auto port = StringConverter<std::uint16_t>()(str_port);
  if (!port) {
    return std::nullopt;
  }
  try {
    return Result(address, *port);
  } catch (...) {
    return std::nullopt;
  }
}

}  // namespace load_balancer::config

#endif  // CONVERTERS_H
