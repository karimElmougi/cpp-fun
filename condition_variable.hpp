#pragma once

#include <condition_variable>
#include <mutex>

namespace ift630 {

template <typename T>
class ConditionVariable final {
  T value{};
  mutable std::mutex mtx;
  mutable std::condition_variable cv{};

 public:
  ConditionVariable() noexcept = default;
  explicit ConditionVariable(T value) noexcept : value{value} {}
  explicit ConditionVariable(T&& value) noexcept : value{std::move(value)} {}
  template <typename... Args>
  explicit ConditionVariable(Args&&... args) noexcept : value{std::forward<Args>(args)...} {}
  ConditionVariable(const ConditionVariable&) = delete;
  ConditionVariable(ConditionVariable&&) = delete;
  ~ConditionVariable() noexcept = default;

  auto operator=(const ConditionVariable&) -> ConditionVariable& = delete;
  auto operator=(ConditionVariable &&) -> ConditionVariable& = delete;

  template <typename Query>
  auto query(Query query) {
    auto lock = std::unique_lock<std::mutex>{mtx};
    return query(value);
  }

  template <typename Pred>
  void wait(Pred pred) const {
    auto lock = std::unique_lock<std::mutex>{mtx};
    cv.wait(lock, [&]() { return pred(value); });
  }

  template <typename F>
  void mutate(F f) {
    auto lock = std::unique_lock<std::mutex>{mtx};
    f(value);
  }

  template <typename Pred, typename F>
  void wait_mutate(Pred pred, F f) {
    auto lock = std::unique_lock<std::mutex>{mtx};
    cv.wait(lock, [&]() { return pred(value); });
    f(value);
  }

  template <typename F>
  void mutate_notify_one(F f) {
    mutate(f);
    notify_one();
  }

  template <typename F>
  void mutate_notify_all(F f) {
    mutate(f);
    notify_all();
  }

  void notify_one() const noexcept { cv.notify_one(); }

  void notify_all() const noexcept { cv.notify_all(); }
};

template <>
class ConditionVariable<bool> final {
  bool value{};
  mutable std::mutex mtx;
  mutable std::condition_variable cv{};

 public:
  ConditionVariable() noexcept = default;
  explicit ConditionVariable(bool value) noexcept : value{value} {}
  ConditionVariable(const ConditionVariable&) = delete;
  ConditionVariable(ConditionVariable&&) = delete;
  ~ConditionVariable() noexcept = default;

  auto operator=(const ConditionVariable&) -> ConditionVariable& = delete;
  auto operator=(ConditionVariable &&) -> ConditionVariable& = delete;

  void wait_true() const {
    auto lock = std::unique_lock<std::mutex>{mtx};
    cv.wait(lock, [&]() { return value; });
  }

  void wait_false() const {
    auto lock = std::unique_lock<std::mutex>{mtx};
    cv.wait(lock, [&]() { return !value; });
  }

  void set_true() noexcept { value = true; }

  void set_true_notify_one() noexcept {
    set_true();
    notify_one();
  }

  void set_true_notify_all() noexcept {
    set_true();
    notify_all();
  }

  void set_false() { value = false; }

  void set_false_notify_one() noexcept {
    set_false();
    notify_one();
  }

  void set_false_notify_all() noexcept {
    set_false();
    notify_all();
  }

  void notify_one() const noexcept { cv.notify_one(); }

  void notify_all() const noexcept { cv.notify_all(); }
};

}  // namespace ift630
