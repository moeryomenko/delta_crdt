#ifndef LWWREG_H
#define LWWREG_H

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>

namespace crdt {

template <typename V>
struct lwwreg {
  using self_type = lwwreg<V>;

  lwwreg() = default;
  explicit lwwreg(std::uint64_t replicaID) : _replicaID(replicaID) {}
  lwwreg(std::uint64_t replicaID, V value) : _replicaID(replicaID) {
    set(std::move(value));
  }

  auto set(V value) -> self_type {
    if (_logical_time == std::numeric_limits<std::uint64_t>::max()) {
      throw std::overflow_error("LWW register clock overflow");
    }
    _value = std::move(value);
    _writer_replicaID = _replicaID;
    ++_logical_time;
    return *this;
  }

  void merge(const self_type& delta) {
    if (_logical_time < delta._logical_time ||
        (_logical_time == delta._logical_time &&
         _writer_replicaID < delta._writer_replicaID)) {
      _value = delta._value;
      _writer_replicaID = delta._writer_replicaID;
    }
    _logical_time = std::max(_logical_time, delta._logical_time);
  }

  auto read() const -> V { return _value; }

  auto logical_time() const -> std::uint64_t { return _logical_time; }

  static auto at_logical_time(std::uint64_t replicaID, V value,
                              std::uint64_t logical_time) -> self_type {
    self_type result(replicaID);
    result._value = std::move(value);
    result._logical_time = logical_time;
    result._writer_replicaID = replicaID;
    return result;
  }

  static auto next_write(std::uint64_t replicaID, V value,
                         std::uint64_t observed_logical_time) -> self_type {
    if (observed_logical_time == std::numeric_limits<std::uint64_t>::max()) {
      throw std::overflow_error("LWW register clock overflow");
    }
    return at_logical_time(replicaID, std::move(value),
                           observed_logical_time + 1);
  }

  auto operator==(const self_type& other) const -> bool {
    return _logical_time == other._logical_time &&
           _writer_replicaID == other._writer_replicaID &&
           _value == other._value;
  }

 private:
  std::uint64_t _replicaID = 0;
  V _value{};
  std::uint64_t _logical_time = 0;
  std::uint64_t _writer_replicaID = 0;
};

}  // namespace crdt.
   //
#endif  // !LWWREG_H
