#ifndef LWWREG_H
#define LWWREG_H

#include <chrono>
#include <iterator>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>

namespace crdt {

template <typename V> struct lwwreg {
  using self_type = lwwreg<V>;

  lwwreg() {}
  explicit lwwreg(std::uint64_t replicaID) : _replicaID(replicaID) {}
  lwwreg(std::uint64_t replicaID, V value)
      : _replicaID(replicaID), _value(value),
        _timestamp(std::chrono::high_resolution_clock::now()) {}
  lwwreg(const lwwreg<V> &) = default;
  lwwreg(lwwreg<V> &&) = delete;

  auto operator=(const self_type &other) -> self_type & {
    merge(other);
    return (*this);
  };
  lwwreg<V> &operator=(lwwreg<V> &&) = delete;

  auto set(V value) noexcept -> /* delta */ self_type {
    _value = value;
    _timestamp = std::chrono::high_resolution_clock::now();
    return *this;
  }

  void merge(const self_type &delta) noexcept {
    if (_timestamp <= delta._timestamp) {
      _value = delta._value;
      _timestamp = delta._timestamp;
    }
  }

  auto read() const noexcept -> V { return _value; }

  auto operator==(const self_type &other) const noexcept -> bool {
    return this->read() == other.read();
  }

private:
  using timestamp_type =
      std::chrono::time_point<std::chrono::high_resolution_clock>;
  std::uint64_t _replicaID;
  V _value;
  timestamp_type _timestamp;
};

} // namespace crdt.
  //
#endif // !LWWREG_H
