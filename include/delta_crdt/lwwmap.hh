#ifndef LWWMAP_H
#define LWWMAP_H

#include <optional>
#include <set>
#include <unordered_map>

#include <delta_crdt/awormap.hh>
#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>
#include <delta_crdt/lwwreg.hh>
#include <utility>

namespace crdt {

template <std::totally_ordered K, typename V,
          iterable_assiative_type<K, lwwreg<V>> _kv_map_type =
              std::unordered_map<K, lwwreg<V>>,
          iterable_assiative_type<dot, K> _entries_map_type =
              std::unordered_map<dot, K>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct lwwmap {
  using self_type =
      lwwmap<K, V, _kv_map_type, _entries_map_type, _set_type, _map_type>;

  explicit lwwmap(std::uint64_t replicaID)
      : _replicaID(replicaID), _map(replicaID) {}

  auto read(K key) const -> std::optional<V> {
    const auto value = _map.read(key);
    return value.has_value() ? std::make_optional(value->read()) : std::nullopt;
  }

  auto operator[](K key) -> std::optional<V> { return read(std::move(key)); }

  auto insert(K key, V value) -> /* delta */ self_type {
    const auto current = _map.read(key);
    const auto logical_time =
        current.has_value() ? current->logical_time() : std::uint64_t{0};
    auto delta = _map.insert(
        key, lwwreg<V>::next_write(_replicaID, std::move(value), logical_time));
    return lwwmap(_replicaID, delta);
  }

  auto insert(const std::pair<K, V>& pair) -> /* delta */ self_type {
    return this->insert(pair.first, pair.second);
  }

  auto insert(std::pair<K, V>&& pair) -> /* delta */ self_type {
    auto [key, value] = std::move(pair);
    return this->insert(std::move(key), std::move(value));
  }

  auto erase(K key) -> /* delta */ self_type {
    auto delta = _map.erase(key);
    return lwwmap(_replicaID, delta);
  }

  void merge(const self_type& delta) { _map.merge(delta._map); }

  auto contains(K key) const -> bool { return _map.contains(key); }

  auto operator==(const self_type& other) const -> bool {
    return this->_map == other._map;
  }

 private:
  std::uint64_t _replicaID;
  awor_map<K, lwwreg<V>, _kv_map_type, _entries_map_type, _set_type, _map_type>
      _map;

  lwwmap(std::uint64_t replicaID,
         awor_map<K, lwwreg<V>, _kv_map_type, _entries_map_type, _set_type,
                  _map_type>
             delta)
      : _replicaID(replicaID), _map(delta) {}
};

}  // namespace crdt.

#endif  // !LWWMAP_H
