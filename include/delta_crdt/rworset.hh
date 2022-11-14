#ifndef RWORSET_H
#define RWORSET_H

#include <compare>
#include <iterator>
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
  explicit rwor_set(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto insert(V value) noexcept
      -> /* delta */ rwor_set<V, _entries_map_type, _set_type, _map_type> {
    return update_entry(value, true);
  }

  auto remove(V value) noexcept
      -> /* delta */ rwor_set<V, _entries_map_type, _set_type, _map_type> {
    return update_entry(value, false);
  }

  void merge(rwor_set<V, _entries_map_type, _set_type, _map_type> delta) {
    _values = crdt::merge(_values, delta._values);
  }

  auto contains(V value) const noexcept -> bool {
    auto entries = values();
    if (entries.find(value) != entries.end())
      return true;
    return false;
  }

  auto operator==(const rwor_set<V, _entries_map_type, _set_type, _map_type>
                      &other) const noexcept -> bool {
    return _values.entries == other._values.entries;
  }

  auto values() const noexcept -> std::set<V> {
    std::unordered_map<V, bool> filter_removed;
    return std::accumulate(
        _values.entries.begin(), _values.entries.end(), std::set<V>{},
        [&filter_removed](std::set<V> values, auto &iter) {
          if (auto it = filter_removed.find(iter.second.first);
              it != filter_removed.end()) {
            it->second &= iter.second.second;
            if (values.find(iter.second.first) != values.end()) {
              if (it->second == false) {
                values.erase(iter.second.first);
              }
            } else if (it->second == true) {
              values.insert(iter.second.first);
            }
          } else {
            filter_removed[iter.second.first] = iter.second.second;
            if (iter.second.second == true) {
              values.insert(iter.second.first);
            } else {
              values.erase(iter.second.first);
            }
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
    rwor_set<V, _entries_map_type, _set_type, _map_type> delta(_replicaID);
    auto observed_add_remove_delta = _values.remove(std::pair{value, true});
    auto observed_rm_remove_delta = _values.remove(std::pair{value, false});
    delta._values =
        crdt::merge(observed_add_remove_delta, observed_rm_remove_delta);
    delta._values =
        crdt::merge(delta._values, _values.add(_replicaID, {value, enable}));
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
