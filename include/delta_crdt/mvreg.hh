#ifndef MVREG_H
#define MVREG_H

#include <algorithm>
#include <compare>
#include <iterator>
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
struct mvreg {
  using self_type = mvreg<V, _entries_map_type, _set_type, _map_type>;

  mvreg(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto set(V value) noexcept -> /* delta */ self_type {
    auto delta = _values.clear();
    delta.merge(_values.insert(_replicaID, value));
    return mvreg(_replicaID, delta);
  }

  void merge(const self_type &delta) noexcept { _values.merge(delta._values); }

  auto read() const noexcept -> std::set<V> {
    std::set<V> result;
    std::transform(_values.entries.cbegin(), _values.entries.cend(),
                   std::inserter(result, result.end()),
                   [](const auto &it) { return it.second; });
    return result;
  }

  auto operator==(const self_type &other) const noexcept -> bool {
    return this->read() == other.read();
  }

private:
  std::uint64_t _replicaID;
  dot_kernel<V, _entries_map_type, _set_type, _map_type> _values;

  mvreg(std::uint64_t replicaID,
        dot_kernel<V, _entries_map_type, _set_type, _map_type> delta)
      : _replicaID(replicaID), _values(delta) {}
};

} // namespace crdt.

#endif // !MVREG_H
