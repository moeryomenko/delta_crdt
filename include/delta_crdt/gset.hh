#ifndef GSET_H
#define GSET_H

#include <concepts>
#include <cstdint>
#include <set>

#include <delta_crdt/crdt_traits.hh>

namespace crdt {

template <std::totally_ordered T, set_type<T> _set_type = std::set<T>>
struct gset {
  gset(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto add(T element) -> gset<T, _set_type> {
    _set.insert(element);
    return gset(_replicaID, {element});
  }

  void merge(gset<T, _set_type> delta) noexcept {
    _set.insert(delta._set.begin(), delta._set.end());
  }

  auto contains(const T &element) const noexcept -> bool {
    return _set.contains(element);
  }

  auto begin() { return _set.begin(); }

  auto end() { return _set.end(); }

  auto operator==(gset<T, _set_type> other) const noexcept -> bool {
    return _set == other._set;
  }

private:
  std::uint64_t _replicaID;
  _set_type _set;

  gset(std::uint64_t replicaID, _set_type set)
      : _replicaID(replicaID), _set(set) {}
};

} // namespace crdt

#endif // !GSET_H
