#ifndef RWORSET_H
#define RWORSET_H

#include <numeric>
#include <set>
#include <unordered_map>
#include <utility>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>

namespace crdt {

template <std::equality_comparable V,
          iterable_assiative_type<dot, std::pair<V, bool>> _entries_map_type =
              std::unordered_map<dot, std::pair<V, bool>>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct rwor_set {
  using self_type = rwor_set<V, _entries_map_type, _set_type, _map_type>;

  explicit rwor_set(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto insert(V value) noexcept -> /* delta */ self_type {
    return update_entry(value, true);
  }

  auto erase(V value) noexcept -> /* delta */ self_type {
    return update_entry(value, false);
  }

  void merge(const self_type &delta) { _values.merge(delta._values); }

  auto contains(V value) const noexcept -> bool {
    auto entries = values();
    if (entries.find(value) != entries.end())
      return true;
    return false;
  }

  auto operator==(const self_type &other) const noexcept -> bool {
    return _values.entries == other._values.entries;
  }

  auto values() const noexcept -> std::set<V> {
    std::unordered_map<V, bool> filter_removed;
    for (const auto &[ignore, value] : _values.entries) {
      auto [key, is_enaled] = value;
      if (auto it = filter_removed.find(key); it != filter_removed.end()) {
        it->second &= is_enaled;
      } else {
        filter_removed[key] = is_enaled;
      }
    }
    return std::accumulate(filter_removed.begin(), filter_removed.end(),
                           std::set<V>{}, [](std::set<V> values, auto &iter) {
                             auto [key, is_enaled] = iter;
                             if (is_enaled) {
                               values.insert(key);
                             }
                             return values;
                           });
  }

private:
  std::uint64_t _replicaID;
  dot_kernel<std::pair<V, bool>, _entries_map_type, _set_type, _map_type>
      _values;

  auto update_entry(V value, bool enable) noexcept
      -> /* delta */ rwor_set<V, _entries_map_type, _set_type, _map_type> {
    self_type delta(_replicaID);
    auto observed_add_remove_delta = _values.erase(std::pair{value, true});
    auto observed_rm_remove_delta = _values.erase(std::pair{value, false});
    delta._values.merge(observed_add_remove_delta)
        .merge(observed_rm_remove_delta)
        .merge(_values.insert(_replicaID, {value, enable}));
    return delta;
  }

  rwor_set(
      std::uint64_t replicaID,
      dot_kernel<std::pair<V, bool>, _entries_map_type, _set_type, _map_type>
          delta)
      : _replicaID(replicaID), _values(delta) {}
};

} // namespace crdt.

#endif // !RWORSET_H
