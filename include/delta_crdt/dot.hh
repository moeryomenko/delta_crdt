#ifndef DOT_H
#define DOT_H

#include <algorithm>
#include <compare>
#include <cstdint>
#include <functional>
#include <iterator>
#include <set>
#include <unordered_map>
#include <utility>

#include <delta_crdt/crdt_traits.hh>
#include <delta_crdt/helpers.hh>

namespace crdt {

struct dot {
  std::uint64_t replicaID;
  std::uint64_t counter;

  auto operator<=>(const dot &) const noexcept = default;
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

  auto insert(dot d) noexcept -> self_type & {
    dot_cloud.insert(d);
    return *this;
  }

  auto compact() noexcept -> self_type & {
    _map_type clock;
    std::erase_if(this->dot_cloud, [&clock, this](const auto &d) {
      auto counter =
          helpers::get_or_default(this->clock.vector, d.replicaID, 0UL);
      if (d.counter == counter + 1) {
        clock[d.replicaID] = d.counter;
        return true;
      } else if (d.counter <= counter) {
        return true;
      } else {
        return false;
      }
    });
    this->clock.vector = clock;

    return *this;
  }

  auto merge(dot_context<_set_type, _map_type> a) noexcept -> self_type & {
    for (const auto [r, c] : a.clock.vector) {
      upsert(this->clock.vector, r, c, helpers::max(c));
    }

    this->dot_cloud.insert(a.dot_cloud.begin(), a.dot_cloud.end());
    return this->compact();
  }

  auto contains(dot d) const noexcept -> bool {
    return this->clock.vector.contains(d.replicaID) ||
           this->dot_cloud.contains(d);
  }

  auto next_dot(std::uint64_t replicaID) -> dot {
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

  auto insert(std::uint64_t replicaID, T value) noexcept
      -> /* delta */ self_type {
    self_type delta;
    auto d = this->context.next_dot(replicaID);
    this->entries[d] = value;

    delta.entries[d] = value;
    delta.context.insert(d).compact();
    return delta;
  }

  auto erase(T value) noexcept -> /* delta */ self_type {
    self_type delta;
    std::erase_if(entries, [&value, &delta](const auto &it) {
      if (it.second == value) {
        delta.context.insert(it.first);
        return true;
      }
      return false;
    });
    return delta;
  }

  auto erase(const dot &d) noexcept -> /* delta */ self_type {
    dot_kernel<T, _entries_map_type, _set_type, _map_type> delta;
    if (this->entries.erase(d)) {
      delta.context.insert(d);
    }
    return delta;
  }

  auto clear() noexcept -> /* delta */ self_type {
    self_type delta;
    for (auto [d, _] : entries) {
      delta.context.insert(d);
    }
    entries.erase(entries.begin(), entries.end());
    return delta;
  }

  auto merge(const self_type &other) noexcept -> self_type & {
    if constexpr (cvrdt<T>) {
      for (auto [key, value] : other.entries) {
        if (auto it = this->entries.find(key); it != this->entries.end()) {
          (*it).merge(value);
        } else {
          this->entries.insert(key, value);
        }
      }
    } else {
      std::copy_if(other.entries.begin(), other.entries.end(),
                   std::inserter(this->entries, this->entries.end()),
                   [this](const auto &e) {
                     return !(this->entries.contains(e.first) ||
                              this->context.contains(e.first));
                   });
      std::erase_if(this->entries, [&other](const auto &it) {
        return other.context.contains(it.first) &&
               !other.entries.contains(it.first);
      });
    }
    this->context.merge(other.context);
    return *this;
  }
};

} // namespace crdt.

namespace std {

using crdt::dot;

template <> struct hash<dot> {
  size_t operator()(const dot &d) const {
    std::hash<std::uint64_t> hasher;
    return hasher(d.replicaID) ^ hasher(d.counter);
  }
};

} // namespace std

#endif // DOT_H
