#ifndef AWORMAP_H
#define AWORMAP_H

#include "aworset.hh"
#include <algorithm>
#include <compare>

#include <crdt_traits.hh>
#include <dot.hh>

namespace crdt {

template <std::equality_comparable K, typename V,
          iterable_assiative_type<K, V> _kv_map_type = std::unordered_map<K, V>,
          iterable_assiative_type<dot, K> _entries_map_type =
              std::unordered_map<dot, K>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct awor_map {
  awor_map(std::uint64_t replicaID) : _replicaID(replicaID), _keys(replicaID){};

  auto insert(K key, V value)
      -> /* delta */ awor_map<K, V, _kv_map_type, _entries_map_type, _set_type,
                              _map_type> {
    auto keys_delta = _keys.insert(key);
    _entries[key] = value;
    return awor_map(_replicaID, keys_delta, key, value);
  }

  auto erase(K key) noexcept
      -> /* delta */ awor_map<K, V, _kv_map_type, _entries_map_type, _set_type,
                              _map_type> {
    auto keys_delta = _keys.remove(key);
    _entries.erase(key);
    return awor_map(_replicaID, keys_delta);
  }

  void
  merge(awor_map<K, V, _kv_map_type, _entries_map_type, _set_type, _map_type>
            delta) noexcept {
    _keys.merge(delta._keys);
    _entries.insert(delta._entries.begin(), delta._entries.end());
    for (auto it = _entries.begin(); it != _entries.end();) {
      if (_keys.contains(it->first)) {
        ++it;
      } else {
        it = _entries.erase(it);
      }
    }
  }

  auto contains(K key) const noexcept -> bool {
    return _keys.contains(key) && _entries.contains(key);
  }

private:
  std::uint64_t _replicaID;
  awor_set<K, _entries_map_type, _set_type, _map_type> _keys;
  _kv_map_type _entries;

  awor_map(std::uint64_t replicaID,
           awor_set<K, _entries_map_type, _set_type, _map_type> delta)
      : _replicaID(replicaID), _keys(delta){};

  awor_map(std::uint64_t replicaID,
           awor_set<K, _entries_map_type, _set_type, _map_type> delta, K key,
           V value)
      : _replicaID(replicaID), _keys(delta) {
    _entries[key] = value;
  };
};

} // namespace crdt.

#endif // !AWORMAP_H
