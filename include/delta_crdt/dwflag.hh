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
  explicit dwflag(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto enable() noexcept
      -> /* delta */ dwflag<_entries_map_type, _set_type, _map_type> {
    return dwflag(_replicaID, _kernel.erase(false));
  }

  auto disable() noexcept
      -> /* delta */ dwflag<_entries_map_type, _set_type, _map_type> {
    auto remove_delta = _kernel.erase(false);
    return dwflag(_replicaID,
                  crdt::merge(remove_delta, _kernel.insert(_replicaID, false)));
  }

  auto is_enaled() const noexcept -> bool { return read() == true; }

  auto is_disaled() const noexcept -> bool { return read() == false; }

  void merge(dwflag<_entries_map_type, _set_type, _map_type> delta) noexcept {
    _kernel = crdt::merge(_kernel, delta._kernel);
  }

  auto operator==(const dwflag<_entries_map_type, _set_type, _map_type> &other)
      const noexcept -> bool {
    return read() == other.read();
  }

private:
  std::uint64_t _replicaID;
  dot_kernel<bool, _entries_map_type, _set_type, _map_type> _kernel;

  auto read() const noexcept -> bool {
    if (_kernel.entries.begin() == _kernel.entries.end()) {
      return true;
    }
    return false;
  }

  dwflag(std::uint64_t replicaID,
         dot_kernel<bool, _entries_map_type, _set_type, _map_type> kernel)
      : _replicaID(replicaID), _kernel(kernel) {}
};

} // namespace crdt.

#endif // !DWFLAG_H
