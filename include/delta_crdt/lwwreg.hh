#ifndef LWWREG_H
#define LWWREG_H

#include <chrono>
#include <iterator>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>

namespace crdt {

template <typename V> struct lwwreg {
  lwwreg() {}
  explicit lwwreg(std::uint64_t replicaID) : _replicaID(replicaID) {}
  lwwreg(std::uint64_t replicaID, V value)
      : _replicaID(replicaID), _value(value),
        _timestamp(std::chrono::high_resolution_clock::now()) {}
  lwwreg(const lwwreg<V> &) = default;
  lwwreg(lwwreg<V> &&) = delete;

  auto operator=(const lwwreg<V> &other) -> lwwreg<V> & {
    merge(other);
    return (*this);
  };
  lwwreg<V> &operator=(lwwreg<V> &&) = delete;

  auto set(V value) noexcept -> /* delta */ lwwreg<V> {
    _value = value;
    _timestamp = std::chrono::high_resolution_clock::now();
    return *this;
  }

  void merge(lwwreg<V> delta) noexcept {
    if (_timestamp <= delta._timestamp) {
      _value = delta._value;
      _timestamp = delta._timestamp;
    }
  }

  auto read() const noexcept -> V { return _value; }

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
