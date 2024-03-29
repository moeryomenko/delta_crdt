#ifndef GROW_COUNTER_H
#define GROW_COUNTER_H

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <unordered_map>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/helpers.hh>

namespace crdt {

template <iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct gcounter {
  using self_type = gcounter<_map_type>;

  explicit gcounter(std::uint64_t replicaID) : _replicaID(replicaID) {
    _values[replicaID] = 0;
  }

  auto operator++() noexcept -> /* delta */ self_type {
    auto value = upsert(this->_values, _replicaID, 1UL,
                        helpers::increment<std::uint64_t>{});
    return gcounter(_replicaID, value);
  }

  void merge(const self_type &other) noexcept {
    std::for_each(other._values.begin(), other._values.end(),
                  [this](const auto &dot) {
                    upsert(this->_values, dot.first, dot.second,
                           helpers::max(dot.second));
                  });
  }

  auto read() const noexcept -> std::uint64_t {
    return std::accumulate(
        _values.begin(), _values.end(), 0,
        [](std::uint64_t sum, const auto &n) { return sum + n.second; });
  }

  auto operator==(const self_type &other) const noexcept -> bool {
    return this->read() == other.read();
  }

private:
  _map_type _values;
  std::uint64_t _replicaID;

  gcounter(std::uint64_t replicaID, std::uint64_t value)
      : _replicaID(replicaID) {
    _values[replicaID] = value;
  }
};

} // namespace crdt

#endif // !GROW_COUNTER_H
