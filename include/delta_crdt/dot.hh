#ifndef DOT_H
#define DOT_H

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <set>
#include <unordered_map>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/helpers.hh>

namespace crdt {

struct dot {
  std::uint64_t replicaID;
  std::uint64_t counter;

  auto operator<=>(const dot&) const = default;
};

template <iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct vector_clock {
  _map_type vector;
};

template <set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct dot_context {
  using self_type = dot_context<_set_type, _map_type>;

  vector_clock<_map_type> clock;
  _set_type dot_cloud;

  auto insert(dot d) -> self_type& {
    dot_cloud.insert(d);
    return *this;
  }

  auto compact() -> self_type& {
    bool advanced = true;
    while (advanced) {
      advanced = false;
      std::erase_if(this->dot_cloud, [this, &advanced](const auto& d) {
        const auto it = this->clock.vector.find(d.replicaID);
        const auto counter = it == this->clock.vector.end() ? 0UL : it->second;
        if (d.counter <= counter) {
          return true;
        }
        if (d.counter == counter + 1) {
          this->clock.vector[d.replicaID] = d.counter;
          advanced = true;
          return true;
        }
        return false;
      });
    }
    return *this;
  }

  auto merge(dot_context<_set_type, _map_type> a) -> self_type& {
    for (const auto [r, c] : a.clock.vector) {
      upsert(this->clock.vector, r, c, helpers::max(c));
    }

    this->dot_cloud.insert(a.dot_cloud.begin(), a.dot_cloud.end());
    return this->compact();
  }

  auto contains(dot d) const -> bool {
    if (const auto it = this->clock.vector.find(d.replicaID);
        it != this->clock.vector.end() && d.counter <= it->second) {
      return true;
    }
    return this->dot_cloud.contains(d);
  }

  auto next_dot(std::uint64_t replicaID) -> dot {
    this->compact();
    return dot{
        .replicaID = replicaID,
        .counter = upsert(this->clock.vector, replicaID, 1UL,
                          helpers::increment<std::uint64_t>{}),
    };
  }
};

template <std::equality_comparable T,
          iterable_assiative_type<dot, T> _entries_map_type =
              std::unordered_map<dot, T>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct dot_kernel {
  using self_type = dot_kernel<T, _entries_map_type, _set_type, _map_type>;

  dot_context<_set_type, _map_type> context;
  _entries_map_type entries;

  auto insert(std::uint64_t replicaID, T value) -> /* delta */ self_type {
    self_type delta;
    auto d = this->context.next_dot(replicaID);
    this->entries[d] = value;

    delta.entries[d] = value;
    delta.context.insert(d).compact();
    return delta;
  }

  auto erase(T value) -> /* delta */ self_type {
    self_type delta;
    std::erase_if(entries, [&value, &delta](const auto& it) {
      if (it.second == value) {
        delta.context.insert(it.first);
        return true;
      }
      return false;
    });
    return delta;
  }

  auto erase(const dot& d) -> /* delta */ self_type {
    dot_kernel<T, _entries_map_type, _set_type, _map_type> delta;
    if (this->entries.erase(d)) {
      delta.context.insert(d);
    }
    return delta;
  }

  auto clear() -> /* delta */ self_type {
    self_type delta;
    for (auto [d, _] : entries) {
      delta.context.insert(d);
    }
    entries.erase(entries.begin(), entries.end());
    return delta;
  }

  auto merge(const self_type& other) -> self_type& {
    for (const auto& [key, value] : other.entries) {
      if (auto it = this->entries.find(key); it != this->entries.end()) {
        if constexpr (cvrdt<T>) {
          it->second.merge(value);
        }
      } else if (!this->context.contains(key)) {
        this->entries.insert({key, value});
      }
    }

    std::erase_if(this->entries, [&other](const auto& entry) {
      return other.context.contains(entry.first) &&
             !other.entries.contains(entry.first);
    });
    this->context.merge(other.context);
    return *this;
  }
};

}  // namespace crdt.

namespace std {

using crdt::dot;

template <>
struct hash<dot> {
  size_t operator()(const dot& d) const {
    std::hash<std::uint64_t> hasher;
    return hasher(d.replicaID) ^ hasher(d.counter);
  }
};

}  // namespace std

#endif  // DOT_H
