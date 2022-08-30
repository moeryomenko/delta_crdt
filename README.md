# delta_crdt 

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
- [conan](https://docs.conan.io/en/latest/installation.html)
- gcc 10+ or clang 10+

```sh
# configure project and download dependencies.
$ cmake -GNinja -H. -Bbuild -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release
# build
$ cmake --build build
# run test
$ ctest --test-dir build
```

## Usage

TODO
