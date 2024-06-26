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

template <std::equality_comparable K, typename V,
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

  auto operator[](K key) noexcept -> std::optional<V> {
    auto val = _map[key];
    return val.has_value() ? std::make_optional(val.value().read())
                           : std::nullopt;
  }

  auto insert(K key, V value) -> /* delta */ self_type {
    auto delta = _map.insert(key, lwwreg<V>(_replicaID, value));
    return lwwmap(_replicaID, delta);
  }

  auto insert(const std::pair<K, V> &pair) -> /* delta */ self_type {
    return this->insert(pair.first, pair.second);
  }

  auto insert(std::pair<K, V> &&pair) -> /* delta */ self_type {
    return this->insert(pair.first, pair.second);
  }

  auto erase(K key) noexcept -> /* delta */ self_type {
    auto delta = _map.erase(key);
    return lwwmap(_replicaID, delta);
  }

  void merge(const self_type &delta) noexcept { _map.merge(delta._map); }

  auto contains(K key) const noexcept -> bool { return _map.contains(key); }

  auto operator==(const self_type &other) const noexcept -> bool {
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

} // namespace crdt.

#endif // !LWWMAP_H
