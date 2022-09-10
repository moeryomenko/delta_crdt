#ifndef CAUSALCOUNTER_H
#define CAUSALCOUNTER_H

#include <algorithm>
#include <compare>
#include <iterator>
#include <numeric>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>

namespace crdt {

template <iterable_assiative_type<dot, std::uint64_t> _entries_map_type =
              std::unordered_map<dot, std::uint64_t>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct causal_counter {
  causal_counter(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto operator++() noexcept
      -> causal_counter<_entries_map_type, _set_type, _map_type> {
    return (*this) += 1;
  }

  auto operator--() noexcept
      -> causal_counter<_entries_map_type, _set_type, _map_type> {
    return (*this) -= 1;
  }

  auto operator+(std::uint64_t value) noexcept
      -> causal_counter<_entries_map_type, _set_type, _map_type> {
    causal_counter<_entries_map_type, _set_type, _map_type> delta_counter(
        _replicaID);
    std::uint64_t base{0UL};
    auto it =
        std::find_if(_value.entries.begin(), _value.entries.end(),
                     [&_replicaID = this->_replicaID, &base](const auto &it) {
                       if (it.first.replicaID == _replicaID) {
                         base = std::max(base, it.second);
                         return true;
                       }
                       return false;
                     });
    if (it != _value.entries.end()) {
      delta_counter._value =
          crdt::merge(delta_counter._value, _value.remove(it->first));
    }
    delta_counter._value =
        crdt::merge(delta_counter._value, _value.add(_replicaID, base + value));
    return delta_counter;
  }

  auto operator-(std::uint64_t value) noexcept
      -> causal_counter<_entries_map_type, _set_type, _map_type> {
    return (*this) + (0 - value);
  }

  auto operator+=(std::uint64_t value) noexcept
      -> causal_counter<_entries_map_type, _set_type, _map_type> {
    auto delta = (*this) + value;
    merge(delta);
    return delta;
  }

  auto operator-=(std::uint64_t value) noexcept
      -> causal_counter<_entries_map_type, _set_type, _map_type> {
    auto delta = (*this) - value;
    merge(delta);
    return delta;
  }

  void merge(const causal_counter<_entries_map_type, _set_type, _map_type>
                 &delta) noexcept {
    _value = crdt::merge(_value, delta._value);
  }

  auto read() const noexcept -> std::uint64_t {
    return std::accumulate(
        _value.entries.begin(), _value.entries.end(), 0UL,
        [](std::uint64_t acc, const auto &it) { return acc + it.second; });
  }

private:
  std::uint64_t _replicaID;
  dot_kernel<std::uint64_t, _entries_map_type, _set_type, _map_type> _value;

  causal_counter(
      std::uint64_t replicaID,
      dot_kernel<std::uint64_t, _entries_map_type, _set_type, _map_type> delta)
      : _replicaID(replicaID), _value(delta){};
};

} // namespace crdt.

#endif // !CAUSALCOUNTER
