# Integration Guide: lotus-core for fcitx5-lotus

This document provides instructions for integrating `lotus-core` into the `fcitx5-lotus` input method, replacing the legacy `bamboo-core` (Go) engine.

## 1. Why `lotus-core`?

- **Zero Overhead**: C++20 implementation eliminates the need for CGO or Go runtime in `fcitx5-lotus`.
- **Memory Safety**: Uses modern smart pointers (`std::shared_ptr`, `std::unique_ptr`) for transformation lifecycle management.
- **100% Logic Parity**: Bit-perfect replication of Bamboo's transformation, tone targeting, and state restoration logic.
- **Extended Layout Support**: Native support for Telex 2, Microsoft layout, and VNI French.

## 2. CMake Integration

Add `lotus-core` as a subdirectory or link it as a library:

```cmake
add_subdirectory(lotus-core)
target_link_libraries(fcitx5-lotus-core PRIVATE lotus-core)
```

## 3. Basic API Usage

### Initialization

```cpp
#include <lotus/engine.hpp>
#include <lotus/rules_data.hpp>

// Create a Telex engine with standard Vietnamese flags
auto engine = lotus::CreateEngine(lotus::GetInputMethod(u"Telex"), lotus::EstdFlags);
```

### Key Processing

```cpp
// Process a key event (simulating 'a' then 'a' in Telex)
engine->ProcessKey('a', lotus::Mode::VietnameseMode);
engine->ProcessKey('a', lotus::Mode::VietnameseMode);

// Get the resulting string
std::u16string result = engine->GetProcessedString(lotus::Mode::VietnameseMode); // Returns u"â"
```

### State Management (Fcitx5 Overlays)

Use `GetProcessedString(Mode::FullText)` to display the raw composition string in the candidate window or preedit.

```cpp
std::u16string raw = engine->GetProcessedString(lotus::Mode::FullText);
```

### State Restoration

If you need to rebuild the engine from existing text in the input field:

```cpp
engine->RebuildEngineFromText(u"nguyễn");
```

## 4. Feature Comparison

| Feature | lotus-core (C++) | bamboo-core (Go) |
| :--- | :--- | :--- |
| **Telex/VNI/VIQR** | Yes | Yes |
| **Telex 2/Microsoft** | Yes (Verified) | Yes |
| **Double-Typing Undo** | Yes | Yes |
| **Standard Tone Style** | Yes | Yes |
| **Performance** | O(1) character lookup | Runtime overhead |
