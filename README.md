# delta_crdt [![CI](https://github.com/moeryomenko/delta_crdt/actions/workflows/cmake.yaml/badge.svg)](https://github.com/moeryomenko/delta_crdt/actions/workflows/cmake.yaml)

This headers only library contains delta-CRDT types.

## What?

CRDT's are the solution to highly available mutable state.

## Disclaimer

This project is not ready for production, and is educational in nature to study the topic of CRDT types.

For more info see [crdt.tech](https://crdt.tech/papers.html)

The source code is available at under the terms of both the MIT license and the Apache License.
See [LICENSE-APACHE](LICENSE-APACHE) and/or [LICENSE-MIT](LICENSE-MIT) for details.

## Build

For build you need:

- [CMake](https://cmake.org/download/)
- [GoogleTest](https://github.com/google/googletest)
- gcc 10+ or clang 10+

```sh
cmake -Bbuild . -DCRDT_BUILD_TESTS=True
```

## Usage

TODO
