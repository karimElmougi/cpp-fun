#pragma once

#include <functional>
#include <mutex>
#include <utility>

#include "memory.hpp"

namespace ift630 {

template <typename T>
class Monitor {
  using mutex = std::mutex;

  mutable unique_ptr<mutex> mtx{};
  T resource{};

 public:
  Monitor() noexcept : mtx{std::make_unique<mutex>()} {}
  explicit Monitor(T&& resource) noexcept
      : mtx{std::make_unique<mutex>()}, resource{std::move(resource)} {}
  template <typename... Args>
  explicit Monitor(Args&&... args) noexcept
      : mtx{std::make_unique<mutex>()}, resource(std::forward<Args>(args)...) {}
  Monitor(const Monitor&) = delete;
  Monitor(Monitor&& other) noexcept = default;
  ~Monitor() noexcept = default;

  auto operator=(const Monitor&) -> Monitor& = delete;
  auto operator=(Monitor&&) noexcept -> Monitor& = default;

  template <typename F>
  auto operator()(F f) {
    const auto _ = std::lock_guard{*mtx};
    return f(resource);
  }

  template <typename F>
  auto operator()(F f) const {
    const auto _ = std::lock_guard{*mtx};
    return f(resource);
  }

  template <typename WaitF, typename F>
  auto operator()(WaitF wait, F f) {
    auto lock = std::unique_lock{*mtx};
    wait(lock, resource);
    return f(resource);
  }

  template <typename WaitF, typename F>
  auto operator()(WaitF wait, F f) const {
    auto lock = std::unique_lock{*mtx};
    wait(lock, resource);
    return f(resource);
  }
};

template <typename T>
auto make_monitor(T&& resource) -> Monitor<T> {
  return Monitor{std::forward<T>(resource)};
}

template <typename T>
auto make_monitor(T& resource) -> Monitor<T> {
  return Monitor{resource};
}

}  // namespace ift630
