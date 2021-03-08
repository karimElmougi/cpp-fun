#pragma once

#include "memory.hpp"

namespace ift630 {

// Because std::vector<bool> is a specialized version of std::vector, indexing into it returns
// std::Bit_reference rather than bool&, which causes all kinds of problems, so here is a wrapper
// type that behaves exactly like a bool
struct _bool final {
 private:
  bool _value{};

 public:
  _bool() noexcept = default;
  _bool(bool value) noexcept : _value{value} {}  // NOLINT
  _bool(const _bool &) noexcept = default;
  _bool(_bool &&) noexcept = default;
  ~_bool() noexcept = default;

  auto operator=(const bool b) noexcept -> _bool & {
    _value = b;
    return *this;
  }
  auto operator=(const _bool &) noexcept -> _bool & = default;
  auto operator=(_bool &&) noexcept -> _bool & = default;

  [[nodiscard]] auto value() noexcept -> bool & { return _value; }
  [[nodiscard]] auto value() const noexcept -> const bool & { return _value; }

  operator bool() const noexcept { return _value; }  // NOLINT
  auto operator&() noexcept -> ptr<bool> { return &_value; }
  auto operator&() const noexcept -> ptr<const bool> { return &_value; }
};

}  // namespace ift630
