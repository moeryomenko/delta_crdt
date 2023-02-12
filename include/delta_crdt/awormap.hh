#ifndef AWORMAP_H
#define AWORMAP_H

#include <algorithm>
#include <compare>
#include <iterator>
#include <numeric>
#include <optional>
#include <set>
#include <unordered_map>

#include <delta_crdt/aworset.hh>
#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>
#include <utility>

namespace crdt {

template <std::equality_comparable K, typename V,
          iterable_assiative_type<K, V> _kv_map_type = std::unordered_map<K, V>,
          iterable_assiative_type<dot, K> _entries_map_type =
              std::unordered_map<dot, K>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct awor_map {
  using self_type =
      awor_map<K, V, _kv_map_type, _entries_map_type, _set_type, _map_type>;

  explicit awor_map(std::uint64_t replicaID)
      : _replicaID(replicaID), _keys(replicaID) {}

  auto operator[](K key) noexcept -> std::optional<V> {
    auto it = _entries.find(key);
    return _keys.contains(key) && it != _entries.end()
               ? std::make_optional(it->second)
               : std::nullopt;
  }

  auto insert(K key, V value) -> /* delta */ self_type {
    auto keys_delta = _keys.insert(key);
    this->insert_entry(key, value);
    return awor_map(_replicaID, keys_delta, key, value);
  }

  auto insert(const std::pair<K, V> &pair) -> /* delta */ self_type {
    return this->insert(pair.first, pair.second);
  }

  auto insert(std::pair<K, V> &&pair) -> /* delta */ self_type {
    return this->insert(pair.first, pair.second);
  }

  auto erase(K key) noexcept -> /* delta */ self_type {
    auto keys_delta = _keys.erase(key);
    _entries.erase(key);
    return awor_map(_replicaID, keys_delta);
  }

  void merge(self_type delta) noexcept {
    _keys.merge(delta._keys);
    for (auto [d, value] : delta._entries) {
      if (_keys.contains(d)) {
        this->insert_entry(d, value);
      }
    }
    std::erase_if(_entries, [this](const auto &it) {
      return !this->_keys.contains(it.first);
    });
  }

  auto contains(K key) const noexcept -> bool {
    return _keys.contains(key) && _entries.contains(key);
  }

  auto operator==(const self_type &other) const noexcept -> bool {
    return this->_keys == other._keys && this->_entries == other._entries;
  }

private:
  std::uint64_t _replicaID;
  awor_set<K, _entries_map_type, _set_type, _map_type> _keys;
  _kv_map_type _entries;

  awor_map(std::uint64_t replicaID,
           awor_set<K, _entries_map_type, _set_type, _map_type> delta)
      : _replicaID(replicaID), _keys(delta) {}

  awor_map(std::uint64_t replicaID,
           awor_set<K, _entries_map_type, _set_type, _map_type> delta, K key,
           V value)
      : _replicaID(replicaID), _keys(delta) {
    _entries[key] = value;
  }

  void insert_entry(K key, V value) {
    if constexpr (crdt::cvrdt<V>) {
      this->upsert_crdt_entry(key, value);
    } else {
      _entries[key] = value;
    }
  }

  void upsert_crdt_entry(K key, V value)
    requires crdt::cvrdt<V>
  {
    if (auto it = _entries.find(key); it != _entries.end()) {
      it->second.merge(value);
    } else {
      _entries[key] = value;
    }
  }
};

} // namespace crdt.

#endif // !AWORMAP_H
