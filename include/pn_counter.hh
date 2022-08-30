#ifndef PN_COUNTER_H
#define PN_COUNTER_H

#include <gcounter.hh>

namespace crdt {

template <iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct pn_counter {
  pn_counter(std::uint64_t replicaID)
      : _replicaID(replicaID), _inc(replicaID), _dec(replicaID) {}

  auto operator++() noexcept -> /* delta */ pn_counter<_map_type> {
    auto inc_delta = ++_inc;
    return pn_counter<_map_type>(_replicaID, inc_delta, gcounter(_replicaID));
  }

  auto operator--() noexcept -> /* delta */ pn_counter<_map_type> {
    auto dec_delta = ++_dec;
    return pn_counter<_map_type>(_replicaID, gcounter(_replicaID), dec_delta);
  }

  void merge(pn_counter<_map_type> other) noexcept {
    _inc.merge(other._inc);
    _dec.merge(other._dec);
  }

  auto read() const noexcept -> std::uint64_t {
    return _inc.read() - _dec.read();
  }

private:
  std::uint64_t _replicaID;
  gcounter<_map_type> _inc;
  gcounter<_map_type> _dec;

  pn_counter(std::uint64_t replicaID, gcounter<_map_type> inc,
             gcounter<_map_type> dec)
      : _replicaID(replicaID), _inc(inc), _dec(dec) {}
};

} // namespace crdt

#endif // !PN_COUNTER_H
