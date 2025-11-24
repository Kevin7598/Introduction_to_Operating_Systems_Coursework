#ifndef SRC_UTILS_FORMATTER_H_
#define SRC_UTILS_FORMATTER_H_

#ifdef __cplusplus
#include <cstddef>
#include <string>
#include <vector>

template <typename T>
[[nodiscard]] static inline auto
fmt_to_string([[maybe_unused]] const std::vector<T> &vec) -> std::string {
  std::string str;
  for (const auto &val : vec) {
    str += std::to_string(val) + " ";
  }
  return str;
}

template <typename T> static inline auto to_string(T str) -> std::string {
  return std::to_string(str);
}

template <> inline auto to_string(std::string str) -> std::string {
  return str;
}

template <typename T>
inline auto operator%(std::string format, T str) -> std::string {
  auto ind = format.find('?');
  if (ind == 0 || format[ind - 1] != '\\') {
    format.replace(ind, 1U, to_string(str));
  }
  return format;
}

template <>
inline auto operator%([[maybe_unused]] std::string format,
                      [[maybe_unused]] const std::string &str) -> std::string {
  auto ind = format.find('?');
  if (ind == 0 || format[ind - 1] != '\\') {
    format.replace(ind, 1U, str);
  }
  return format;
}

template <>
inline auto operator%([[maybe_unused]] std::string format,
                      [[maybe_unused]] const char *str) -> std::string {
  auto ind = format.find('?');
  if (ind == 0 || format[ind - 1] != '\\') {
    format.replace(ind, 1U, str);
  }
  return format;
}

inline auto operator""_f(const char *str, size_t size) -> std::string {
  (void)size;
  return {str};
}

#endif  // __cplusplus
#endif  // SRC_UTILS_FORMATTER_H_
