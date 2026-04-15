# lotus-core

A high-performance Vietnamese input engine core written in C++20.

`lotus-core` is a bit-perfect logical port of the [bamboo-core](https://github.com/BambooEngine/bamboo-core) (Go) engine. It is designed to be integrated into native Linux and cross-platform input methods like `fcitx5-lotus`, providing zero runtime overhead and modern memory safety.

## Key Features

- **100% Logic Parity**: Achieve bit-perfect results compared to the original Go implementation.
- **Performance**: Native C++20 implementation for maximum efficiency and minimum latency.
- **Memory Safety**: Robust lifecycle management using `std::shared_ptr` and `std::unique_ptr`.
- **Extended Layouts**: Native support for:
  - Telex (Basic & Telex 2)
  - VNI (Basic & French Keyboard)
  - VIQR
  - Microsoft Layout
- **State Recovery**: Ability to rebuild the engine state from plain text (`RebuildEngineFromText`).
- **Standard Compliant**: Follows both modern (standard) and old-style Vietnamese tone marking.

## Build Instructions

`lotus-core` uses CMake and requires a C++20-compliant compiler (e.g., GCC 10+, Clang 10+).

```bash
mkdir build && cd build
cmake ..
make
```

## Running Tests

The project includes a comprehensive parity test suite verified against the Go reference.

```bash
cd build
./lotus-tests
```

## Integration

See [INTEGRATION.md](./INTEGRATION.md) for detailed instructions on using `lotus-core` in your project.

## License

This project is licensed under the MIT License.
