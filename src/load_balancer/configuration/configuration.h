#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <any>
#include <functional>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace load_balancer::config {

/**
 * \brief Класс, выполняющий чтение конфигурации из файла.
 */
class Configuration {
 public:
  /// Имя файла с конфигурацией по умолчанию.
  static constexpr auto kDefaultFileName = "config.properties";

  explicit Configuration(std::string file_name = kDefaultFileName);
  Configuration(Configuration &other) = delete;
  Configuration &operator=(Configuration &other) = delete;
  virtual ~Configuration() = default;

  /**
   * \brief Получить значение свойства по ключу.
   */
  template <typename T>
  const T &GetParam(const std::string &key, const T &default_value);
  /**
   * \brief Повторно прочитать значения свойств из файла.
   */
  void UpdateConfiguration();

 protected:
  std::shared_mutex mutex_;
  std::string file_name_;
  std::unordered_map<std::string, std::any> params_;

 private:
  /**
   * \brief Прочитать значения параметров из файла.
   */
  void ReadParamsFromFile();
  /**
   * \brief Поиск значения параметра.
   * \return std::nullopt - если параметр с таким ключом не найден.
   */
  std::optional<std::any> FindParam(const std::string &key);
};

/**
 * \brief Преобразователь строки в указанный тип данных.
 * \tparam T целевой тип.
 */
template <typename T>
struct StringConverter {
  using ParsingType = T;
  std::optional<T> operator()(const std::string &str_value) const;
};

template <typename T>
const T &Configuration::GetParam(const std::string &key, const T &default_value) {
  auto value = FindParam(key);
  if (!value) {
    return default_value;
  }
  if (value->type() != typeid(std::string) && value->type() != typeid(T)) {
    return default_value;
  }
  if (value->type() == typeid(T)) {
    return any_cast<T &>(*value);
  }
  const auto str_value = any_cast<std::string &>(*value);
  const auto parsed_value = StringConverter<T>()(str_value);
  if (parsed_value) {
    std::lock_guard lock(mutex_);
    params_[key] = *parsed_value;
    return std::any_cast<T &>(params_[key]);
  }
  return default_value;
}

}  // namespace load_balancer::config

#endif  // CONFIGURATION_H
