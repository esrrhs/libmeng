# libmeng

[English](README.md) | [中文](README_ZH.md)

**libmeng** is a lightweight, ultra-fast, cross-platform coroutine / fiber library written in C and modern C++. Designed for high-performance concurrent systems, it provides ultra-low latency context switching (~25 ns) and supports tens of millions of concurrent coroutines.

---

## 🌟 Key Features

- **Blazing Fast Context Switching**: Handcrafted lightweight assembly switching achieves latency down to **~25 ns** per context switch and a throughput of **~40 million switches/sec**.
- **Cross-Platform & Multi-Architecture**:
  - **Linux**: x86_64, aarch64 / ARM64
  - **Windows**: x86 (Win32), x64 (Win64)
  - **macOS**: x86_64, Apple Silicon (ARM64)
- **Dual API Design**:
  - **Pure C API** (`meng.h`): Minimal, zero dependencies, ABI stable, and 100% backward compatible.
  - **Modern C++ API** (`meng.hpp`): C++11/14/17/20 ready, featuring RAII resource management, lambda closures with variable captures, exception propagation across coroutines, and move semantics.
- **Nested Coroutine Support**: Coroutines can create and schedule sub-coroutines seamlessly.
- **Thread-Safe**: Uses `thread_local` state isolation, allowing concurrent coroutine execution across multiple worker threads.
- **Strict ABI Compliance**: Enforces 16-byte stack alignment, ensuring safe execution of vectorized SSE/AVX instructions and modern runtime libraries.
- **Stack Overflow Canary**: Includes built-in magic canary detection for stack boundary violation prevention.
- **Modern CMake Integration**: First-class support for CMake 3.15+, `FetchContent`, `find_package(meng)`, and CTest automated testing.

---

## ⚡ Performance Benchmark

Measured on a Linux x86_64 machine with 10,000,000 ping-pong context switches:

| Metric | Measurement |
| :--- | :--- |
| **Switch Latency** | **~25.08 ns** / switch |
| **Throughput** | **~39.87 M** switches / second |
| **Memory Footprint** | Extremely compact control block, configurable stack size (default 64 KB) |

---

## 🚀 Quick Start

### 1. Modern C++ Interface (`meng.hpp`)

Use modern C++ lambdas, captures, and RAII:

```cpp
#include "meng.hpp"
#include <iostream>

int main() {
    int counter = 0;

    // Create a coroutine with lambda and variable capture
    libmeng::coroutine co([&counter]() {
        for (int i = 0; i < 3; ++i) {
            counter++;
            std::cout << "Coroutine step " << i << ", counter = " << counter << std::endl;
            libmeng::yield(); // Yield execution back to the caller
        }
    });

    // Schedule and resume until completion
    while (co.resume()) {
        std::cout << "Back in main thread" << std::endl;
    }

    std::cout << "Done! Final counter: " << counter << std::endl;
    return 0; // RAII automatically frees coroutine resources
}
```

### 2. Pure C Interface (`meng.h`)

Compatible with standard C codebases:

```c
#include "meng.h"
#include <stdio.h>

void worker(meng * m, void * arg, size_t argsize) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        printf("Coroutine %d step %d\n", id, i);
        meng_yield(m); // Or call meng_yield_current()
    }
}

int main() {
    int id1 = 1, id2 = 2;
    meng * m1 = meng_create(worker, 16 * 1024, &id1, sizeof(id1));
    meng * m2 = meng_create(worker, 16 * 1024, &id2, sizeof(id2));

    while (!meng_end(m1) || !meng_end(m2)) {
        if (!meng_end(m1)) meng_run(m1);
        if (!meng_end(m2)) meng_run(m2);
    }

    meng_delete(m1);
    meng_delete(m2);
    return 0;
}
```

---

## 🛠️ Build and Test

### Prerequisites
- CMake 3.15 or newer
- GCC 4.8+ / Clang 3.8+ / MSVC 2015+ (C++11 support)

### One-line Build & Test
```bash
./build.sh           # Builds in Debug mode and runs all unit tests
./build.sh release   # Builds in Release mode
```

### Standard CMake Workflow
```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release -DMENG_BUILD_TESTS=ON

# Build
cmake --build build -j

# Run automated tests
ctest --test-dir build --output-on-failure

# Run benchmark
./build/bin/benchmark
```

---

## 📦 Integration

### Option 1: CMake `FetchContent` (Recommended)
```cmake
include(FetchContent)
FetchContent_Declare(
    meng
    GIT_REPOSITORY https://github.com/esrrhs/libmeng.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(meng)

target_link_libraries(your_target PRIVATE meng::meng)
```

### Option 2: System Installation (`find_package`)
```bash
cmake --build build --target install
```
In your `CMakeLists.txt`:
```cmake
find_package(meng REQUIRED)
target_link_libraries(your_target PRIVATE meng::meng)
```

---

## 📖 API Reference

### C API (`meng.h`)
- `meng * meng_create(meng_main func, size_t stacksize, const void * arg, size_t argsize)`: Create a new coroutine with specified entry function, stack size, and user data.
- `void meng_run(meng * m)`: Run or resume the coroutine until it yields or terminates.
- `void meng_yield(meng * m)`: Yield CPU execution back to the caller.
- `void meng_yield_current(void)`: Yield execution from the currently running coroutine without needing the handle.
- `bool meng_end(meng * m)`: Check if the coroutine has finished execution.
- `void meng_delete(meng * m)`: Destroy the coroutine and release all allocated memory.
- `meng * meng_running(void)`: Returns the currently running coroutine in this thread (or NULL if in main thread).
- `void * meng_get_arg(meng * m)`: Retrieve the pointer to user arguments.
- `size_t meng_get_arg_size(meng * m)`: Retrieve user argument size in bytes.
- `size_t meng_get_stack_size(meng * m)`: Retrieve coroutine stack size in bytes.

### C++ API (`meng.hpp`)
- `libmeng::coroutine`:
  - `coroutine(Callable&& fn, size_t stack_size = 64 * 1024)`: Constructs a coroutine from any callable (lambda, functor, function).
  - `bool resume()`: Resumes execution. Propagates uncaught exceptions back to the caller. Returns `true` if suspended and still runnable; `false` if finished.
  - `void yield()`: Yields execution back to the caller.
  - `bool done() const`: Returns `true` when finished.
  - Destructor automatically reclaims all coroutine resources (RAII).
- `libmeng::yield()`: Yield the currently running coroutine from any call stack depth.
- `libmeng::current()`: Retrieve the current `libmeng::coroutine*` pointer.

---

## 📄 License
MIT License
