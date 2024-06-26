#ifndef AWORSET_H
#define AWORSET_H

#include <algorithm>
#include <numeric>
#include <set>
#include <unordered_map>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>

namespace crdt {

template <std::equality_comparable V,
          iterable_assiative_type<dot, V> _entries_map_type =
              std::unordered_map<dot, V>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct awor_set {
  using self_type = awor_set<V, _entries_map_type, _set_type, _map_type>;

  explicit awor_set(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto insert(V value) noexcept -> self_type {
    auto remove_delta = _values.erase(value);
    auto add_delta = _values.insert(_replicaID, value);
    return awor_set(_replicaID, remove_delta.merge(add_delta));
  }

  auto erase(V value) noexcept {
    return awor_set(_replicaID, _values.erase(value));
  }

  void merge(self_type delta) { _values.merge(delta._values); }

  auto contains(V value) const noexcept -> bool {
    return std::find_if(_values.entries.begin(), _values.entries.end(),
                        [&value](const auto &it) {
                          return it.second == value;
                        }) != _values.entries.end();
  }

  auto operator==(const self_type &other) const noexcept -> bool {
    return _values.entries == other._values.entries;
  }

  auto values() const noexcept -> std::set<V> {
    return std::accumulate(_values.entries.begin(), _values.entries.end(),
                           std::set<V>{}, [](std::set<V> values, auto &iter) {
                             values.insert(iter.second);
                             return values;
                           });
  }

private:
  std::uint64_t _replicaID;
  dot_kernel<V, _entries_map_type, _set_type, _map_type> _values;

  awor_set(std::uint64_t replicaID,
           dot_kernel<V, _entries_map_type, _set_type, _map_type> delta)
      : _replicaID(replicaID), _values(delta) {}
};

} // namespace crdt.

#endif // !AWORSET_H
