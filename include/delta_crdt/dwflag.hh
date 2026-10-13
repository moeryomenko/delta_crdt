#ifndef DWFLAG_H
#define DWFLAG_H

#include <set>
#include <unordered_map>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/dot.hh>

namespace crdt {

template <iterable_assiative_type<dot, bool> _entries_map_type =
              std::unordered_map<dot, bool>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct dwflag {
  using self_type = dwflag<_entries_map_type, _set_type, _map_type>;

  explicit dwflag(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto enable() -> /* delta */ self_type {
    return dwflag(_replicaID, _kernel.erase(false));
  }

  auto disable() -> /* delta */ self_type {
    auto remove_delta = _kernel.erase(false);
    return dwflag(_replicaID,
                  remove_delta.merge(_kernel.insert(_replicaID, false)));
  }

  auto read() const -> bool { return _kernel.entries.empty(); }

  auto is_enabled() const -> bool { return read(); }

  auto is_disabled() const -> bool { return !read(); }

  void merge(const self_type& delta) { _kernel.merge(delta._kernel); }

  auto operator==(const self_type& other) const -> bool {
    return _kernel.entries == other._kernel.entries &&
           _kernel.context.clock.vector == other._kernel.context.clock.vector &&
           _kernel.context.dot_cloud == other._kernel.context.dot_cloud;
  }

 private:
  std::uint64_t _replicaID;
  dot_kernel<bool, _entries_map_type, _set_type, _map_type> _kernel;

  dwflag(std::uint64_t replicaID,
         dot_kernel<bool, _entries_map_type, _set_type, _map_type> kernel)
      : _replicaID(replicaID), _kernel(kernel) {}
};

}  // namespace crdt.

#endif  // !DWFLAG_H
