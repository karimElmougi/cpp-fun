#pragma once

#include <deque>

#include "memory.hpp"
#include "monitor.hpp"
#include "optional.hpp"
#include "semaphore.hpp"

namespace ift630 {

/// Unbounded queue that never blocks the callers of push() and pop() for longer than the time of
/// insertion into the queue
template <typename T>
class AsyncQueue {
  Monitor<std::deque<T>> monitor{};

 public:
  AsyncQueue() noexcept = default;
  AsyncQueue(AsyncQueue&&) noexcept = default;
  AsyncQueue(const AsyncQueue&) = delete;
  ~AsyncQueue() noexcept = default;

  auto operator=(AsyncQueue&&) noexcept -> AsyncQueue& = default;
  auto operator=(const AsyncQueue&) -> AsyncQueue& = delete;

  void push(const T& item) {
    constexpr auto push_fn = [&item](auto& q) { q.push_back(item); };
    monitor(push_fn);
  }

  void push(T&& item) {
    const auto push_fn = [&item](auto& q) { q.push_back(std::forward<T>(item)); };
    monitor(push_fn);
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    const auto emplace_fn = [&args...](auto& q) { q.emplace_back(std::forward<Args>(args)...); };
    monitor(emplace_fn);
  }

  auto pop() noexcept -> optional<T> {
    constexpr auto pop_front_fn = [](auto& q) -> optional<T> {
      if (q.empty()) {
        return none;
      }
      const auto item = T{q.front()};
      q.pop_front();
      return item;
    };
    return monitor(pop_front_fn);
  }

  [[nodiscard]] auto empty() const noexcept -> bool {
    constexpr auto is_empty_fn = [](const auto& q) { return q.empty(); };
    return monitor(is_empty_fn);
  }
};

/// Bounded queue which blocks the callers of pop() if the queue is empty, and the callers of push()
/// when the queue is full
template <typename T>
class SyncQueue {
  Monitor<std::deque<T>> monitor{};
  MoveSemaphore full_semaphore;
  MoveSemaphore empty_semaphore;
  const size_t capacity;

 public:
  explicit SyncQueue(size_t size) noexcept : full_semaphore{size}, capacity{size} {}
  SyncQueue(const SyncQueue&) = delete;
  SyncQueue(SyncQueue&&) noexcept = default;
  ~SyncQueue() noexcept = default;

  auto operator=(const SyncQueue&) -> SyncQueue& = delete;
  auto operator=(SyncQueue&&) noexcept -> SyncQueue& = default;

  void push(const T& item) {
    full_semaphore.wait();
    const auto push_fn = [&item](auto& q) { q.push_back(item); };
    monitor(push_fn);
    empty_semaphore.signal();
  }

  void push(T&& item) {
    full_semaphore.wait();
    const auto push_fn = [&item](auto& q) { q.push_back(std::forward<T>(item)); };
    monitor(push_fn);
    empty_semaphore.signal();
  }

  template <typename... Args>
  void emplace(Args&&... args) {
    full_semaphore.wait();
    const auto emplace_fn = [&args...](auto& q) { q.emplace_back(std::forward<Args>(args)...); };
    monitor(emplace_fn);
    empty_semaphore.signal();
  }

  auto pop() noexcept -> T {
    empty_semaphore.wait();
    constexpr auto pop_front_fn = [](auto& q) {
      const auto item = T{q.front()};
      q.pop_front();
      return item;
    };
    const auto item = monitor(pop_front_fn);
    full_semaphore.signal();
    return item;
  }

  auto try_pop() noexcept -> optional<T> {
    constexpr auto pop_front_fn = [](auto& q) -> optional<T> {
      if (q.empty()) {
        return none;
      }
      const auto item = T{q.front()};
      q.pop_front();
      return item;
    };
    const auto item = monitor(pop_front_fn);
    if (item.has_value()) {
      full_semaphore.signal();
    }
    return item;
  }

  [[nodiscard]] auto empty() const noexcept -> bool {
    constexpr auto is_empty_fn = [](const auto& q) { return q.empty(); };
    return monitor(is_empty_fn);
  }

  [[nodiscard]] auto full() const noexcept -> bool {
    constexpr auto is_full_fn = [capacity = this->capacity](const auto& q) {
      return q.size() == capacity;
    };
    return monitor(is_full_fn);
  }
};

}  // namespace ift630
