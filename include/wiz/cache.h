#ifndef WIZ_CACHE_H
#define WIZ_CACHE_H

#include <chrono>
#include <string>
#include <tuple>
#include <unordered_map>

namespace wiz {
template <typename T> class cache {
public:
  cache(std::chrono::seconds ttl) : ttl_(ttl) {}

  void put(const std::string &key, const T &value) {
    cache_[key] = {value, std::chrono::system_clock::now() + ttl_};
  }

  std::tuple<bool, T> get(const std::string &key) {
    auto it = cache_.find(key);
    if (it == cache_.end()) {
      return {false, T()};
    }

    auto &data = it->second;
    if (std::chrono::system_clock::now() > data.timestamp) {
      cache_.erase(it);
      return {false, T()};
    }

    return {true, data.value};
  }

private:
  struct data {
    T value;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
  };

  std::unordered_map<std::string, data> cache_;
  std::chrono::seconds ttl_;
};
} // namespace wiz

#endif // WIZ_CACHE_H