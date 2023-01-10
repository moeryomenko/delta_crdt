#ifndef RWORMAP_H
#define RWORMAP_H

#include <algorithm>
#include <compare>
#include <iterator>
#include <optional>
#include <set>
#include <unordered_map>
#include <utility>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>
#include <delta_crdt/rworset.hh>

namespace crdt {

template <std::equality_comparable K, typename V,
          iterable_assiative_type<K, V> _kv_map_type = std::unordered_map<K, V>,
          iterable_assiative_type<dot, std::pair<K, bool>> _entries_map_type =
              std::unordered_map<dot, std::pair<K, bool>>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct rwor_map {
  using self_type =
      rwor_map<K, V, _kv_map_type, _entries_map_type, _set_type, _map_type>;

  explicit rwor_map(std::uint64_t replicaID)
      : _replicaID(replicaID), _keys(replicaID) {}

  auto operator[](K key) noexcept -> std::optional<V> {
    auto it = _entries.find(key);
    return _keys.contains(key) && it != _entries.end()
               ? std::make_optional(it->second)
               : std::nullopt;
  }

  auto insert(K key, V value) -> /* delta */ self_type {
    auto keys_delta = _keys.insert(key);
    _entries[key] = value;
    return rwor_map(_replicaID, keys_delta, key, value);
  }

  auto erase(K key) noexcept -> /* delta */ self_type {
    auto keys_delta = _keys.erase(key);
    _entries.erase(key);
    return rwor_map(_replicaID, keys_delta);
  }

  void merge(const self_type &delta) noexcept {
    _keys.merge(delta._keys);
    for (auto [d, value] : delta._entries) {
      if (_keys.contains(d))
        _entries[d] = value;
    }
    std::erase_if(_entries, [this](const auto &it) {
      return !this->_keys.contains(it.first);
    });
  }

  auto contains(K key) const noexcept -> bool {
    return _keys.contains(key) && _entries.contains(key);
  }

private:
  std::uint64_t _replicaID;
  rwor_set<K, _entries_map_type, _set_type, _map_type> _keys;
  _kv_map_type _entries;

  rwor_map(std::uint64_t replicaID,
           rwor_set<K, _entries_map_type, _set_type, _map_type> delta)
      : _replicaID(replicaID), _keys(delta) {}

  rwor_map(std::uint64_t replicaID,
           rwor_set<K, _entries_map_type, _set_type, _map_type> delta, K key,
           V value)
      : _replicaID(replicaID), _keys(delta) {
    _entries[key] = value;
  }
};

} // namespace crdt.

#endif // !AWORMAP_H
