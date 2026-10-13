# delta_crdt [![CI](https://github.com/moeryomenko/delta_crdt/actions/workflows/cmake.yaml/badge.svg)](https://github.com/moeryomenko/delta_crdt/actions/workflows/cmake.yaml)

`delta_crdt` is a header-only C++20 library of educational delta-state CRDTs.

## Status

This project is for studying CRDT implementations and is not production-ready. It has no stable wire format, compatibility promise, persistence layer, or operational security review.

## Requirements

- CMake 3.20 or newer
- A C++20 compiler

## Build and install

```sh
cmake -S . -B build -DCRDT_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
cmake --install build --prefix /desired/prefix
```

A CMake consumer can use the exported target:

```cmake
find_package(delta_crdt CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE delta_crdt::delta_crdt)
```

## Maintainer quality checks

Configure a test-enabled build before running the quality targets because this header-only library uses test translation units for its clang-tidy compilation database:

```sh
cmake -S . -B build -DCRDT_BUILD_TESTS=ON
cmake --build build --target format-check
cmake --build build --target lint
```

`format-check` and `lint` do not rewrite source. Run the following only when you intend to rewrite all repository-owned C++ sources:

```sh
cmake --build build --target format
```

Only `format` and `format-check` require `clang-format`; only `lint` requires `clang-tidy`.

## Usage

Each replica needs a unique, durable replica ID. Mutating operations update local state and return a delta that can be transmitted to another replica. Deltas may be duplicated, delayed, replayed, or delivered in a different order.

```cpp
#include <cassert>

#include <delta_crdt/pn_counter.hh>

crdt::pn_counter first(1);
crdt::pn_counter second(2);

const auto delta = ++first;
second.merge(delta);
second.merge(delta); // replay is safe

assert(first.read() == 1);
assert(second.read() == 1);
```

Merge a full replica state when anti-entropy requires it; merge an operation result when sending a delta. Replicas must retain causal state and tombstones long enough for peers to synchronize. Discarding that state without a coordinated compaction protocol can resurrect removed values or lose updates. Serialization and transport are the caller's responsibility; no versioned serialization format is provided.

## Conflict policies

- `gcounter` is grow-only and returns `uint64_t`.
- `pn_counter` and `causal_counter` return signed `int64_t` values. Values outside their representable range throw `std::overflow_error`.
- `lwwreg` orders writes by a logical timestamp and then the writing replica ID. It is deterministic, but it is a last-write-wins policy rather than causal ordering.
- `awor_set` and `awor_map` use add-wins observed-remove membership.
- `rwor_set` and `rwor_map` use remove-wins membership.
- Concurrent non-CRDT map values resolve to their greatest value under `operator<`. Use a nested CRDT such as `lwwreg`, or `lwwmap`, when last-write-wins value semantics are required.
- `mvreg` retains concurrent values.
- `ewflag` resolves concurrent enable/disable to enabled; `dwflag` resolves them to disabled.

Set-like value types must be totally ordered because their observable values are returned as `std::set`. The default map storage uses `std::unordered_map`, so default key and value types must also satisfy its hash and equality requirements. Custom container template parameters may impose different requirements.

For background on CRDTs, see [crdt.tech](https://crdt.tech/papers.html).

The source code is available under the terms of both the MIT license and the Apache License. See [LICENSE-APACHE](LICENSE-APACHE) and [LICENSE-MIT](LICENSE-MIT).
