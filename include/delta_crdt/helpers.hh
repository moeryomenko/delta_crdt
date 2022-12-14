#ifndef HELPERS_H
#define HELPERS_H

#include <concepts>

#include <delta_crdt/crdt_traits.hh>

namespace crdt {

namespace helpers {

template <crdt::incrementable_type T> struct increment {
  constexpr auto operator()(const T &arg) const -> T { return arg + 1; }
};

template <std::totally_ordered T> struct max {
  max(T value) : _value(value) {}

  constexpr auto operator()(const T &arg) const -> T {
    return (_value < arg) ? arg : _value;
  }

private:
  T _value;
};

template <typename _key_type, typename _value_type,
          crdt::iterable_assiative_type<_key_type, _value_type> _map_type>
auto get_or_default(_map_type _map, _key_type _key, _value_type _value)
    -> _value_type {
  if (auto it = _map.find(_key); it != _map.end()) {
    return it->second;
  } else {
    return _value;
  }
}

} // namespace helpers

template <typename K, typename V, typename T,
          iterable_assiative_type<K, V> _map_type>
auto upsert(_map_type &map, K key, V val, T fn) noexcept -> V {
  if (auto it = map.find(key); it != map.end()) {
    auto val = fn(it->second);
    it->second = val;
    return val;
  } else {
    map[key] = val;
    return val;
  }
}

} // namespace crdt

#endif // !HELPERS_H
