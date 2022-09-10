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
  vector_clock<_map_type> clock;
  _set_type dot_cloud;

  auto add(dot d) noexcept -> dot_context<_set_type, _map_type> {
    dot_cloud.insert(d);
    return *this;
  }
};

template <set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
auto contains(dot d, dot_context<_set_type, _map_type> ctx) noexcept -> bool {
  if (ctx.clock.vector.contains(d.replicaID) || ctx.dot_cloud.contains(d)) {
    return true;
  }
  return false;
}

template <set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
auto next_dot(std::uint64_t replicaID,
              dot_context<_set_type, _map_type> ctx) noexcept
    -> std::pair<dot, dot_context<_set_type, _map_type>> {
  auto value = upsert(replicaID, 1UL, helpers::increment<std::uint64_t>{},
                      ctx.clock.vector);
  return {dot{.replicaID = replicaID, .counter = value}, ctx};
}

template <set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
auto compact(dot_context<_set_type, _map_type> ctx) noexcept
    -> dot_context<_set_type, _map_type> {
  dot_context<_set_type, _map_type> _ctx;
  std::copy_if(ctx.dot_cloud.begin(), ctx.dot_cloud.end(),
               std::inserter(_ctx.dot_cloud, _ctx.dot_cloud.end()),
               [&clock = ctx.clock.vector, &ctx = _ctx](const auto &dot) {
                 auto counter =
                     helpers::get_or_default(clock, dot.replicaID, 0UL);
                 if (dot.counter == counter + 1) {
                   ctx.clock.vector[dot.replicaID] = dot.counter;
                   return false;
                 } else if (dot.counter <= counter) {
                   return false;
                 } else {
                   return true;
                 };
               });
  return _ctx;
}

template <set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
auto merge(dot_context<_set_type, _map_type> a,
           dot_context<_set_type, _map_type> b)
    -> dot_context<_set_type, _map_type> {

  dot_context<_set_type, _map_type> ctx{.clock = b.clock,
                                        .dot_cloud = b.dot_cloud};
  for (const auto [r, c] : a.clock.vector) {
    upsert(r, c, helpers::max(c), ctx.clock.vector);
  }
  ctx.dot_cloud.insert(a.dot_cloud.begin(), a.dot_cloud.end());
  return compact(ctx);
}

template <std::equality_comparable T,
          iterable_assiative_type<dot, T> _entries_map_type =
              std::unordered_map<dot, T>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
struct dot_kernel {
  dot_context<_set_type, _map_type> context;
  _entries_map_type entries;

  auto add(std::uint64_t replicaID, T value) noexcept
      -> /* delta */ dot_kernel<T, _entries_map_type, _set_type, _map_type> {
    dot_kernel<T, _entries_map_type, _set_type, _map_type> delta;
    auto [d, ctx] = next_dot(replicaID, context);
    entries[d] = value;
    context = ctx;

    delta.entries[d] = value;
    delta.context = compact(dot_context<_set_type, _map_type>{}.add(d));
    return delta;
  }

  auto remove(T value) noexcept
      -> /* delta */ dot_kernel<T, _entries_map_type, _set_type, _map_type> {
    dot_kernel<T, _entries_map_type, _set_type, _map_type> delta;
    for (auto it = entries.begin(); it != entries.end();) {
      if (it->second == value) {
        delta.context = dot_context<_set_type, _map_type>{}.add(it->first);
        it = entries.erase(it);
      } else {
        ++it;
      }
    }
    return delta;
  }

  auto remove(const dot &d) noexcept
      -> /* delta */ dot_kernel<T, _entries_map_type, _set_type, _map_type> {
    dot_kernel<T, _entries_map_type, _set_type, _map_type> delta;
    auto it = entries.find(d);
    if (it != entries.end()) {
      delta.context = dot_context<_set_type, _map_type>{}.add(it->first);
      entries.erase(it);
    }
    return delta;
  }

  auto removeAll() noexcept
      -> /* delta */ dot_kernel<T, _entries_map_type, _set_type, _map_type> {
    dot_kernel<T, _entries_map_type, _set_type, _map_type> delta;
    for (auto [d, _] : entries) {
      delta.context.add(d);
    }
    entries.erase(entries.begin(), entries.end());
    return delta;
  }
};

template <std::equality_comparable T,
          iterable_assiative_type<dot, T> _entries_map_type =
              std::unordered_map<dot, T>,
          set_type<dot> _set_type = std::set<dot>,
          iterable_assiative_type<std::uint64_t, std::uint64_t> _map_type =
              std::unordered_map<std::uint64_t, std::uint64_t>>
auto merge(dot_kernel<T, _entries_map_type, _set_type, _map_type> a,
           dot_kernel<T, _entries_map_type, _set_type, _map_type> b)
    -> dot_kernel<T, _entries_map_type, _set_type, _map_type> {
  std::copy_if(b.entries.begin(), b.entries.end(),
               std::inserter(a.entries, a.entries.end()), [&a](const auto &e) {
                 if (!(a.entries.contains(e.first) ||
                       crdt::contains(e.first, a.context)))
                   return true;
                 else
                   return false;
               });
  for (auto it = a.entries.begin(); it != a.entries.end();) {
    if (crdt::contains(it->first, b.context) && !b.entries.contains(it->first))
      it = a.entries.erase(it);
    else
      it++;
  }
  return dot_kernel<T, _entries_map_type, _set_type, _map_type>{
      .context = merge(a.context, b.context), .entries = a.entries};
}

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
