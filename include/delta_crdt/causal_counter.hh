#ifndef CAUSALCOUNTER_H
#define CAUSALCOUNTER_H

#include <numeric>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>

namespace crdt {

template <iterable_assiative_type<dot, std::int64_t> _entries_map_type =
              std::unordered_map<dot, std::int64_t>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct causal_counter {
  using self_type = causal_counter<_entries_map_type, _set_type, _map_type>;

  explicit causal_counter(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto operator++() -> self_type { return (*this) += 1; }

  auto operator--() -> self_type { return (*this) -= 1; }

  auto operator+(std::int64_t value) const -> self_type {
    self_type delta(_replicaID);
    std::int64_t base = 0;
    for (const auto& [d, entry] : _value.entries) {
      if (d.replicaID == _replicaID) {
        base = checked_add(base, entry);
        delta._value.context.insert(d);
      }
    }
    delta._value.context.compact();
    delta._value.insert(_replicaID, checked_add(base, value));
    return delta;
  }

  auto operator-(std::int64_t value) const -> self_type {
    if (value == std::numeric_limits<std::int64_t>::min()) {
      throw std::overflow_error("causal counter subtraction overflow");
    }
    return (*this) + -value;
  }

  auto operator+=(std::int64_t value) -> self_type {
    auto delta = (*this) + value;
    merge(delta);
    return delta;
  }

  auto operator-=(std::int64_t value) -> self_type {
    auto delta = (*this) - value;
    merge(delta);
    return delta;
  }

  void merge(const self_type& delta) { _value.merge(delta._value); }

  auto read() const -> std::int64_t {
    return std::accumulate(_value.entries.begin(), _value.entries.end(),
                           std::int64_t{0},
                           [](std::int64_t total, const auto& entry) {
                             return checked_add(total, entry.second);
                           });
  }

  auto operator==(const self_type& other) const -> bool {
    return _value.entries == other._value.entries &&
           _value.context.clock.vector == other._value.context.clock.vector &&
           _value.context.dot_cloud == other._value.context.dot_cloud;
  }

 private:
  std::uint64_t _replicaID;
  dot_kernel<std::int64_t, _entries_map_type, _set_type, _map_type> _value;

  static auto checked_add(std::int64_t left,
                          std::int64_t right) -> std::int64_t {
    if ((right > 0 &&
         left > std::numeric_limits<std::int64_t>::max() - right) ||
        (right < 0 &&
         left < std::numeric_limits<std::int64_t>::min() - right)) {
      throw std::overflow_error("causal counter overflow");
    }
    return left + right;
  }
};

}  // namespace crdt.

#endif  // !CAUSALCOUNTER
