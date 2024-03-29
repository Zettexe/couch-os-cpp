#ifndef WIZ_STRING_H
#define WIZ_STRING_H

#include <algorithm>
#include <string>
#include <vector>

namespace wiz {
class string : public std::string {
public:
  string() : std::string() {}
  string(const char *str) : std::string(str) {}
  string(const std::string &str) : std::string(str) {}

  bool starts_with(const std::string &search_string) const { return find(search_string) == 0; }
  bool is_int() const { return std::all_of(begin(), end(), ::isdigit); }

  string transform(int (*unary_operator)(int)) {
    string str = string(*this);
    std::transform(begin(), end(), str.begin(), unary_operator);
    return str;
  }

  string ltrim() {
    string str = string(*this);
    str.erase(str.begin(),
              std::find_if(str.begin(), str.end(), [](int ch) { return !std::isspace(ch); }));
    return str;
  }

  string rtrim() {
    string str = string(*this);
    str.erase(
      std::find_if(str.rbegin(), str.rend(), [](int ch) { return !std::isspace(ch); }).base(),
      str.end());
    return str;
  }

  string trim() { return ltrim().rtrim(); }

  std::vector<string> split(const std::string &delimiter) const {
    std::vector<string> tokens;
    std::size_t start = 0;
    std::size_t end = 0;
    while ((end = find(delimiter, start)) != std::string::npos) {
      tokens.push_back(substr(start, end - start));
      start = end + 1;
    }
    tokens.push_back(substr(start));
    return tokens;
  }

  std::vector<string> split(const char &delimiter) const {
    std::vector<string> tokens;
    std::size_t start = 0;
    std::size_t end = 0;
    while ((end = find(delimiter, start)) != std::string::npos) {
      tokens.push_back(substr(start, end - start));
      start = end + 1;
    }
    tokens.push_back(substr(start));
    return tokens;
  }

  bool contains(const std::string &search, size_t start_from = 0) const {
    return find(search, start_from) != std::string::npos;
  }

  bool contains(const char *search, size_t start_from = 0) const {
    return find(search, start_from) != std::string::npos;
  }

  bool contains(const char *search, size_t start_from, size_t count) const {
    return find(search, start_from, count) != std::string::npos;
  }

  bool contains(char search, size_t start_from = 0) const {
    return find(search, start_from) != std::string::npos;
  }

  string &operator=(const std::string &str) {
    std::string::operator=(str);
    return *this;
  }

  string &operator=(const char *str) {
    std::string::operator=(str);
    return *this;
  }
};
} // namespace wiz

#endif // WIZ_STRING_H