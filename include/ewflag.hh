#ifndef EWFLAG_H
#define EWFLAG_H

#include <crdt_traits.hh>
#include <dot.hh>

namespace crdt {

template <iterable_assiative_type<dot, bool> _entries_map_type =
              std::unordered_map<dot, bool>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct ewflag {
  ewflag(std::uint64_t replicaID) : _replicaID(replicaID) {}

  auto enable() noexcept
      -> /* delta */ ewflag<_entries_map_type, _set_type, _map_type> {
    auto remove_delta = _kernel.remove(true);
    return ewflag(_replicaID,
                  crdt::merge(remove_delta, _kernel.add(_replicaID, true)));
  }

  auto disable() noexcept
      -> /* delta */ ewflag<_entries_map_type, _set_type, _map_type> {
    return ewflag(_replicaID, _kernel.remove(true));
  }

  auto is_enaled() const noexcept -> bool { return read() == true; }

  auto is_disaled() const noexcept -> bool { return read() == false; }

  void merge(ewflag<_entries_map_type, _set_type, _map_type> delta) noexcept {
    _kernel = crdt::merge(_kernel, delta._kernel);
  }

  auto operator==(const ewflag<_entries_map_type, _set_type, _map_type> &other)
      const noexcept -> bool {
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
