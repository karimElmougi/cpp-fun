#pragma once

#include <utility>

#include "memory.hpp"
#include "optional.hpp"
#include "sync_queue.hpp"

namespace ift630 {

using std::pair;

template <typename T>
class Chan {
  shared_ptr<SyncQueue<T>> _queue;

 public:
  explicit Chan(shared_ptr<SyncQueue<T>> queue) noexcept : _queue{std::move(queue)} {}
  Chan(Chan&&) noexcept = default;
  Chan(const Chan&) = delete;
  virtual ~Chan() noexcept = default;

 protected:
  [[nodiscard]] auto queue() noexcept -> shared_ptr<SyncQueue<T>> { return _queue; }

 public:
  auto operator=(Chan&&) noexcept -> Chan& = default;
  auto operator=(const Chan&) -> Chan& = delete;
};

template <typename T>
class SendChan : private Chan<T> {
  using Chan<T>::queue;

 public:
  using Chan<T>::Chan;

  void send(const T& message) { queue()->push(message); }

  void send(T&& message) { queue()->push(std::forward<T>(message)); }

  auto operator<<(const T& message) -> SendChan& {
    send(message);
    return *this;
  }

  auto operator<<(T&& message) -> SendChan& {
    send(std::forward<T>(message));
    return *this;
  }
};

template <typename T>
class ReceiveChan : private Chan<T> {
  using Chan<T>::queue;

 public:
  using Chan<T>::Chan;

  auto receive() -> T { return queue()->pop(); }

  auto try_receive() -> optional<T> { return queue->try_pop(); }

  auto operator>>(T& item) -> ReceiveChan& {
    item = receive();
    return *this;
  }

  auto operator>>(optional<T>& item) -> ReceiveChan& {
    item = try_receive();
    return *this;
  }
};

/// Creates a pair of objects representing a Single Producer, Single Consummer (SPSC) channel
template <typename T>
auto make_chan(size_t size) noexcept -> pair<SendChan<T>, ReceiveChan<T>> {
  const auto queue = std::make_shared<SyncQueue<T>>(size);
  return {SendChan<T>{queue}, ReceiveChan<T>{queue}};
}

}  // namespace ift630
