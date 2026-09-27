# libmeng

[![CI](https://github.com/esrrhs/libmeng/actions/workflows/ci.yml/badge.svg)](https://github.com/esrrhs/libmeng/actions/workflows/ci.yml)

[English](README.md) | [中文](README_ZH.md)

**libmeng** 是一个高性能、跨平台的轻量级协程库（Fiber / Coroutine），支持单机千万级高并发协程调度与低延迟上下文切换。

---

## 🌟 特性

- **极速上下文切换**：基于手写轻量汇编，单次上下文切换延迟仅 **~25 ns**，吞吐量可达 **~4000 万次/秒**。
- **跨平台与多架构支持**：
  - Linux (x86_64, aarch64 / ARM64, GCC / Clang)
  - Windows (x86_64, x86, MinGW-w64)
  - macOS (x86_64, Apple Silicon / ARM64)
- **双重 API 接口**：
  - **原生 C API** (`meng.h`)：极简、零依赖、100% 向后兼容。
  - **现代 C++ 接口** (`meng.hpp`)：支持 C++11/14/17/20，具备 RAII 自动管理、Lambda 闭包、捕获变量、异常自动捕获与跨协程重新抛出、移动语义。
- **协程嵌套调用**：支持协程内部创建并调度新协程。
- **线程安全**：基于 `thread_local` 隔离，支持在多线程中并行运行各线程的独立协程。
- **严格遵循 ABI**：栈帧保证 16 字节对齐，安全支持 SSE / AVX 向量化指令。
- **栈溢出检测**：提供 Magic Canary 金丝雀检测与栈边界校验。
- **现代 CMake 构建**：原生支持 `FetchContent`、`find_package(meng)` 与 CTest 自动化测试。

---

## ⚡ 性能基准

在 Linux x86_64 环境下的实测性能（10,000,000 次 Ping-Pong 往返切换）：

| 指标 | 测试数值 |
| :--- | :--- |
| **切换延迟 (Latency)** | **~25.08 ns** / 次 |
| **吞吐量 (Throughput)** | **~39.87 M** switches / s |
| **内存占用** | 结构体精简，自定义栈大小（默认 64KB） |

---

## 🚀 快速上手

### 1. 现代 C++ 示例 (`meng.hpp`)

使用现代 C++ Lambda 与 RAII：

```cpp
#include "meng.hpp"
#include <iostream>

int main() {
    int counter = 0;

    // 创建协程，支持捕获变量与任意 Lambda
    libmeng::coroutine co([&counter]() {
        for (int i = 0; i < 3; ++i) {
            counter++;
            std::cout << "Coroutine step " << i << ", counter = " << counter << std::endl;
            libmeng::yield(); // 让出执行权
        }
    });

    // 调度执行
    while (co.resume()) {
        std::cout << "Back in main thread" << std::endl;
    }

    std::cout << "Done! Final counter: " << counter << std::endl;
    return 0; // RAII 自动销毁协程
}
```

### 2. 原生 C API 示例 (`meng.h`)

兼容经典 C 风格接口：

```c
#include "meng.h"
#include <stdio.h>

void func(meng * m, void * arg, size_t argsize) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        printf("coroutine %d step %d\n", id, i);
        meng_yield(m); // 或者调用 meng_yield_current()
    }
}

int main() {
    int arg1 = 1, arg2 = 2;
    meng * m1 = meng_create(func, 16 * 1024, &arg1, sizeof(arg1));
    meng * m2 = meng_create(func, 16 * 1024, &arg2, sizeof(arg2));

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

## 🛠️ 构建与测试

### 环境要求
- CMake 3.15 或更高版本
- GCC 4.8+ / Clang 3.8+ / MSVC 2015+ (支持 C++11 标准)

### 一键构建与测试
```bash
./build.sh           # Debug 模式构建并自动运行单元测试
./build.sh release   # Release 模式构建
```

### 使用标准 CMake 构建
```bash
# 配置构建目录
cmake -B build -DCMAKE_BUILD_TYPE=Release -DMENG_BUILD_TESTS=ON

# 编译
cmake --build build -j

# 运行自动化测试
ctest --test-dir build --output-on-failure

# 运行性能基准测试
./build/bin/benchmark
```

---

## 📦 集成到你的项目中

### 方式 1: CMake `FetchContent` (推荐)
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

### 方式 2: `find_package` (先安装后使用)
```bash
cmake --build build --target install
```
在你的 `CMakeLists.txt` 中：
```cmake
find_package(meng REQUIRED)
target_link_libraries(your_target PRIVATE meng::meng)
```

---

## 📖 API 说明

### C API (`meng.h`)
- `meng * meng_create(meng_main func, size_t stacksize, const void * arg, size_t argsize)`: 创建协程对象，可指定栈大小和传入参数。
- `void meng_run(meng * m)`: 执行或恢复该协程。
- `void meng_yield(meng * m)`: 协程主动让出 CPU 执行权，返回调用者。
- `void meng_yield_current(void)`: 当前运行的协程让出执行权（无需显式持有 `m` 指针）。
- `bool meng_end(meng * m)`: 判断协程是否已运行完毕。
- `void meng_delete(meng * m)`: 销毁协程并释放相关资源。
- `meng * meng_running(void)`: 获取当前线程正在运行的协程指针。
- `void * meng_get_arg(meng * m)`: 获取协程参数缓冲区的指针。
- `size_t meng_get_arg_size(meng * m)`: 获取协程参数大小。
- `size_t meng_get_stack_size(meng * m)`: 获取协程栈容量。

### C++ API (`meng.hpp`)
- `libmeng::coroutine`: 现代 C++ 协程类。
  - `coroutine(Callable&& fn, size_t stack_size = 64 * 1024)`: 构造并绑定任意可调用对象。
  - `bool resume()`: 恢复执行；若有未处理异常会在主线程抛出。
  - `void yield()`: 让出执行权。
  - `bool done() const`: 检查是否结束。
  - 析构函数遵循 RAII 自动释放。
- `libmeng::yield()`: 任意调用深度让出当前协程。
- `libmeng::current()`: 获取当前 `coroutine*`。

---

## 📄 开源许可
MIT License
