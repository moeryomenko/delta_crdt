#ifndef EWFLAG_H
#define EWFLAG_H

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
struct ewflag {
  using self_type = ewflag<_entries_map_type, _set_type, _map_type>;

  explicit ewflag(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto enable() noexcept -> /* delta */ self_type {
    auto remove_delta = _kernel.erase(true);
    return ewflag(_replicaID,
                  remove_delta.merge(_kernel.insert(_replicaID, true)));
  }

  auto disable() noexcept -> /* delta */ self_type {
    return ewflag(_replicaID, _kernel.erase(true));
  }

  auto is_enaled() const noexcept -> bool { return read(); }

  auto is_disaled() const noexcept -> bool { return !read(); }

  void merge(const self_type &delta) noexcept { _kernel.merge(delta._kernel); }

  auto operator==(const self_type &other) const noexcept -> bool {
    return read() == other.read();
  }

private:
  std::uint64_t _replicaID;
  dot_kernel<bool, _entries_map_type, _set_type, _map_type> _kernel;

  auto read() const noexcept -> bool {
    if (_kernel.entries.begin() == _kernel.entries.end()) {
      return false;
    }
    return true;
  }

  ewflag(std::uint64_t replicaID,
         dot_kernel<bool, _entries_map_type, _set_type, _map_type> kernel)
      : _replicaID(replicaID), _kernel(kernel) {}
};

} // namespace crdt.

#endif // !EWFLAG_H
