#pragma once

#include "condition_variable.hpp"
#include "memory.hpp"

namespace ift630 {

class Semaphore final {
  ConditionVariable<size_t> cv;

 public:
  Semaphore() noexcept = default;
  explicit Semaphore(size_t initial_count) noexcept : cv{initial_count} {}
  Semaphore(const Semaphore&) = delete;
  Semaphore(Semaphore&&) = delete;
  ~Semaphore() noexcept = default;

  auto operator=(const Semaphore&) -> Semaphore& = delete;
  auto operator=(Semaphore &&) -> Semaphore& = delete;

  inline void wait() noexcept { enter(); }

  inline void enter() noexcept { p(); }

  inline void p() noexcept {
    cv.wait_mutate([](auto count) { return count != 0; }, [](auto& count) { --count; });
  }

  inline void signal() noexcept { exit(); }

  inline void exit() noexcept { v(); }

  inline void v() noexcept {
    cv.mutate_notify_one([](auto& count) { ++count; });
  }
};

class CopySemaphore final {
  shared_ptr<Semaphore> semaphore{};

 public:
  CopySemaphore() noexcept : semaphore{std::make_shared<Semaphore>()} {}
  explicit CopySemaphore(size_t initial_count) noexcept
      : semaphore{std::make_shared<Semaphore>(initial_count)} {}
  CopySemaphore(const CopySemaphore&) noexcept = default;
  CopySemaphore(CopySemaphore&&) noexcept = default;
  ~CopySemaphore() noexcept = default;

  auto operator=(const CopySemaphore&) noexcept -> CopySemaphore& = default;
  auto operator=(CopySemaphore&&) noexcept -> CopySemaphore& = default;

  inline void wait() const noexcept { enter(); }
  inline void enter() const noexcept { p(); }
  inline void p() const noexcept { semaphore->p(); }
  inline void signal() const noexcept { exit(); }
  inline void exit() const noexcept { v(); }
  inline void v() const noexcept { semaphore->v(); }
};

class MoveSemaphore final {
  unique_ptr<Semaphore> semaphore{};

 public:
  MoveSemaphore() noexcept : semaphore{std::make_unique<Semaphore>()} {}
  explicit MoveSemaphore(size_t initial_count) noexcept
      : semaphore{std::make_unique<Semaphore>(initial_count)} {}
  MoveSemaphore(const MoveSemaphore&) noexcept = delete;
  MoveSemaphore(MoveSemaphore&&) noexcept = default;
  ~MoveSemaphore() noexcept = default;

  auto operator=(const MoveSemaphore&) noexcept -> MoveSemaphore& = delete;
  auto operator=(MoveSemaphore&&) noexcept -> MoveSemaphore& = default;

  inline void wait() const noexcept { enter(); }
  inline void enter() const noexcept { p(); }
  inline void p() const noexcept { semaphore->p(); }
  inline void signal() const noexcept { exit(); }
  inline void exit() const noexcept { v(); }
  inline void v() const noexcept { semaphore->v(); }
};

};  // namespace ift630
